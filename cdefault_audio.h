#ifndef CDEFAULT_AUDIO_H_
#define CDEFAULT_AUDIO_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

// TODO: return error codes not b32s?
// TODO: custom audio mixer?
// TODO: support other backends / platforms

typedef S32 AudioStreamHandle;
typedef S32 AudioDeviceHandle;

// TODO: put default audio spec in the device?
typedef struct AudioDevice AudioDevice;
struct AudioDevice {
  AudioDeviceHandle handle;
  String8 name;
  AtomicB32 is_connected;
  AudioDevice* next;
};

typedef enum AudioStreamFormat AudioStreamFormat;
enum AudioStreamFormat {
  AudioStreamFormat_Unknown,
  AudioStreamFormat_F32,
  AudioStreamFormat_S16,
  AudioStreamFormat_S32,
};

// NOTE: Leave values empty to use defaults.
typedef struct AudioStreamSpec AudioStreamSpec;
struct AudioStreamSpec {
  AudioDeviceHandle device_handle;
  U32 channels;
  U32 frequency;
  AudioStreamFormat format;
};
#define AUDIO_DEFAULT_DEVICE 0 // NOTE: Always attach to the default audio device.
#define AUDIO_DEFAULT_FREQUENCY 44100
#define AUDIO_DEFAULT_CHANNELS 2
#define AUDIO_DEFAULT_FORMAT AudioStreamFormat_F32

// NOTE: Must call Init before calling subsequent functions.
B32  AudioInit(void);
void AudioDeinit(void);

// NOTE: Retrieves a singly-linked list of AudioDevices. Owned by the audio system (pls don't free).
// NOTE: You only really need to call this if you want to attach to a specific device, use
// AUDIO_DEFAULT_DEVICE in the audio spec to automatically create streams on the default device.
B32  AudioGetDevices(AudioDevice** devices);

// NOTE: Opens a new audio stream conforming to spec. On success, handle == the newly created stream's handle.
B32  AudioStreamOpen(AudioStreamHandle* handle, AudioStreamSpec spec);
B32  AudioStreamClose(AudioStreamHandle handle);

// NOTE: Retrieves a byte stream in which audio data (conforming to spec) can be placed. Once data has
// been placed, AudioStreamReleaseBuffer must be called.
B32  AudioStreamAcquireBuffer(AudioStreamHandle handle, U8** buffer, U32* buffer_size);
B32  AudioStreamReleaseBuffer(AudioStreamHandle handle);

// NOTE: Waits until the audio stream has space for more audio data.
B32  AudioStreamWait(AudioStreamHandle handle);

#endif // CDEFAULT_AUDIO_H_

#ifdef CDEFAULT_AUDIO_IMPLEMENTATION
#undef CDEFAULT_AUDIO_IMPLEMENTATION

#if defined(OS_WINDOWS)
#  define CDEFAULT_AUDIO_BACKEND_WASAPI
#elif defined(OS_LINUX)
#  define CDEFAULT_AUDIO_BACKEND_PULSEAUDIO
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

// NOTE: https://www.codeproject.com/Articles/13601/COM-in-plain-C
// NOTE: Notification interface that Windows invokes when various events happen.
typedef struct WASAPI_NotificationClient WASAPI_NotificationClient;
struct WASAPI_NotificationClient {
  IMMNotificationClientVtbl* v_table;
  AtomicS32 ref_count;
};

// NOTE: Represents a physical audio device. Once they are registered, they
// remain registered for the duration of the program (e.g. through connections,
// disconnections, reconnections).
typedef struct WASAPI_AudioDevice WASAPI_AudioDevice;
struct WASAPI_AudioDevice {
  AudioDevice base;
  Arena* arena;
  LPWSTR imm_device_id;
};

// NOTE: Represents a stream of audio data. These are paired with a physical device.
// Some configuration options (namely, pairing with the default device) allow
// streams to be moved to other devices, e.g. on device connection events, etc.
typedef struct WASAPI_AudioStream WASAPI_AudioStream;
struct WASAPI_AudioStream {
  AudioStreamHandle handle;
  AudioStreamSpec spec;
  WASAPI_AudioDevice* device;
  AtomicS32 ref_count;
  U32 sample_frames;
  U32 buffer_size;
  HANDLE event;
  IAudioClient* client;
  IAudioRenderClient* render;
  WASAPI_AudioStream* next;
  WASAPI_AudioStream* prev;
};

// NOTE: Global audio context.
typedef struct WASAPI_AudioContext WASAPI_AudioContext;
struct WASAPI_AudioContext {
  B32 initialized;
  Arena* arena;
  Mutex mutex;
  AtomicS32 next_device_id;
  WASAPI_AudioDevice* default_device;
  WASAPI_AudioDevice* devices;
  AtomicS32 next_stream_id;
  // TODO: if there are a lot of streams playing at once, O(n) lookups could be slow?
  WASAPI_AudioStream* streams_front;
  WASAPI_AudioStream* streams_back;
  WASAPI_AudioStream* streams_free_list;
};

#define WASAPI_LOG_ERROR(hresult, fmt, ...)                 \
  do {                                                      \
    char _err_buffer[1024];                                 \
    _err_buffer[0] = '\0';                                  \
    FormatMessageA(                                         \
        FORMAT_MESSAGE_FROM_SYSTEM, NULL, hresult, 0,       \
        _err_buffer, STATIC_ARRAY_SIZE(_err_buffer), NULL);  \
    LOG_ERROR(fmt ": %s", ##__VA_ARGS__, _err_buffer);      \
  } while (0)

// NOTE: UUIDs required for WASAPI. Newer API gives a function to derive them
// but older versions of Windows can't do that, so better to just bake them in.
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
static const GUID _wasapi_GUID_DataFormat_PCM                  = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
static const GUID _wasapi_GUID_DataFormat_Float                = { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
static const ERole _wasapi_role = eConsole;

static WASAPI_AudioContext _wasapi_ctx;
static WASAPI_NotificationClient _wasapi_notification_client;
static IMMDeviceEnumerator* _wasapi_enum = NULL;

static WASAPI_AudioStream* WASAPI_ContextAllocateStream(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness)) {
  WASAPI_AudioStream* stream = NULL;
  if (ctx->streams_free_list != NULL) {
    stream = ctx->streams_free_list;
    SLL_STACK_POP(ctx->streams_free_list, next);
  } else {
    stream = ARENA_PUSH_STRUCT(ctx->arena, WASAPI_AudioStream);
  }
  MEMORY_ZERO_STRUCT(stream);
  AtomicS32Init(&stream->ref_count, 1);
  DLL_PUSH_BACK(ctx->streams_front, ctx->streams_back, stream, prev, next);
  return stream;
}

static void WASAPI_ContextUnrefStream(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), WASAPI_AudioStream* stream) {
  if (AtomicS32FetchSub(&stream->ref_count, 1) - 1 > 0) {
    return;
  }
  if (stream->client != NULL) {
    IAudioClient_Stop(stream->client);
    IAudioClient_Release(stream->client);
  }
  if (stream->render != NULL) {
    IAudioRenderClient_Release(stream->render);
  }
  if (stream->event != NULL) {
    CloseHandle(stream->event);
  }
  DLL_REMOVE(ctx->streams_front, ctx->streams_back, stream, prev, next);
  SLL_STACK_PUSH(ctx->streams_free_list, stream, next);
}

static WASAPI_AudioStream* WASAPI_FindStreamByHandle(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), AudioStreamHandle handle) {
  for (WASAPI_AudioStream* stream = ctx->streams_front; stream != NULL; stream = stream->next) {
    if (stream->handle == handle) {
      return stream;
    }
  }
  return NULL;
}

static WASAPI_AudioDevice* WASAPI_FindDeviceByHandle(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), AudioDeviceHandle handle) {
  if (handle == AUDIO_DEFAULT_DEVICE) {
    return ctx->default_device;
  }
  for (WASAPI_AudioDevice* device = ctx->devices; device != NULL; device = (WASAPI_AudioDevice*) device->base.next) {
    if (device->base.handle == handle) {
      return device;
    }
  }
  return NULL;
}

static WASAPI_AudioDevice* WASAPI_FindDeviceByImmDeviceId(WASAPI_AudioContext* ctx, LockWitness UNUSED(witness), LPWSTR imm_device_id) {
  for (WASAPI_AudioDevice* device = ctx->devices; device != NULL; device = (WASAPI_AudioDevice*) device->base.next) {
    if (wcscmp(device->imm_device_id, imm_device_id) == 0) {
      return device;
    }
  }
  return NULL;
}

static B32 WASAPI_ContextAddDevice(WASAPI_AudioContext* ctx, LockWitness witness, IMMDevice* imm_device, WASAPI_AudioDevice** out_device) {
  WASAPI_AudioDevice* device = NULL;
  LPWSTR imm_device_id = NULL;
  IPropertyStore* imm_device_properties = NULL;
  U64 arena_pos = ctx->arena->pos;
  B32 success = false;
  HRESULT result;

  result = IMMDevice_GetId(imm_device, &imm_device_id);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm_device_id.");
    goto audio_device_initialize_exit;
  }

  // NOTE: E.g. if the device was unplugged, then replugged back in.
  device = WASAPI_FindDeviceByImmDeviceId(ctx, witness, imm_device_id);
  if (device != NULL)  {
    AtomicB32Store(&device->base.is_connected, true);
    success = true;
    goto audio_device_initialize_exit;
  }

  device = ARENA_PUSH_STRUCT(ctx->arena, WASAPI_AudioDevice);
  device->arena = ArenaAllocate();
  device->base.next = (AudioDevice*) ctx->devices;
  ctx->devices = device;

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
  device->base.name.str = ARENA_PUSH_ARRAY(device->arena, U8, utf8_size);
  device->base.name.size = utf8_size;
  WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, (LPSTR) device->base.name.str, utf8_size, NULL, NULL);

  device->base.handle = AtomicS32FetchAdd(&ctx->next_device_id, 1);
  AtomicB32Store(&device->base.is_connected, true);
  LOG_INFO("[AUDIO] Registered device: %d: %.*s", device->base.handle, device->base.name.size, device->base.name.str);
  success = true;

audio_device_initialize_exit:
  if (!success && device != NULL) {
    ArenaRelease(device->arena);
    ArenaPopTo(ctx->arena, arena_pos);
    ctx->devices = (WASAPI_AudioDevice*) ctx->devices->base.next;
    device = NULL;
  }
  if (imm_device_properties != NULL) { IPropertyStore_Release(imm_device_properties);   }
  if (imm_device_id != NULL) { CoTaskMemFree(imm_device_id); }
  if (out_device != NULL) { *out_device = device; }
  return success;
}

B32 WASAPI_ContextAddStream(WASAPI_AudioContext* ctx, LockWitness witness, AudioStreamSpec spec, AudioStreamHandle stream_handle) {
  WASAPI_AudioStream* stream = WASAPI_ContextAllocateStream(ctx, witness);
  stream->handle = stream_handle;

  if (spec.device_handle == 0) { spec.device_handle = AUDIO_DEFAULT_DEVICE; }
  if (spec.channels == 0)      { spec.channels = AUDIO_DEFAULT_CHANNELS; }
  if (spec.frequency == 0)     { spec.frequency = AUDIO_DEFAULT_FREQUENCY; }
  if (spec.format == 0)        { spec.format = AudioStreamFormat_F32; }
  stream->spec = spec;

  IMMDevice* imm_device = NULL;
  HRESULT result;
  B32 success = false;

  WASAPI_AudioDevice* device = WASAPI_FindDeviceByHandle(ctx, witness, spec.device_handle);
  if (device == NULL) {
    goto audio_stream_open_end;
  }
  stream->device = device;

  result = IMMDeviceEnumerator_GetDevice(_wasapi_enum, device->imm_device_id, &imm_device);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm_device for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  result = IMMDevice_Activate(imm_device, &_wasapi_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)& stream->client);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to activate device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  stream->event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (stream->event == NULL) {
    LOG_ERROR("[AUDIO] Failed to create audio event for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  // NOTE: The Windows API will convert and mix the formats if we ask it to (in SHAREMODE_SHARED only).
  // Tell it what audio data format we will be feeding it and enable this behavior.
  WAVEFORMATEXTENSIBLE custom_format;
  MEMORY_ZERO_STRUCT(&custom_format);
  custom_format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  custom_format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
  custom_format.Format.nChannels = (WORD) spec.channels;
  custom_format.Format.nSamplesPerSec = spec.frequency;
  switch (spec.format) {
    case AudioStreamFormat_F32: {
      custom_format.Format.wBitsPerSample = 32;
      custom_format.Samples.wValidBitsPerSample = 32;
      custom_format.SubFormat = _wasapi_GUID_DataFormat_Float;
    } break;
    case AudioStreamFormat_S16: {
      custom_format.Format.wBitsPerSample = 16;
      custom_format.Samples.wValidBitsPerSample = 16;
      custom_format.SubFormat = _wasapi_GUID_DataFormat_PCM;
    } break;
    case AudioStreamFormat_S32: {
      custom_format.Format.wBitsPerSample = 32;
      custom_format.Samples.wValidBitsPerSample = 32;
      custom_format.SubFormat = _wasapi_GUID_DataFormat_PCM;
    } break;
    default: {
      LOG_ERROR("[AUDIO] Unrecognized audio format provided: %d", spec.format);
      UNIMPLEMENTED();
    } break;
  }
  custom_format.Format.nBlockAlign = (custom_format.Format.wBitsPerSample / 8) * custom_format.Format.nChannels;
  custom_format.Format.nAvgBytesPerSec = custom_format.Format.nSamplesPerSec * custom_format.Format.nBlockAlign;

  DWORD stream_flags = 0;
  stream_flags |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
  stream_flags |= AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
  stream_flags |= AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;

  result = IAudioClient_Initialize(stream->client, AUDCLNT_SHAREMODE_SHARED, stream_flags, 0, 0, (WAVEFORMATEX*) &custom_format, NULL);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to initialize the audio client for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  result = IAudioClient_SetEventHandle(stream->client, stream->event);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to set audio event handle for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  REFERENCE_TIME default_period;
  result = IAudioClient_GetDevicePeriod(stream->client, &default_period, NULL);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get default period for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  U32 device_sample_frames;
  result = IAudioClient_GetBufferSize(stream->client, &device_sample_frames);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to determine sample frames for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  F32 default_period_millis = default_period / 10000.0f;
  F32 default_period_frames = default_period_millis * ((F32) spec.frequency) / 1000.0f;
  stream->sample_frames = (U32) F32Ceil(default_period_frames);
  // NOTE: when requesting a buffer, the device needs space for that buffer. if the maximum size of
  // the buffer is requested, the device won't be able to lend space for a subsequent buffer until
  // the existing one is played. this causes skips, because the device plays nothing during the overlap
  // of "i'm out of buffer to play, give me more" and "here's more buffer for you to play".
  // so, ensure that the max number of frames is the device max / 2, so there's space to place the next
  // buffer while the current one is playing. at least that's my theory for what's going on.
  // of course this requires the system be able to feed it data at that speed reliably, so that may be
  // another source of skipping...
  stream->sample_frames = MIN(stream->sample_frames, device_sample_frames / 2);
  stream->buffer_size = stream->sample_frames * custom_format.Format.nBlockAlign;

  result = IAudioClient_GetService(stream->client, &_wasapi_IID_IAudioRenderClient, (void**)&stream->render);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get render client for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  result = IAudioClient_Start(stream->client);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to start client for device: %.*s", device->base.name.size, device->base.name.str);
    goto audio_stream_open_end;
  }

  LOG_INFO("[AUDIO] Successfully created stream on device: %.*s", device->base.name.size, device->base.name.str);
  success = true;

audio_stream_open_end:
  if (!success && stream != NULL) {
    WASAPI_ContextUnrefStream(ctx, witness, stream);
    stream = NULL;
  }
  if (imm_device != NULL) { IMMDevice_Release(imm_device); }
  return success;
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
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm_device on state change");
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
    WASAPI_AudioDevice* device;
    if (WASAPI_ContextAddDevice(ctx, witness, imm_device, &device)) {
      LOG_INFO("[AUDIO] Device connected: %.*s", device->base.name.size, device->base.name.str);
    }
  } else {
    WASAPI_AudioDevice* device = WASAPI_FindDeviceByImmDeviceId(ctx, witness, (LPWSTR) imm_device_id);
    if (device == NULL) {
      LOG_WARN("[AUDIO] Unknown device disconnected.");
      goto on_device_state_changed_exit;
    }
    LOG_INFO("[AUDIO] Device disconnected: %.*s", device->base.name.size, device->base.name.str);
    AtomicB32Store(&device->base.is_connected, false);
    // NOTE: release streams attached to disconnected device.
    // TODO: move them to another device? config option?
    for (WASAPI_AudioStream* stream = ctx->streams_front; stream != NULL; stream = stream->next) {
      if (stream->spec.device_handle == device->base.handle) {
        WASAPI_ContextUnrefStream(ctx, witness, stream);
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

  WASAPI_AudioDevice* device = WASAPI_FindDeviceByImmDeviceId(ctx, witness, (LPWSTR) imm_device_id);
  if (device == NULL) {
    LOG_WARN("[AUDIO] Unknown device marked as new default, attempting to add...");
    IMMDevice* imm_device = NULL;
    HRESULT result = IMMDeviceEnumerator_GetDevice(_wasapi_enum, imm_device_id, &imm_device);
    if (FAILED(result)) {
      WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get imm_device for new default device.");
    } else {
      WASAPI_ContextAddDevice(ctx, witness, imm_device, &device);
    }
    IMMDevice_Release(imm_device);
  }
  ctx->default_device = device;
  // NOTE: reconnect default device streams to attach to new default device.
  if (device != NULL) {
    LOG_INFO("[AUDIO] New default audio device detected: %.*s", device->base.name.size, device->base.name.str);
    for (WASAPI_AudioStream* stream = ctx->streams_front; stream != NULL; stream = stream->next) {
      if (stream->spec.device_handle == AUDIO_DEFAULT_DEVICE) {
        AudioStreamHandle handle = stream->handle;
        AudioStreamSpec spec = stream->spec;
        WASAPI_ContextUnrefStream(ctx, witness, stream);
        if (!WASAPI_ContextAddStream(ctx, witness, spec, handle)) {
          LOG_ERROR("[AUDIO] Failed to move audio stream to new default device, giving up.");
        }
      }
    }
  } else {
    LOG_ERROR("[AUDIO] Failed to recognize new default device.");
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
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to enumerate audio endpoints.");
    goto detect_devices_end;
  }
  UINT num_imm_devices;
  result = IMMDeviceCollection_GetCount(imm_device_collection, &num_imm_devices);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed determine count of connected devices.");
    goto detect_devices_end;
  }
  for (UINT i = 0; i < num_imm_devices; i++) {
    imm_device = NULL;
    result = IMMDeviceCollection_Item(imm_device_collection, i, &imm_device);
    if (FAILED(result)) {
      WASAPI_LOG_ERROR(result, "[AUDIO] Failed to retrieve device from collection.");
      goto detect_devices_continue;
    }
    if (!WASAPI_ContextAddDevice(ctx, witness, imm_device, NULL)) {
      goto detect_devices_continue;
    }
    found_device = true;
detect_devices_continue:
    if (imm_device != NULL) { IMMDevice_Release(imm_device); }
    continue;
  }

  // NOTE: Find default device.
  result = IMMDeviceEnumerator_GetDefaultAudioEndpoint(_wasapi_enum, eRender, _wasapi_role, &imm_device);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to detect default device.");
    goto detect_devices_end;
  }
  result = IMMDevice_GetId(imm_device, &imm_device_id);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to get default device imm_device_id.");
    goto detect_devices_end;
  }
  WASAPI_AudioDevice* default_device = WASAPI_FindDeviceByImmDeviceId(ctx, witness, imm_device_id);
  if (default_device == NULL) {
    LOG_ERROR("[AUDIO] Failed to locate default device in list of registered devices.");
    goto detect_devices_end;
  }
  ctx->default_device = default_device;
  found_default_device = true;
  LOG_INFO("[AUDIO] Detected default device: %.*s", default_device->base.name.size, default_device->base.name.str);

detect_devices_end:
  if (found_device && !found_default_device) {
    LOG_WARN("[AUDIO] Default device not detected, selecting an arbitrary device.");
    ctx->default_device = ctx->devices;
  }

  if (imm_device_collection != NULL) { IMMDeviceCollection_Release(imm_device_collection); }
  if (imm_device != NULL) { IMMDevice_Release(imm_device); }
  if (imm_device_id != NULL) { CoTaskMemFree(imm_device_id); }
}

B32 WASAPI_AudioInit(void) {
  LOG_INFO("[AUDIO] Initializing WASAPI.");
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (ctx->initialized) { return true; }

  HRESULT result;

  result = CoInitialize(NULL);
  if (result != S_OK && result != S_FALSE) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Call to CoInitialize failed.");
    return 1;
  }
  result = CoCreateInstance(&_wasapi_CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &_wasapi_IID_IMMDeviceEnumerator, (LPVOID*)& _wasapi_enum);
  if (FAILED(result)) {
    WASAPI_LOG_ERROR(result, "[AUDIO] Failed to initialize audio device enumerator.");
    return 1;
  }

  ctx->arena = ArenaAllocate();
  MutexInit(&ctx->mutex);
  AtomicS32Init(&ctx->next_device_id, 1);
  AtomicS32Init(&ctx->next_stream_id, 1);
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

  IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(_wasapi_enum, (IMMNotificationClient*)&_wasapi_notification_client);
  AtomicS32FetchSub(&_wasapi_notification_client.ref_count, 1);

  LockWitness witness = MutexLock(&ctx->mutex);
  for (WASAPI_AudioStream* stream = ctx->streams_front; stream != NULL; stream = stream->next) {
    WASAPI_ContextUnrefStream(ctx, witness, stream);
  }
  for (WASAPI_AudioDevice* device = ctx->devices; device != NULL; device = (WASAPI_AudioDevice*) device->base.next) {
    AtomicB32Store(&device->base.is_connected, false);
    ArenaRelease(device->arena);
  }

  ArenaRelease(ctx->arena);
  MutexUnlock(&ctx->mutex);
  MutexDeinit(&ctx->mutex);

  CoUninitialize();
}

B32 WASAPI_AudioGetDevices(AudioDevice** devices) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  *devices = (AudioDevice*) ctx->devices;
  return true;
}

B32 WASAPI_AudioStreamOpen(AudioStreamHandle* handle, AudioStreamSpec spec) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  LockWitness witness = MutexLock(&ctx->mutex);
  *handle = AtomicS32FetchAdd(&ctx->next_stream_id, 1);
  B32 success = WASAPI_ContextAddStream(ctx, witness, spec, *handle);
  MutexUnlock(&ctx->mutex);
  return success;
}

B32 WASAPI_AudioStreamClose(AudioStreamHandle handle) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_AudioStream* stream = WASAPI_FindStreamByHandle(ctx, witness, handle);
  if (stream == NULL) { return false; }
  WASAPI_ContextUnrefStream(ctx, witness, stream);
  MutexUnlock(&ctx->mutex);
  return true;
}


B32 WASAPI_AudioStreamAcquireBuffer(AudioStreamHandle handle, U8** buffer, U32* buffer_size) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  LockWitness witness = MutexLock(&ctx->mutex);
  B32 success = false;
  WASAPI_AudioStream* stream = WASAPI_FindStreamByHandle(ctx, witness, handle);
  if (stream != NULL) {
    // TODO: retries / recovery plan
    ASSERT(stream->render != NULL);
    *buffer_size = stream->buffer_size;
    HRESULT result = IAudioRenderClient_GetBuffer(stream->render, stream->sample_frames, buffer);
    success = SUCCEEDED(result);
  }
  MutexUnlock(&ctx->mutex);
  return success;
}

B32 WASAPI_AudioStreamReleaseBuffer(AudioStreamHandle handle) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }
  LockWitness witness = MutexLock(&ctx->mutex);

  B32 success = false;
  WASAPI_AudioStream* stream = WASAPI_FindStreamByHandle(ctx, witness, handle);
  if (stream != NULL) {
    HRESULT result = IAudioRenderClient_ReleaseBuffer(stream->render, stream->sample_frames, 0);
    success = SUCCEEDED(result);
  }

  MutexUnlock(&ctx->mutex);
  return success;
}

B32 WASAPI_AudioStreamWait(AudioStreamHandle handle) {
  WASAPI_AudioContext* ctx = &_wasapi_ctx;
  if (!ctx->initialized) { return false; }

  LockWitness witness = MutexLock(&ctx->mutex);
  WASAPI_AudioStream* stream = WASAPI_FindStreamByHandle(ctx, witness, handle);
  if (stream == NULL) { return false; }
  ASSERT(AtomicS32FetchAdd(&stream->ref_count, 1) > 0);
  MutexUnlock(&ctx->mutex);

  B32 success = false;
  if (stream == NULL) {
    goto play_buffer_end;
  }
  while (true) {
    DWORD wait_result = WaitForSingleObjectEx(stream->event, 200, FALSE);
    switch (wait_result) {
      case WAIT_OBJECT_0: {
        U32 padding = 0;
        HRESULT result = IAudioClient_GetCurrentPadding(stream->client, &padding);
        if (FAILED(result)) { continue; }
        if (padding <= (U32) stream->sample_frames) {
          success = true;
          goto play_buffer_end;
        }
      } break;
      case WAIT_TIMEOUT: {
        if (!AtomicB32Load(&stream->device->base.is_connected)) {
          success = false;
          goto play_buffer_end;
        }
      } break;
      default: {
        IAudioClient_Stop(stream->client);
        success = false;
        goto play_buffer_end;
      } break;
    }
  }

play_buffer_end:
  witness = MutexLock(&ctx->mutex);
  WASAPI_ContextUnrefStream(ctx, witness, stream);
  MutexUnlock(&ctx->mutex);

  return success;
}

#elif defined(CDEFAULT_AUDIO_BACKEND_PULSEAUDIO)

#include <pulse/pulseaudio.h>

typedef struct PULSEAUDIO_AudioContext PULSEAUDIO_AudioContext;
struct PULSEAUDIO_AudioContext {
  B32 initialized;
  pa_threaded_mainloop* mainloop;
  pa_context* context;
};

static PULSEAUDIO_AudioContext _pulse_context;

static void PULSEAUDIO_ContextStateUpdate(pa_context* UNUSED(c), void* user_data) {
  PULSEAUDIO_AudioContext* ctx = &_pulse_context;
  pa_threaded_mainloop_signal(ctx->mainloop, 0);
}

B32 PULSEAUDIO_AudioInit(void) {
  LOG_INFO("[AUDIO] Initializing PulseAudio.");
  return true;
  PULSEAUDIO_AudioContext* ctx = &_pulse_context;
  if (ctx->initialized) { return true; }

  pa_proplist* proplist = NULL;
  B32 success = false;

  ctx->mainloop = pa_threaded_mainloop_new();
  if (ctx->mainloop = NULL) {
    LOG_ERROR("[AUDIO] Failed to create main loop.");
    goto pulseaudio_init_end;
  }
  if (pa_threaded_mainloop_start(ctx->mainloop) < 0) {
    LOG_ERROR("[AUDIO] Failed to start main loop.");
    goto pulseaudio_init_end;
  }

  pa_threaded_mainloop_lock(ctx->mainloop);

  pa_mainloop_api* mainloop_api = pa_threaded_mainloop_get_api(ctx->mainloop);
  if (mainloop_api == NULL) {
    LOG_ERROR("[AUDIO] Failed to get main loop API.");
    goto pulseaudio_init_end_unlock;
  }

  proplist = pa_proplist_new();
  if (proplist == NULL) {
    LOG_ERROR("[AUDIO] Failed to create proplist.");
    goto pulseaudio_init_end_unlock;
  }
  pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "applications-games"); // TODO: dynamic?

  ctx->context = pa_context_new_with_proplist(mainloop_api, "my-game", proplist);
  if (ctx->context == NULL) {
    LOG_ERROR("[AUDIO] Failed to create context.");
    goto pulseaudio_init_end_unlock;
  }

  pa_context_set_state_callback(ctx->context, PULSEAUDIO_ContextStateUpdate, NULL);
  if (pa_context_connect(ctx->context, NULL, 0, NULL) < 0) {
    LOG_ERROR("[AUDIO] Failed to connect to PulseAudio.");
    goto pulseaudio_init_end_unlock;
  }
  // NOTE: wait until connection with pulseaudio server is established.
  S32 state = pa_context_get_state(ctx->context);
  while (PA_CONTEXT_IS_GOOD(state) && (state != PA_CONTEXT_READY)) {
    pa_threaded_mainloop_wait(ctx->mainloop);
    state = pa_context_get_state(ctx->context);
  }
  if (state != PA_CONTEXT_READY) {
    LOG_ERROR("[AUDIO] Gave up trying to connect to PulseAudio.");
    goto pulseaudio_init_end_unlock;
  }

  ctx->initialized = true;
  success = true;

pulseaudio_init_end_unlock:
  pa_threaded_mainloop_unlock(ctx->mainloop);
pulseaudio_init_end:
  if (!success) {
    if (ctx->context != NULL) { pa_context_disconnect(ctx->context); }
    if (ctx->mainloop != NULL) { pa_threaded_mainloop_free(ctx->mainloop); }
  }
  if (proplist != NULL) { pa_proplist_free(proplist); }
  return success;
}

void PULSEAUDIO_AudioDeinit(void) {
  PULSEAUDIO_AudioContext* ctx = &_pulse_context;
  if (!ctx->initialized) { return; }

  pa_context_disconnect(ctx->context);
  ctx->initialized = false;

  return;
}

B32 PULSEAUDIO_AudioGetDevices(AudioDevice** devices) {
  *devices = NULL;
  return true;
}

B32 PULSEAUDIO_AudioStreamOpen(AudioStreamHandle* handle, AudioStreamSpec spec) {
  return true;
}

B32 PULSEAUDIO_AudioStreamClose(AudioStreamHandle handle) {
  return true;
}

B32 PULSEAUDIO_AudioStreamAcquireBuffer(AudioStreamHandle handle, U8** buffer, U32* buffer_size) {
  *buffer = NULL;
  *buffer_size = 0;
  return true;
}

B32 PULSEAUDIO_AudioStreamReleaseBuffer(AudioStreamHandle handle) {
  return true;
}

B32 PULSEAUDIO_AudioStreamWait(AudioStreamHandle handle) {
  return true;
}

#elif defined(CDEFAULT_AUDIO_BACKEND_FAKE)

B32 FAKE_AudioInit(void) {
  LOG_WARN("[AUDIO] No audio backend specified, installing fake.");
  return true;
}

void FAKE_AudioDeinit(void) {
  return;
}

B32 FAKE_AudioGetDevices(AudioDevice** devices) {
  *devices = NULL;
  return true;
}

B32 FAKE_AudioStreamOpen(AudioStreamHandle* UNUSED(handle), AudioStreamSpec UNUSED(spec)) {
  return true;
}

B32 FAKE_AudioStreamClose(AudioStreamHandle UNUSED(handle)) {
  return true;
}

B32 FAKE_AudioStreamAcquireBuffer(AudioStreamHandle UNUSED(handle), U8** UNUSED(buffer), U32* UNUSED(buffer_size)) {
  *buffer = NULL;
  *buffer_size = 0;
  return true;
}

B32 FAKE_AudioStreamReleaseBuffer(AudioStreamHandle UNUSED(handle)) {
  return true;
}

B32 FAKE_AudioStreamWait(AudioStreamHandle UNUSED(handle)) {
  return true;
}

#else
#error CDEFAULT_AUDIO: Unknown or unsupported driver detected.
#endif

#if defined(CDEFAULT_AUDIO_BACKEND_WASAPI)
#error hi
#  define CDEFAULT_AUDIO_BACKEND_NAMESPACE WASAPI_
#elif defined(CDEFAULT_AUDIO_BACKEND_PULSEAUDIO)
#  define CDEFAULT_AUDIO_BACKEND_NAMESPACE PULSEAUDIO_
#elif defined(CDEFAULT_AUDIO_BACKEND_FAKE)
#  define CDEFAULT_AUDIO_BACKEND_NAMESPACE FAKE_
#else
#  error No cdefault audio driver specified.
#endif
#define CDEFAULT_AUDIO_BACKEND_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_AUDIO_BACKEND_FN(x) CDEFAULT_AUDIO_BACKEND_FN_IMPL(CDEFAULT_AUDIO_BACKEND_NAMESPACE, x)

B32 AudioInit(void) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioInit());
}

void AudioDeinit(void) {
  CDEFAULT_AUDIO_BACKEND_FN(AudioDeinit());
}

B32 AudioGetDevices(AudioDevice** devices) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioGetDevices(devices));
}

B32 AudioStreamOpen(AudioStreamHandle* handle, AudioStreamSpec spec) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioStreamOpen(handle, spec));
}

B32 AudioStreamClose(AudioStreamHandle handle) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioStreamClose(handle));
}

B32 AudioStreamAcquireBuffer(AudioStreamHandle handle, U8** buffer, U32* buffer_size) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioStreamAcquireBuffer(handle, buffer, buffer_size));
}

B32 AudioStreamReleaseBuffer(AudioStreamHandle handle) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioStreamReleaseBuffer(handle));
}

B32 AudioStreamWait(AudioStreamHandle handle) {
  return CDEFAULT_AUDIO_BACKEND_FN(AudioStreamWait(handle));
}

#endif // CDEFAULT_AUDIO_IMPLEMENTATION
