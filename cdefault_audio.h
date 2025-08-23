#ifndef CDEFAULT_AUDIO_H_
#define CDEFAULT_AUDIO_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

// TODO: support more than just F32 format
// TODO: manual selection of devices
// TODO: double check failure case logic
// TODO: query information about device
// TODO: support other backends

B32  AudioInit(void);
void AudioDeinit(void);
B32  AudioGetDefaultDevice(S32* device_id);
B32  AudioDeviceGetBuffer(S32 device_id, U8** buffer, U32* buffer_size);
B32  AudioDevicePlayBuffer(S32 device_id);
B32  AudioDeviceWait(S32 device_id);

#endif // CDEFAULT_AUDIO_H_

#ifdef CDEFAULT_AUDIO_IMPLEMENTATION
#undef CDEFAULT_AUDIO_IMPLEMENTATION

#ifdef OS_WINDOWS
#  define CDEFAULT_AUDIO_BACKEND_WASAPI
#else
#  define CDEFAULT_AUDIO_BACKEND_FAKE
#endif

#if defined(CDEFAULT_AUDIO_BACKEND_WASAPI)

#define COBJMACROS
#include <windows.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <objbase.h>
#include <audioclient.h>
#include <wchar.h> // TODO: add string16 to std.

// https://www.codeproject.com/Articles/13601/COM-in-plain-C
typedef struct WASAPI_NotificationClient WASAPI_NotificationClient;
struct WASAPI_NotificationClient {
  IMMNotificationClientVtbl* v_table;
  AtomicS32 ref_count;
};

// TODO: this probably needs a mutex, or some other way to handle byzantine failures.
typedef struct WASAPI_AudioDevice WASAPI_AudioDevice;
struct WASAPI_AudioDevice {
  Arena* arena;
  AtomicS32 ref_count;
  S32 id;
  B32 dead;
  LPWSTR imm_device_id;
  String8 name;
  U32 sample_frames;
  U64 buffer_size;
  HANDLE event;
  IAudioClient* client;
  IAudioRenderClient* render;
  WASAPI_AudioDevice* prev;
  WASAPI_AudioDevice* next;
};

typedef struct WASAPI_AudioContext WASAPI_AudioContext;
struct WASAPI_AudioContext {
  Arena* arena;
  Mutex mutex;
  AtomicS32 next_device_id;
  WASAPI_AudioDevice* default_device;
  WASAPI_AudioDevice* devices_front;
  WASAPI_AudioDevice* devices_back;
  WASAPI_AudioDevice* devices_free_list;
  B32 initialized;
};

#define WASAPI_LOG_ERROR(hresult, fmt, ...)                 \
  do {                                                      \
    char _err_buffer[1024];                                 \
    _err_buffer[0] = '\0';                                  \
    FormatMessageA(                                         \
        FORMAT_MESSAGE_FROM_SYSTEM, NULL, hresult, 0,       \
        _err_buffer, STATIC_ARRAY_LEN(_err_buffer), NULL);  \
    LOG_ERROR(fmt ": %s", ##__VA_ARGS__, _err_buffer);      \
  } while (0)

// NOTE: UUIDs required for WASAPI.
static const IID _wasapi_IID_IAudioRenderClient                = { 0xf294acfc, 0x3146, 0x4483, { 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2 } };
static const IID _wasapi_IID_IAudioCaptureClient               = { 0xc8adbd64, 0xe71e, 0x48a0, { 0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17 } };
static const IID _wasapi_IID_IAudioClient                      = { 0x1cb9ad4c, 0xdbfa, 0x4c32, { 0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2 } };
static const IID _wasapi_IID_IMMDeviceEnumerator               = { 0xa95664d2, 0x9614, 0x4f35, { 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6 } };
static const IID _wasapi_IID_IMMNotificationClient             = { 0x7991eec9, 0x7e89, 0x4d85, { 0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0 } };
static const IID _wasapi_IID_IMMEndpoint                       = { 0x1be09788, 0x6894, 0x4089, { 0x85, 0x86, 0x9a, 0x2a, 0x6c, 0x26, 0x5a, 0xc5 } };
static const PROPERTYKEY _wasapi_PKEY_Device_FriendlyName      = { { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, } }, 14 };
static const PROPERTYKEY _wasapi_PKEY_AudioEngine_DeviceFormat = { { 0xf19f064d, 0x82c,  0x4e27, { 0xbc, 0x73, 0x68, 0x82, 0xa1, 0xbb, 0x8e, 0x4c, } }, 0  };
static const PROPERTYKEY _wasapi_PKEY_AudioEndpoint_GUID       = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, } }, 4  };
static const CLSID _wasapi_CLSID_MMDeviceEnumerator            = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e } };
static const ERole _wasapi_role = eConsole;

static WASAPI_AudioContext _wasapi_ctx;
static WASAPI_NotificationClient _wasapi_notification_client;
static IMMDeviceEnumerator* _wasapi_enum = NULL;

static WASAPI_AudioDevice* WASAPI_ContextAllocateDevice(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness)) {
  WASAPI_AudioDevice* device = NULL;
  if (ctx->devices_free_list != NULL) {
    device = ctx->devices_free_list;
    SLL_STACK_POP(ctx->devices_free_list, next);
  } else {
    device = ARENA_PUSH_STRUCT(ctx->arena, WASAPI_AudioDevice);
  }
  MEMORY_ZERO_STRUCT(device);
  device->arena = ArenaAllocate();
  AtomicS32Init(&device->ref_count, 1);
  DLL_PUSH_BACK(ctx->devices_front, ctx->devices_back, device, prev, next);
  return device;
}

static void WASAPI_ContextUnrefDevice(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), WASAPI_AudioDevice* device) {
  if (AtomicS32FetchSub(&device->ref_count, 1) - 1 > 0) {
    return;
  }

  LOG_INFO("[AUDIO] Releasing device: %.*s", device->name.size, device->name.str);
  ArenaRelease(device->arena);
  if (device->client != NULL) {
    IAudioClient_Stop(device->client);
    IAudioClient_Release(device->client);
  }
  if (device->render != NULL) {
    IAudioRenderClient_Release(device->render);
  }
  if (device->event != NULL) {
    CloseHandle(device->event);
  }
  DLL_REMOVE(ctx->devices_front, ctx->devices_back, device, prev, next);
  SLL_STACK_PUSH(ctx->devices_free_list, device, next);
}

static WASAPI_AudioDevice* WASAPI_ContextFindByDeviceId(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), S32 device_id) {
  for (WASAPI_AudioDevice* device = ctx->devices_front; device != NULL; device = device->next) {
    if (device->id == device_id) {
      return device;
    }
  }
  return NULL;
}

static WASAPI_AudioDevice* WASAPI_ContextFindByImmDeviceId(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), LPWSTR imm_device_id) {
  for (WASAPI_AudioDevice* device = ctx->devices_front; device != NULL; device = device->next) {
    if (wcscmp(device->imm_device_id, imm_device_id) == 0) {
      return device;
    }
  }
  return NULL;
}

static B32 WASAPI_ContextTryAddDevice(WASAPI_AudioContext* ctx, LockWitness witness, IMMDevice* imm_device, WASAPI_AudioDevice** out_device) {
  WASAPI_AudioDevice* device = NULL;
  HRESULT result;
  B32 success = false;
  LPWSTR imm_device_id = NULL;
  IPropertyStore* imm_device_properties = NULL;

  result = IMMDevice_GetId(imm_device, &imm_device_id);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm_device_id.");
    goto audio_device_initialize_exit;
  }

  // TODO: is this the best place to put this? should there be different behavior?
  WASAPI_AudioDevice* duplicate_device = WASAPI_ContextFindByImmDeviceId(ctx, witness, imm_device_id);
  if (duplicate_device != NULL)  {
    LOG_WARN("[AUDIO] Duplicate device detected: %.*s", duplicate_device->name.size, duplicate_device->name.str);
    WASAPI_ContextUnrefDevice(ctx, witness, duplicate_device);
  }
  device = WASAPI_ContextAllocateDevice(ctx, witness);

  U64 imm_device_id_len = wcslen(imm_device_id) + 1;
  device->imm_device_id = ARENA_PUSH_ARRAY(device->arena, wchar_t, imm_device_id_len);
  wcscpy_s(device->imm_device_id, imm_device_id_len, imm_device_id);

  result = IMMDevice_OpenPropertyStore(imm_device, STGM_READ, &imm_device_properties);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to open device property store.");
    goto audio_device_initialize_exit;
  }

  PROPVARIANT var;
  PropVariantInit(&var);
  result = IPropertyStore_GetValue(imm_device_properties, &_wasapi_PKEY_Device_FriendlyName, &var);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to read device name.");
    goto audio_device_initialize_exit;
  }
  S32 utf8_size = WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, NULL, 0, NULL, NULL) - 1;
  device->name.str = ARENA_PUSH_ARRAY(device->arena, U8, utf8_size);
  device->name.size = utf8_size;
  WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, (LPSTR) device->name.str, utf8_size, NULL, NULL);
  LOG_INFO("[AUDIO] Attempting to add device: %.*s", device->name.size, device->name.str);

  result = IMMDevice_Activate(imm_device, &_wasapi_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)& device->client);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to activate device: %.*s", device->name.size, device->name.str);
    goto audio_device_initialize_exit;
  }

  device->event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (device->event == NULL) {
    LOG_ERROR("[AUDIO] Failed to create audio event.");
    goto audio_device_initialize_exit;
  }

  WAVEFORMATEX* format = NULL;
  result = IAudioClient_GetMixFormat(device->client, &format);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get mix format for device: %.*s", device->name.size, device->name.str);
    goto audio_device_initialize_exit;
  }

  DWORD stream_flags = 0;
  stream_flags |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
  // TODO, search replace 44100
  // if ((DWORD) device->spec.frequency != format->nSamplesPerSec)
  if (44100 != format->nSamplesPerSec) {
    stream_flags |= AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
    stream_flags |= AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
    format->nSamplesPerSec = 44100;
    format->nAvgBytesPerSec = format->nSamplesPerSec * format->nChannels * (format->wBitsPerSample / 8);
  }
  result = IAudioClient_Initialize(device->client, AUDCLNT_SHAREMODE_SHARED, stream_flags, 0, 0, format, NULL);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to initialize the audio client for device: %.*s", device->name.size, device->name.str);
    goto audio_device_initialize_exit;
  }

  result = IAudioClient_SetEventHandle(device->client, device->event);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to set audio event handle.");
    goto audio_device_initialize_exit;
  }

  REFERENCE_TIME default_period;
  result = IAudioClient_GetDevicePeriod(device->client, &default_period, NULL);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get default period for device: %.*s", device->name.size, device->name.str);
    goto audio_device_initialize_exit;
  }

  U32 device_sample_frames;
  result = IAudioClient_GetBufferSize(device->client, &device_sample_frames);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to determine sample frames for device: %.*s", device->name.size, device->name.str);
    goto audio_device_initialize_exit;
  }

  F32 default_period_millis = default_period / 10000.0f;
  F32 default_period_frames = default_period_millis * 44100.0f / 1000.0f;
  device->sample_frames = (U32) F32Ceil(default_period_frames);
  // NOTE: when requesting a buffer, the device needs space for that buffer. if the maximum size of
  // the buffer is requested, the device won't be able to lend space for a subsequent buffer until
  // the existing one is played. this causes skips, because the device plays nothing during this overlap.
  // so, ensure that the max number of frames is the device max / 2, so there's space to place the next
  // buffer while the current one is playing.
  device->sample_frames = MIN(device->sample_frames, device_sample_frames / 2);
  // TODO: configurable formats?
  device->buffer_size = device->sample_frames * sizeof(F32) * format->nChannels;

  result = IAudioClient_GetService(device->client, &_wasapi_IID_IAudioRenderClient, (void**)&device->render);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get render client for device: %.*s", device->name.size, device->name.str);
    goto audio_device_initialize_exit;
  }

  result = IAudioClient_Start(device->client);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to start client for device: %.*s", device->name.size, device->name.str);
    goto audio_device_initialize_exit;
  }

  device->dead = false;
  device->id = AtomicS32FetchAdd(&ctx->next_device_id, 1);
  LOG_INFO("[AUDIO] Successfully added device: %.*s", device->name.size, device->name.str);
  success = true;

audio_device_initialize_exit:
  if (imm_device_properties != NULL) { IPropertyStore_Release(imm_device_properties);   }
  if (imm_device_id != NULL) { CoTaskMemFree(imm_device_id); }
  if (!success && device != NULL) { WASAPI_ContextUnrefDevice(ctx, witness, device); device = NULL; }
  if (out_device != NULL) { *out_device = device; }
  return success;
}

static void WASAPI_ContextDetectDevices(WASAPI_AudioContext* ctx, LockWitness witness) {
  IMMDeviceCollection* imm_device_collection = NULL;
  IMMDevice* imm_device = NULL;
  LPWSTR imm_device_id = NULL;
  HRESULT result;
  B32 found_device = false;
  B32 found_default_device = false;

  // NOTE: Detect currently connected devices.
  result = IMMDeviceEnumerator_EnumAudioEndpoints(_wasapi_enum, eRender, DEVICE_STATE_ACTIVE, &imm_device_collection);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to enumerate audio endpoints.");
    goto detect_devices_end;
  }
  UINT num_imm_devices;
  result = IMMDeviceCollection_GetCount(imm_device_collection, &num_imm_devices);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable determine count of connected devices.");
    goto detect_devices_end;
  }
  for (WASAPI_AudioDevice* device = ctx->devices_front; device != NULL; device = device->next) {
    WASAPI_ContextUnrefDevice(ctx, witness, device);
  }
  for (UINT i = 0; i < num_imm_devices; i++) {
    imm_device = NULL;
    B32 success = false;
    result = IMMDeviceCollection_Item(imm_device_collection, i, &imm_device);
    if (FAILED(result)) {
      WASAPI_LOG_ERROR(result, "[AUDIO] Unable to retrieve device from collection.");
      goto detect_devices_cleanup_and_continue;
    }
    if (!WASAPI_ContextTryAddDevice(ctx, witness, imm_device, NULL)) {
      goto detect_devices_cleanup_and_continue;
    }
    success = true;
    found_device = true;
detect_devices_cleanup_and_continue:
    if (imm_device != NULL) { IMMDevice_Release(imm_device); }
    continue;
  }

  // NOTE: Find default device.
  result = IMMDeviceEnumerator_GetDefaultAudioEndpoint(_wasapi_enum, eRender, _wasapi_role, &imm_device);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to detect default device.");
    goto detect_devices_end;
  }
  result = IMMDevice_GetId(imm_device, &imm_device_id);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to get default device imm_device_id.");
    goto detect_devices_end;
  }
  WASAPI_AudioDevice* default_device = WASAPI_ContextFindByImmDeviceId(ctx, witness, imm_device_id);
  if (default_device == NULL) {
    LOG_ERROR("[AUDIO] Unable to locate default device in list of registered devices.");
    goto detect_devices_end;
  }
  ctx->default_device = default_device;
  found_default_device = true;
  LOG_INFO("[AUDIO] Detected default device: %.*s", default_device->name.size, default_device->name.str);

detect_devices_end:
  if (found_device && !found_default_device) {
    LOG_WARN("[AUDIO] Default device not detected, selecting an arbitrary device.");
    ctx->default_device = ctx->devices_front;
  }

  if (imm_device_collection != NULL) { IMMDeviceCollection_Release(imm_device_collection); }
  if (imm_device != NULL) { IMMDevice_Release(imm_device); }
  if (imm_device_id != NULL) { CoTaskMemFree(imm_device_id); }
}

static HRESULT WASAPI_NotificationClientQueryInterface(IMMNotificationClient* client, REFIID iid, void** ppv) {
  if (MEMORY_COMPARE(iid, &IID_IUnknown, sizeof(*iid)) != 0 &&
      MEMORY_COMPARE(iid, &_wasapi_IID_IMMNotificationClient, sizeof(*iid)) != 0) {
    return E_NOINTERFACE;
  }
  *ppv = client;
  client->lpVtbl->AddRef(client);
  return S_OK;
}

static ULONG WASAPI_NotificationClientAddRef(IMMNotificationClient* client) {
  S32 result = AtomicS32FetchAdd(&((WASAPI_NotificationClient*)client)->ref_count, 1);
  ASSERT(result >= 0);
  return ((ULONG) result + 1);
}

static ULONG WASAPI_NotificationClientRelease(IMMNotificationClient* client) {
  S32 result = AtomicS32FetchSub(&((WASAPI_NotificationClient*)client)->ref_count, 1);
  ASSERT(result > 0);
  return ((ULONG) result - 1);
}

static HRESULT WASAPI_NotificationClientOnDeviceStateChanged(IMMNotificationClient* UNUSED(client), LPCWSTR imm_device_id, DWORD state) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  LockWitness witness = MutexLock(&ctx->mutex);

  IMMDevice* imm_device = NULL;
  IMMEndpoint* imm_endpoint = NULL;
  HRESULT result;

  result = IMMDeviceEnumerator_GetDevice(_wasapi_enum, imm_device_id, &imm_device);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to get imm_device on state change");
    goto on_device_state_changed_exit;
  }

  result = IMMDevice_QueryInterface(imm_device, &_wasapi_IID_IMMEndpoint, (void**) &imm_endpoint);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm device endpoint.");
    goto on_device_state_changed_exit;
  }

  EDataFlow flow;
  result = IMMEndpoint_GetDataFlow(imm_endpoint, &flow);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm data flow.");
    goto on_device_state_changed_exit;
  }
  // NOTE: only care about playback devices.
  if (flow != eRender) { goto on_device_state_changed_exit; }

  if (state == DEVICE_STATE_ACTIVE) {
    WASAPI_ContextTryAddDevice(ctx, witness, imm_device, NULL);
  } else {
    WASAPI_AudioDevice* device = WASAPI_ContextFindByImmDeviceId(ctx, witness, (LPWSTR) imm_device_id);
    if (device == NULL) {
      goto on_device_state_changed_exit;
    }
    device->dead = true;
    WASAPI_ContextUnrefDevice(ctx, witness, device);
    if (device == ctx->default_device) {
      LOG_WARN("[AUDIO] Unexpected disconnection of default device, attempting to choose an arbitrary replacement.");
      ctx->default_device = ctx->devices_front;
      if (ctx->default_device == NULL) {
        LOG_WARN("[AUDIO] No devices found.");
      }
    }
  }

on_device_state_changed_exit:
  MutexUnlock(&ctx->mutex);
  if (imm_device != NULL) { IMMDevice_Release(imm_device); }
  if (imm_endpoint != NULL) { IMMEndpoint_Release(imm_endpoint); }
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnDeviceAdded(IMMNotificationClient* UNUSED(client), LPCWSTR UNUSED(imm_device_id)) {
  // NOTE: Handled in OnDeviceStateChange.
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnDeviceRemoved(IMMNotificationClient* UNUSED(client), LPCWSTR UNUSED(imm_device_id)) {
  // NOTE: Handled in OnDeviceStateChange.
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnDefaultDeviceChanged(IMMNotificationClient* UNUSED(client), EDataFlow UNUSED(flow), ERole role, LPCWSTR imm_device_id) {
  if (role != _wasapi_role) { return S_OK; }
  if (flow != eRender) { return S_OK; }

  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  LockWitness witness = MutexLock(&ctx->mutex);

  WASAPI_AudioDevice* device = WASAPI_ContextFindByImmDeviceId(ctx, witness, (LPWSTR) imm_device_id);
  if (device == NULL) {
    LOG_WARN("[AUDIO] Unknown device marked as new default, attempting to add...");
    IMMDevice* imm_device = NULL;
    HRESULT result = IMMDeviceEnumerator_GetDevice(_wasapi_enum, imm_device_id, &imm_device);
    if (FAILED(result)) {
      WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm_device for new default device.");
    } else {
      WASAPI_ContextTryAddDevice(ctx, witness, imm_device, &device);
    }
    IMMDevice_Release(imm_device);
  }
  if (device != NULL) {
    LOG_INFO("[AUDIO] New default audio device detected: %.*s", device->name.size, device->name.str);
    ctx->default_device = device;
  } else {
    LOG_ERROR("[AUDIO] Unable to add new default device.");
  }

  MutexUnlock(&ctx->mutex);
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnPropertyValueChanged(IMMNotificationClient* UNUSED(client), LPCWSTR UNUSED(imm_device_id), const PROPERTYKEY UNUSED(key)) {
  // NOTE: Not needed.
  return S_OK;
}

static IMMNotificationClientVtbl _wasapi_notification_client_vtable = {
  WASAPI_NotificationClientQueryInterface,
  WASAPI_NotificationClientAddRef,
  WASAPI_NotificationClientRelease,
  WASAPI_NotificationClientOnDeviceStateChanged,
  WASAPI_NotificationClientOnDeviceAdded,
  WASAPI_NotificationClientOnDeviceRemoved,
  WASAPI_NotificationClientOnDefaultDeviceChanged,
  WASAPI_NotificationClientOnPropertyValueChanged,
};

B32 WASAPI_AudioGetDefaultDevice(S32* device_id) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  MutexLock(&ctx->mutex);

  B32 success = false;
  if (ctx->default_device != NULL) {
    *device_id = ctx->default_device->id;
    success = true;
  }

  MutexUnlock(&ctx->mutex);
  return success;
}

B32 WASAPI_AudioDeviceGetBuffer(S32 device_id, U8** buffer, U32* buffer_size) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  LockWitness witness = MutexLock(&ctx->mutex);

  B32 success = false;
  WASAPI_AudioDevice* device = WASAPI_ContextFindByDeviceId(ctx, witness, device_id);
  if (device != NULL) {
    // TODO: retries / recovery plan
    ASSERT(device->render != NULL);
    *buffer_size = (U32) device->buffer_size;
    HRESULT result = IAudioRenderClient_GetBuffer(device->render, device->sample_frames, buffer);
    success = SUCCEEDED(result);
  }

  MutexUnlock(&ctx->mutex);
  return success;
}

B32 WASAPI_AudioDevicePlayBuffer(S32 device_id) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  LockWitness witness = MutexLock(&ctx->mutex);

  B32 success = false;
  WASAPI_AudioDevice* device = WASAPI_ContextFindByDeviceId(ctx, witness, device_id);
  if (device != NULL) {
    HRESULT result = IAudioRenderClient_ReleaseBuffer(device->render, device->sample_frames, 0);
    success = SUCCEEDED(result);
  }

  MutexUnlock(&ctx->mutex);
  return success;
}

B32 WASAPI_AudioDeviceWait(S32 device_id) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }

  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_AudioDevice* device = WASAPI_ContextFindByDeviceId(ctx, witness, device_id);
  ASSERT(AtomicS32FetchAdd(&device->ref_count, 1) > 0);
  MutexUnlock(&ctx->mutex);

  B32 success = false;
  if (device == NULL) {
    goto play_buffer_end;
  }
  while (true) {
    if (device->dead) {
      success = false;
      goto play_buffer_end;
    }

    DWORD wait_result = WaitForSingleObjectEx(device->event, 200, FALSE);
    switch (wait_result) {
      case WAIT_TIMEOUT: continue;
      case WAIT_OBJECT_0: {
        U32 padding = 0;
        ASSERT(device->ref_count > 0);
        HRESULT result = IAudioClient_GetCurrentPadding(device->client, &padding);
        if (FAILED(result)) { continue; }
        if (padding <= (U32) device->sample_frames) {
          success = true;
          goto play_buffer_end;
        }
      } break;
      default: {
        IAudioClient_Stop(device->client);
        success = false;
        goto play_buffer_end;
      } break;
    }
  }

play_buffer_end:
  witness = MutexLock(&ctx->mutex);
  WASAPI_ContextUnrefDevice(ctx, witness, device);
  MutexUnlock(&ctx->mutex);

  return success;
}

B32 WASAPI_AudioInit(void) {
  LOG_INFO("[AUDIO] Initializing WASAPI.");

  HRESULT result;

  result = CoInitialize(NULL);
  if (result != S_OK && result != S_FALSE) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Call to CoInitialize failed.");
    return 1;
  }
  result = CoCreateInstance(&_wasapi_CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &_wasapi_IID_IMMDeviceEnumerator, (LPVOID*)& _wasapi_enum);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to initialize audio device enumerator.");
    return 1;
  }

  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  MEMORY_ZERO_STRUCT(ctx);
  ctx->arena = ArenaAllocate();
  MutexInit(&ctx->mutex);
  AtomicS32Init(&ctx->next_device_id, 1);
  ctx->initialized = false;
  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_ContextDetectDevices(ctx, witness);

  MEMORY_ZERO_STRUCT(&_wasapi_notification_client);
  _wasapi_notification_client.v_table = &_wasapi_notification_client_vtable;
  AtomicS32Init(&_wasapi_notification_client.ref_count, 1);
  IMMDeviceEnumerator_RegisterEndpointNotificationCallback(_wasapi_enum, (IMMNotificationClient*)&_wasapi_notification_client);

  ctx->initialized = true;
  MutexUnlock(&ctx->mutex);

  return true;
}

void WASAPI_AudioDeinit(void) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return; }

  LockWitness witness = MutexLock(&ctx->mutex);
  for (WASAPI_AudioDevice* device = ctx->devices_front; device != NULL; device = device->next) {
    WASAPI_ContextUnrefDevice(ctx, witness, device);
  }
  ArenaRelease(ctx->arena);
  MutexUnlock(&ctx->mutex);
  MutexDeinit(&ctx->mutex);

  IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(_wasapi_enum, (IMMNotificationClient*)&_wasapi_notification_client);
  CoUninitialize();
}

#elif defined(CDEFAULT_AUDIO_BACKEND_FAKE)

// TODO.

#else
# error CDEFAULT_AUDIO: Unknown or unsupported driver detected.
#endif

#if defined(CDEFAULT_AUDIO_BACKEND_WASAPI)
# define CDEFAULT_AUDIO_BACKEND_NAMESPACE WASAPI_
#elif defined(CDEFAULT_AUDIO_BACKEND_FAKE)
# define CDEFAULT_AUDIO_BACKEND_NAMESPACE FAKE_
#else
# error No cdefault audio driver specified.
#endif
#define CDEFAULT_AUDIO_BACKEND_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_AUDIO_BACKEND_FN(x) CDEFAULT_AUDIO_BACKEND_FN_IMPL(CDEFAULT_AUDIO_BACKEND_NAMESPACE, x)

B32 AudioInit(void) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioInit());
}

void AudioDeinit(void) {
  CDEFAULT_AUDIO_BACKEND_FN(AudioDeinit());
}

B32 AudioGetDefaultDevice(S32* device_id) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioGetDefaultDevice(device_id));
}

B32 AudioDeviceGetBuffer(S32 device_id, U8** buffer, U32* buffer_size) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioDeviceGetBuffer(device_id, buffer, buffer_size));
}

B32 AudioDevicePlayBuffer(S32 device_id) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioDevicePlayBuffer(device_id));
}

B32 AudioDeviceWait(S32 device_id) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioDeviceWait(device_id));
}

#endif // CDEFAULT_AUDIO_IMPLEMENTATION
