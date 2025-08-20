#ifndef CDEFAULT_AUDIO_H_
#define CDEFAULT_AUDIO_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

typedef struct AudioSpec AudioSpec;
struct AudioSpec {
  S32 frequency;
  U8 channels;
};

// TODO: query for device that matches spec
// TODO: automatically handle device closure
// TODO: query information about device
B32  AudioInit(void);
void AudioDeinit(void);
B32  AudioDeviceActivate(S32 device_id);
B32  AudioDeviceMaybeGetBuffer(S32 device_id, U8** buffer);
B32  AudioDevicePlayBuffer(S32 device_id);

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
  WASAPI_AudioDevice* next;

  // NOTE: Available once the device has first been detected.
  S32 id;
  LPWSTR imm_device_id;
  String8 name;
  AudioSpec spec;
  WAVEFORMATEX* format;

  // NOTE: Only available once the device has been activated.
  U32 sample_frames;
  U64 buffer_size;
  HANDLE event;
  IAudioClient* client;
  IAudioRenderClient* render;
};

typedef struct WASAPI_AudioContext WASAPI_AudioContext;
struct WASAPI_AudioContext {
  Arena* arena;
  Mutex mutex;
  AtomicS32 next_device_id;
  WASAPI_AudioDevice* devices;
  WASAPI_AudioDevice* devices_free_list;
  AtomicB32 initialized;
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
static const IID _wasapi_IID_IMMNotificationClient             = { 0x7991eec9, 0x7e89, 0x4d85,{ 0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0 } };
static const PROPERTYKEY _wasapi_PKEY_Device_FriendlyName      = { { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, } }, 14 };
static const PROPERTYKEY _wasapi_PKEY_AudioEngine_DeviceFormat = { { 0xf19f064d, 0x82c,  0x4e27, { 0xbc, 0x73, 0x68, 0x82, 0xa1, 0xbb, 0x8e, 0x4c, } }, 0  };
static const PROPERTYKEY _wasapi_PKEY_AudioEndpoint_GUID       = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, } }, 4  };
static const CLSID _wasapi_CLSID_MMDeviceEnumerator            = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e } };

static WASAPI_AudioContext _wasapi_ctx;
static WASAPI_NotificationClient _wasapi_notification_client;
static Thread _wasapi_thread;
static AtomicB32 _wasapi_thread_terminate;
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
  return device;
}

static void WASAPI_ContextReleaseDevice(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), WASAPI_AudioDevice* device) {
  ArenaRelease(device->arena);
  SLL_STACK_PUSH(ctx->devices_free_list, device, next);
}

static WASAPI_AudioDevice* WASAPI_ContextFindByDeviceId(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), S32 device_id) {
  for (WASAPI_AudioDevice* device = ctx->devices; device != NULL; device = device->next) {
    if (device->id == device_id) {
      return device;
    }
  }
  return NULL;
}

static WASAPI_AudioDevice* WASAPI_ContextFindByImmDeviceId(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), LPWSTR imm_device_id) {
  for (WASAPI_AudioDevice* device = ctx->devices; device != NULL; device = device->next) {
    if (wcscmp(device->imm_device_id, imm_device_id) == 0) {
      return device;
    }
  }
  return NULL;
}

static B32 WASAPI_ContextTryAddDevice(WASAPI_AudioContext* ctx, LockWitness witness, IMMDevice* imm_device) {
  WASAPI_AudioDevice* device = WASAPI_ContextAllocateDevice(ctx, witness);
  HRESULT result;
  B32 success = false;
  LPWSTR imm_device_id                  = NULL;
  IPropertyStore* imm_device_properties = NULL;

  result = IMMDevice_GetId(imm_device, &imm_device_id);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to get imm_device_id.");
    goto audio_device_initialize_exit;
  }
  U64 imm_device_id_len = wcslen(imm_device_id) + 1;
  device->imm_device_id = ARENA_PUSH_ARRAY(device->arena, wchar_t, imm_device_id_len);
  wcscpy_s(device->imm_device_id, imm_device_id_len, imm_device_id);

  result = IMMDevice_OpenPropertyStore(imm_device, STGM_READ, &imm_device_properties);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to open device property store.");
    goto audio_device_initialize_exit;
  }

  PROPVARIANT var;
  PropVariantInit(&var);
  result = IPropertyStore_GetValue(imm_device_properties, &_wasapi_PKEY_Device_FriendlyName, &var);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to read device name.");
    goto audio_device_initialize_exit;
  }
  S32 utf8_size = WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, NULL, 0, NULL, NULL) - 1;
  device->name.str = ARENA_PUSH_ARRAY(device->arena, U8, utf8_size);
  device->name.size = utf8_size;
  WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, (LPSTR) device->name.str, utf8_size, NULL, NULL);

  PropVariantClear(&var);
  result = IPropertyStore_GetValue(imm_device_properties, &_wasapi_PKEY_AudioEngine_DeviceFormat, &var);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to read device format.");
    goto audio_device_initialize_exit;
  }
  WAVEFORMATEXTENSIBLE format;
  MEMORY_ZERO_STRUCT(&format);
  MEMORY_COPY(&format, var.blob.pBlobData, MIN(sizeof(WAVEFORMATEXTENSIBLE), sizeof(var.blob.cbSize)));
  device->spec.channels = (U8) format.Format.nChannels;
  device->spec.frequency = format.Format.nSamplesPerSec;

  device->id = AtomicS32FetchAdd(&ctx->next_device_id, 1);

  // TODO: is this the best place to put this? should there be different behavior?
  WASAPI_AudioDevice* duplicate_device = WASAPI_ContextFindByImmDeviceId(ctx, witness, imm_device_id);
  if (duplicate_device != NULL) {
    // TEMP log?
    LOG_WARN("[AUDIO] Duplicate device detected: %.*s", duplicate_device->name.size, duplicate_device->name.str);
    WASAPI_ContextReleaseDevice(ctx, witness, duplicate_device);
  }
  SLL_STACK_PUSH(ctx->devices, device, next);

  success = true;
  LOG_INFO("[AUDIO] Added device: %.*s", device->name.size, device->name.str);

audio_device_initialize_exit:
  if (imm_device_properties != NULL) { IPropertyStore_Release(imm_device_properties);   }
  if (imm_device_id != NULL) { CoTaskMemFree(imm_device_id); }
  if (!success) { WASAPI_ContextReleaseDevice(ctx, witness, device); }
  return success;
}

static void WASAPI_ContextDetectDevices(WASAPI_AudioContext* ctx, LockWitness witness) {
  IMMDeviceCollection* imm_device_collection = NULL;

  HRESULT result;
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

  for (WASAPI_AudioDevice* device = ctx->devices; device != NULL; device = device->next) {
    WASAPI_ContextReleaseDevice(ctx, witness, device);
  }

  for (UINT i = 0; i < num_imm_devices; i++) {
    IMMDevice* imm_device = NULL;
    B32 success = false;

    result = IMMDeviceCollection_Item(imm_device_collection, i, &imm_device);
    if (FAILED(result)) {
      WASAPI_LOG_ERROR(result, "[AUDIO] Unable to retrieve device from collection.");
      goto detect_devices_cleanup_and_continue;
    }

    if (!WASAPI_ContextTryAddDevice(ctx, witness, imm_device)) {
      goto detect_devices_cleanup_and_continue;
    }

    success = true;

detect_devices_cleanup_and_continue:
    if (imm_device != NULL) { IMMDevice_Release(imm_device); }
    continue;
  }

detect_devices_end:
  if (imm_device_collection != NULL) { IMMDeviceCollection_Release(imm_device_collection); }
  return;
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
  IMMDevice* imm_device = NULL;
  HRESULT result;

  result = IMMDeviceEnumerator_GetDevice(_wasapi_enum, imm_device_id, &imm_device);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to get imm_device on state change");
    goto on_device_state_changed_exit;
  }

  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  LockWitness witness = MutexLock(&ctx->mutex);
  if (state == DEVICE_STATE_ACTIVE) {
    // TODO: react to failure?
    WASAPI_ContextTryAddDevice(ctx, witness, imm_device);
  } else {
    UNIMPLEMENTED();
  }
  MutexUnlock(&ctx->mutex);

on_device_state_changed_exit:
  if (imm_device != NULL) { IMMDevice_Release(imm_device); }
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnDeviceAdded(IMMNotificationClient* client, LPCWSTR imm_device_id) {
  // NOTE: Handled in OnDeviceStateChange.
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnDeviceRemoved(IMMNotificationClient* client, LPCWSTR imm_device_id) {
  // NOTE: Handled in OnDeviceStateChange.
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnDefaultDeviceChanged(IMMNotificationClient* client, EDataFlow flow, ERole role, LPCWSTR imm_device_id) {
  UNIMPLEMENTED();
  return S_OK;
}

static HRESULT WASAPI_NotificationClientOnPropertyValueChanged(IMMNotificationClient* client, LPCWSTR imm_device_id, const PROPERTYKEY key) {
  UNIMPLEMENTED();
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

static S32 WASAPI_MainThread(void* args) {
  Sem* init_done = (Sem*) args;
  LOG_INFO("[AUDIO] Initializing WASAPI.");

  HRESULT result;
  result = CoInitialize(NULL);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Call to CoInitialize failed.");
    SemSignal(init_done);
    return 1;
  }

  result = CoCreateInstance(&_wasapi_CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &_wasapi_IID_IMMDeviceEnumerator, (LPVOID*)& _wasapi_enum);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to initialize audio device enumerator.");
    SemSignal(init_done);
    return 1;
  }

  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_ContextDetectDevices(ctx, witness);
  AtomicB32Store(&ctx->initialized, true);
  MutexUnlock(&ctx->mutex);

  MEMORY_ZERO_STRUCT(&_wasapi_notification_client);
  _wasapi_notification_client.v_table = &_wasapi_notification_client_vtable;
  AtomicS32Init(&_wasapi_notification_client.ref_count, 1);
  IMMDeviceEnumerator_RegisterEndpointNotificationCallback(_wasapi_enum, (IMMNotificationClient*)&_wasapi_notification_client);

  SemSignal(init_done);

  while (!AtomicB32Load(&_wasapi_thread_terminate)) {
  }

  CoUninitialize();
  return 0;
}

// TODO: disconnect device on failure to activate?
B32 WASAPI_AudioDeviceActivate(S32 device_id) {
  IMMDevice* imm_device = NULL;
  HRESULT result;

  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_AudioDevice* device = WASAPI_ContextFindByDeviceId(ctx, witness, device_id);
  MutexUnlock(&ctx->mutex);
  if (device == NULL) { return false; }

  result = IMMDeviceEnumerator_GetDevice(_wasapi_enum, device->imm_device_id, &imm_device);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get device: %.*s", device->name.size, device->name.str);
    return false;
  }

  result = IMMDevice_Activate(imm_device, &_wasapi_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)& device->client);
  IMMDevice_Release(imm_device);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to activate device: %.*s", device->name.size, device->name.str);
    return false;
  }

  device->event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (device->event == NULL) {
    LOG_ERROR("[AUDIO] Failed to create event for device: %.*s", device->name.size, device->name.str);
    return false;
  }

  WAVEFORMATEX* format = NULL;
  result = IAudioClient_GetMixFormat(device->client, &format);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get mix format for device: %.*s", device->name.size, device->name.str);
    return false;
  }
  device->format = format;
  device->spec.channels = (U8) format->nChannels;

  // TODO: test / verify formats

  REFERENCE_TIME period;
  result = IAudioClient_GetDevicePeriod(device->client, &period, NULL);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get minimumx period for device: %.*s", device->name.size, device->name.str);
    return false;
  }

  DWORD stream_flags = 0;
  stream_flags |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
  if ((DWORD) device->spec.frequency != format->nSamplesPerSec) {
    stream_flags |= AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
    stream_flags |= AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
    // TODO:
    // format->nSamplesPerSec = device->spec.frequency;
    // format->nAvgBytesPerSec = format->nSamplesPerSec * format->nChannels * (format->wBitsPerSample / 8);
  }
  result = IAudioClient_Initialize(device->client, AUDCLNT_SHAREMODE_SHARED, stream_flags, 0, 0, format, NULL);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to initialize the audio client for device: %.*s", device->name.size, device->name.str);
    return false;
  }

  result = IAudioClient_SetEventHandle(device->client, device->event);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to set event handle for device: %.*s", device->name.size, device->name.str);
    return false;
  }

  U32 buffer_size;
  result = IAudioClient_GetBufferSize(device->client, &buffer_size);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to determine buffer size for device: %.*s", device->name.size, device->name.str);
    return false;
  }

  F32 period_millis = period / 10000.0f;
  F32 period_frames = period_millis * format->nSamplesPerSec / 1000.0f;
  U32 sample_frames = (U32) F32Ceil(period_frames);
  sample_frames = CLAMP_TOP(sample_frames, buffer_size);
  device->sample_frames = sample_frames;
  device->buffer_size = sample_frames * format->nSamplesPerSec * format->nChannels;

  result = IAudioClient_GetService(device->client, &_wasapi_IID_IAudioRenderClient, (void**)&device->render);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Unable to get render client for device: %.*s", device->name.size, device->name.str);
    return false;
  }

  result = IAudioClient_Start(device->client);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to start client for device: %.*s", device->name.size, device->name.str);
    return false;
  }

  LOG_INFO("[AUDIO] Activated device: %.*s", device->name.size, device->name.str);

  return true;
}

B32 WASAPI_AudioDeviceMaybeGetBuffer(S32 device_id, U8** buffer) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_AudioDevice* device = WASAPI_ContextFindByDeviceId(ctx, witness, device_id);
  MutexUnlock(&ctx->mutex);
  if (device == NULL) { return false; }

  ASSERT(device->render != NULL);
  // TODO: retries / recovery plan
  HRESULT result = IAudioRenderClient_GetBuffer(device->render, device->sample_frames, buffer);
  return SUCCEEDED(result);
}

B32 WASAPI_AudioDevicePlayBuffer(S32 device_id) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_AudioDevice* device = WASAPI_ContextFindByDeviceId(ctx, witness, device_id);
  MutexUnlock(&ctx->mutex);
  if (device == NULL) { return false; }

  HRESULT result = IAudioRenderClient_ReleaseBuffer(device->render, device->sample_frames, 0);
  return SUCCEEDED(result);
}

B32 WASAPI_AudioInit(void) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  MEMORY_ZERO_STRUCT(ctx);
  ctx->arena = ArenaAllocate();
  MutexInit(&ctx->mutex);
  AtomicS32Init(&ctx->next_device_id, 0);
  ctx->devices = NULL;
  AtomicB32Init(&ctx->initialized, false);

  AtomicB32Init(&_wasapi_thread_terminate, false);
  Sem init_done; SemInit(&init_done, 0);
  ThreadCreate(&_wasapi_thread, WASAPI_MainThread, &init_done);
  SemWait(&init_done); SemDeinit(&init_done);

  return AtomicB32Load(&ctx->initialized);
}

void WASAPI_AudioDeinit(void) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return; }

  AtomicB32Store(&_wasapi_thread_terminate, true);
  ThreadJoin(&_wasapi_thread);

  LockWitness witness = MutexLock(&ctx->mutex);
  for (WASAPI_AudioDevice* device = ctx->devices; device != NULL; device = device->next) {
    WASAPI_ContextReleaseDevice(ctx, witness, device);
  }
  ArenaRelease(ctx->arena);
  MutexUnlock(&ctx->mutex);
  MutexDeinit(&ctx->mutex);
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

B32 AudioDeviceActivate(S32 device_id) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioDeviceActivate(device_id));
}

B32 AudioDeviceMaybeGetBuffer(S32 device_id, U8** buffer) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioDeviceMaybeGetBuffer(device_id, buffer));
}

B32 AudioDevicePlayBuffer(S32 device_id) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioDevicePlayBuffer(device_id));
}

#endif // CDEFAULT_AUDIO_IMPLEMENTATION
