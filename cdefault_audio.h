#ifndef CDEFAULT_AUDIO_H_
#define CDEFAULT_AUDIO_H_

#include "cdefault_std.h"

///////////////////////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////////////////////

typedef S32 AudioDeviceID;

B8 AudioInit(void);
void AudioDeinit(void);

///////////////////////////////////////////////////////////////////////////////
// Internal API
///////////////////////////////////////////////////////////////////////////////

typedef struct AudioSpec AudioSpec;
struct AudioSpec {
  S32 frequency;
  U8 channels;
};

typedef struct AudioDevice AudioDevice;
struct AudioDevice {
  Arena* arena;
  AudioDeviceID id;
  String8 name;
  AudioSpec spec;
  void* handle;
  AudioDevice* next;
};

typedef struct AudioDriverWorkItem AudioDriverWorkItem;
struct AudioDriverWorkItem {
  ThreadStartFunc* entry;
  void* args;
  AudioDriverWorkItem* next;
};

typedef struct AudioDriver AudioDriver;
struct AudioDriver {
  Arena* arena;
  Mutex mutex;
  CV cv;
  AtomicU32 next_device_id;
  AudioDevice* devices;
  AudioDriverWorkItem* pending_work_items;
  AudioDriverWorkItem* free_work_items;
  B8 initialized;
};

typedef struct AudioContext AudioContext;
struct AudioContext {
  AudioDriver driver;
  Thread thread;
  B8 thread_terminate;
  B8 initialized;
};

void AudioDeviceInit(AudioDevice* device);
void AudioDeviceDeinit(AudioDevice* device);
void AudioDriverRegisterDevice(AudioDriver* driver, AudioDevice* device);
AudioDevice* AudioDriverGetDevice(AudioDriver* driver, AudioDeviceID id);

#ifdef OS_WINDOWS
// NOTE: need to link with ole32.lib.
#  define CDEFAULT_AUDIO_DRIVER_WASAPI
#else
#  define CDEFAULT_AUDIO_DRIVER_FAKE
#endif

#if defined(CDEFAULT_AUDIO_DRIVER_WASAPI)
# define CDEFAULT_AUDIO_DRIVER_NAMESPACE WASAPI_
#elif defined(CDEFAULT_AUDIO_DRIVER_FAKE)
# define CDEFAULT_AUDIO_DRIVER_NAMESPACE FAKE_
#else
# error No cdefault audio driver specified.
#endif
#define CDEFAULT_AUDIO_DRIVER_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_AUDIO_DRIVER_FN(x) CDEFAULT_AUDIO_DRIVER_FN_IMPL(CDEFAULT_AUDIO_DRIVER_NAMESPACE, x)
B8 CDEFAULT_AUDIO_DRIVER_FN(AudioDriverInitialize)(AudioDriver* driver);
void CDEFAULT_AUDIO_DRIVER_FN(AudioDriverDetectDevices)(AudioDriver* driver);

#endif // CDEFAULT_AUDIO_H_

#ifdef CDEFAULT_AUDIO_IMPLEMENTATION
#undef CDEFAULT_AUDIO_IMPLEMENTATION

#if defined(CDEFAULT_AUDIO_DRIVER_WASAPI)

#define COBJMACROS
#include <windows.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <objbase.h>
#include <audioclient.h>
#include <wchar.h> // TODO: add string16 to std.

// NOTE: UUIDs required for WASAPI.
static const IID _cdef_IID_IAudioRenderClient                = { 0xf294acfc, 0x3146, 0x4483, { 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2 } };
static const IID _cdef_IID_IAudioCaptureClient               = { 0xc8adbd64, 0xe71e, 0x48a0, { 0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17 } };
static const IID _cdef_IID_IAudioClient                      = { 0x1cb9ad4c, 0xdbfa, 0x4c32, { 0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2 } };
static const IID _cdef_IID_IMMDeviceEnumerator               = { 0xa95664d2, 0x9614, 0x4f35, { 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6 } };
static const PROPERTYKEY _cdef_PKEY_Device_FriendlyName      = { { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, } }, 14 };
static const PROPERTYKEY _cdef_PKEY_AudioEngine_DeviceFormat = { { 0xf19f064d, 0x82c,  0x4e27, { 0xbc, 0x73, 0x68, 0x82, 0xa1, 0xbb, 0x8e, 0x4c, } }, 0  };
static const PROPERTYKEY _cdef_PKEY_AudioEndpoint_GUID       = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, } }, 4  };
static const CLSID _cdef_CLSID_MMDeviceEnumerator            = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e } };

static IMMDeviceEnumerator* _cdef_audio_enum = NULL;

typedef struct WASAPI_Handle WASAPI_Handle;
struct WASAPI_Handle {
  GUID d_sound_guid;
  LPWSTR imm_device_id;
};

B8 WASAPI_AudioDriverInitialize(AudioDriver* driver) {
  LOG_INFO("[AUDIO] Initializing WASAPI.");

  HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  if (result == RPC_E_CHANGED_MODE) { result = CoInitializeEx(NULL, COINIT_MULTITHREADED); }
  if (FAILED(result)) {
    LOG_ERROR("[AUDIO] Unable to initialize windows concurrency model.");
    return false;
  }

  if (FAILED(CoCreateInstance(&_cdef_CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &_cdef_IID_IMMDeviceEnumerator, (LPVOID*)&_cdef_audio_enum))) {
    LOG_ERROR("[AUDIO] Unable to initialize audio device enumerator.");
    return false;
  }

  MutexLock(&driver->mutex);
  driver->initialized = true;
  MutexUnlock(&driver->mutex);
  return true;
}

void WASAPI_AudioDriverDetectDevices(AudioDriver* driver) {
  ASSERT(driver->initialized);

  IMMDeviceCollection* imm_devices = NULL;
  if (FAILED(IMMDeviceEnumerator_EnumAudioEndpoints(_cdef_audio_enum, eRender, DEVICE_STATE_ACTIVE, &imm_devices))) {
    LOG_WARN("[AUDIO] Unable to enumerate audio endpoints.");
    return;
  }

  UINT num_imm_devices;
  if (FAILED(IMMDeviceCollection_GetCount(imm_devices, &num_imm_devices))) {
    LOG_WARN("[AUDIO] Unable determine count of connected devices.");
    IMMDeviceCollection_Release(imm_devices);
    return;
  }

  MutexLock(&driver->mutex);
  for (UINT i = 0; i < num_imm_devices; i++) {
    IMMDevice* imm_device                 = NULL;
    LPWSTR imm_device_id                  = NULL;
    IPropertyStore* imm_device_properties = NULL;
    B8 success = false; U64 pop_to = driver->arena->pos;

    AudioDevice* device = ARENA_PUSH_STRUCT(driver->arena, AudioDevice);
    AudioDeviceInit(device);
    device->handle = ARENA_PUSH_STRUCT(device->arena, WASAPI_Handle);
    MEMORY_ZERO(device->handle, sizeof(WASAPI_Handle));
    WASAPI_Handle* device_handle = (WASAPI_Handle*) device->handle;

    // NOTE: Retrieve device.
    if (FAILED(IMMDeviceCollection_Item(imm_devices, i, &imm_device))) {
      LOG_WARN("[AUDIO] Unable to retrieve device from collection.");
      goto cleanup_and_continue;
    }
    if (FAILED(IMMDevice_GetId(imm_device, &imm_device_id))) {
      LOG_WARN("[AUDIO] Unable to get device id.");
      goto cleanup_and_continue;
    }
    U64 imm_device_id_len = wcslen(imm_device_id) + 1;
    device_handle->imm_device_id = ARENA_PUSH_ARRAY(device->arena, wchar_t, imm_device_id_len);
    wcscpy_s(device_handle->imm_device_id, imm_device_id_len, imm_device_id);

    // NOTE: Retrieve device properties
    if (FAILED(IMMDevice_OpenPropertyStore(imm_device, STGM_READ, &imm_device_properties))) {
      LOG_WARN("[AUDIO] Unable to open device property store.");
      goto cleanup_and_continue;
    }

    PROPVARIANT var;
    PropVariantInit(&var);
    if (FAILED(IPropertyStore_GetValue(imm_device_properties, &_cdef_PKEY_Device_FriendlyName, &var))) {
      LOG_WARN("[AUDIO] Unable to read device name.");
      goto cleanup_and_continue;
    }
    S32 utf8_size = WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, NULL, 0, NULL, NULL) - 1;
    device->name.str = ARENA_PUSH_ARRAY(device->arena, U8, utf8_size);
    device->name.size = utf8_size;
    WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, (LPSTR) device->name.str, utf8_size, NULL, NULL);

    PropVariantClear(&var);
    if (FAILED(IPropertyStore_GetValue(imm_device_properties, &_cdef_PKEY_AudioEngine_DeviceFormat, &var))) {
      LOG_WARN("[AUDIO] Unable to read device format.");
      goto cleanup_and_continue;
    }
    WAVEFORMATEXTENSIBLE format;
    MEMORY_ZERO_STRUCT(&format);
    MEMORY_COPY(&format, var.blob.pBlobData, MIN(sizeof(WAVEFORMATEXTENSIBLE), sizeof(var.blob.cbSize)));
    device->spec.channels = (U8) format.Format.nChannels;
    device->spec.frequency = format.Format.nSamplesPerSec;

    PropVariantClear(&var);
    if (FAILED(IPropertyStore_GetValue(imm_device_properties, &_cdef_PKEY_AudioEndpoint_GUID, &var))) {
      LOG_WARN("[AUDIO] Unable to read DSound GUID.");
      goto cleanup_and_continue;
    }
    CLSIDFromString(var.pwszVal, &device_handle->d_sound_guid);
    PropVariantClear(&var);

    success = true;
    AudioDriverRegisterDevice(driver, device);

    LOG_INFO("[AUDIO] Detected device: %.*s", device->name.size, device->name.str);

cleanup_and_continue:
    if (!success)                      { ArenaPopTo(driver->arena, pop_to);             }
    if (imm_device != NULL)            { IMMDevice_Release(imm_device);                 }
    if (imm_device_id != NULL)         { CoTaskMemFree(imm_device_id);                  }
    if (imm_device_properties != NULL) { IPropertyStore_Release(imm_device_properties); }
    continue;
  }
  MutexUnlock(&driver->mutex);
}

#elif defined(CDEFAULT_AUDIO_DRIVER_FAKE)

B8 FAKE_AudioDriverInitialize(AudioDriver* driver) {
  driver->initialized = true;
  LOG_WARN("[AUDIO] No supported audio driver implementation found, installing fakes.");
  return true;
}
void FAKE_AudioDriverDetectDevices(AudioDriver* driver) {}

#else
# error CDEFAULT_AUDIO: Unknown or unsupported driver detected.
#endif

static AudioContext _cdef_audio_context;

static S32 AudioThreadEntry(void* user_data) {
  Sem* init_done = (Sem*) user_data;
  AudioContext* ctx = &_cdef_audio_context;

  // NOTE: initialize audio driver.
  ctx->driver.arena = ArenaAllocate();
  MutexInit(&ctx->driver.mutex);
  CVInit(&ctx->driver.cv);
  CDEFAULT_AUDIO_DRIVER_FN(AudioDriverInitialize)(&ctx->driver);
  CDEFAULT_AUDIO_DRIVER_FN(AudioDriverDetectDevices)(&ctx->driver);
  SemSignal(init_done);

  // NOTE: audio driver main loop.
  AudioDriverWorkItem* work = NULL;
  while (true) {
    MutexLock(&ctx->driver.mutex);
    while (ctx->driver.pending_work_items == NULL && !ctx->thread_terminate) {
      CVWait(&ctx->driver.cv, &ctx->driver.mutex);
    }
    if (ctx->thread_terminate) { return 0; }
    ASSERT(ctx->driver.pending_work_items != NULL);
    work = ctx->driver.pending_work_items;
    SLL_STACK_POP(ctx->driver.pending_work_items, next);
    SLL_STACK_PUSH(ctx->driver.free_work_items, work, next);
    MutexUnlock(&ctx->driver.mutex);
    (work->entry)(work->args);
  }
  return 0;
}

static void AudioDriverPushWork(AudioDriver* driver, ThreadStartFunc* entry, void* args) {
  MutexLock(&driver->mutex);

  AudioDriverWorkItem* work = NULL;
  if (driver->free_work_items != NULL) {
    work = driver->free_work_items;
    SLL_STACK_POP(driver->free_work_items, next);
  } else {
    work = ARENA_PUSH_STRUCT(driver->arena, AudioDriverWorkItem);
  }
  work->entry = entry;
  work->args = args;
  SLL_STACK_PUSH(driver->pending_work_items, work, next);

  MutexUnlock(&driver->mutex);
  CVSignal(&driver->cv);
}

void AudioDeviceInit(AudioDevice* device) {
  MEMORY_ZERO_STRUCT(device);
  device->arena = ArenaAllocate();
}

void AudioDeviceDeinit(AudioDevice* device) {
  ArenaRelease(device->arena);
}

void AudioDriverRegisterDevice(AudioDriver* driver, AudioDevice* device) {
  // todo: check if handle is already present, or something?
  device->id = AtomicU32FetchAdd(&driver->next_device_id, 1);
  SLL_STACK_PUSH(driver->devices, device, next);
}

AudioDevice* AudioDriverGetDevice(AudioDriver* driver, AudioDeviceID id) {
  for (AudioDevice* device = driver->devices; device != NULL; device = device->next) {
    if (device->id == id) { return device; }
  }
  return NULL;
}

B8 AudioInit(void) {
  AudioContext* ctx = &_cdef_audio_context;
  MEMORY_ZERO_STRUCT(ctx);
  ctx->thread_terminate = false;

  Sem init_done; SemInit(&init_done, 0);
  ThreadCreate(&ctx->thread, AudioThreadEntry, &init_done);
  SemWait(&init_done); SemDeinit(&init_done);

  LOG_INFO("[AUDIO] Audio initialized.");
  return true;
}

void AudioDeinit(void) {
  AudioContext* ctx = &_cdef_audio_context;
  if (!ctx->initialized) { return; }

  ctx->thread_terminate = true;
  MutexLock(&ctx->driver.mutex);
  CVSignal(&ctx->driver.cv);
  MutexUnlock(&ctx->driver.mutex);
  ThreadJoin(&ctx->thread);

  MutexDeinit(&ctx->driver.mutex);
  CVDeinit(&ctx->driver.cv);
  ArenaRelease(ctx->driver.arena);
}

#endif // CDEFAULT_AUDIO_IMPLEMENTATION
