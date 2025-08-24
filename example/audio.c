#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"
#include "third_party/stb_vorbis.c"

int main(void) {
  ASSERT(AudioInit());

  S32 stb_err = 0;
  stb_vorbis* vorbis = stb_vorbis_open_filename("Z:\\cdefault\\example\\data\\test_audio.ogg", &stb_err, NULL);
  ASSERT(stb_err == 0);
  stb_vorbis_info vorbis_info = stb_vorbis_get_info(vorbis);

  AudioStreamSpec spec;
  MEMORY_ZERO_STRUCT(&spec);
  spec.device_handle = AUDIO_DEFAULT_DEVICE;
  spec.channels = vorbis_info.channels;
  spec.frequency = vorbis_info.sample_rate;
  spec.format = AudioStreamFormat_F32;
  AudioStreamHandle stream;
  ASSERT(AudioStreamOpen(&stream, spec));

  U8* buffer;
  U32 buffer_size;
  while(true) {
    if (!AudioStreamAcquireBuffer(stream, &buffer, &buffer_size)) { continue; }
    uint32_t read = stb_vorbis_get_samples_float_interleaved(
        vorbis, vorbis_info.channels, (F32*) buffer, buffer_size / sizeof(F32));
    if (!AudioStreamReleaseBuffer(stream)) { continue; }
    if (read == 0) { stb_vorbis_seek_start(vorbis); }
    if (!AudioStreamWait(stream)) { continue; }
  }

  AudioStreamClose(stream);
  stb_vorbis_close(vorbis);
  AudioDeinit();
}
