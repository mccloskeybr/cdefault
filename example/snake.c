#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"
#include "third_party/stb_vorbis.c"

#define TILE_SIZE     62
#define TILE_PAD      1
#define TILE_RADIUS   20   // NOTE: rounded corner radius
#define NUM_TILES     10   // NOTE: number of tiles per row or column.
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

#define GRID_COLOR  (V3) { 0.8f, 0.8f, 0.8f }
#define SNAKE_COLOR (V3) { 0.0f, 1.0f, 0.0f }
#define APPLE_COLOR (V3) { 1.0f, 0.0f, 0.0f }

typedef struct SnakeSegment SnakeSegment;
struct SnakeSegment {
  S32 x, y;
  SnakeSegment* next;
  SnakeSegment* prev;
};
SnakeSegment segments_memory[NUM_TILES * NUM_TILES];
static S32 next_segment_index = 0;

typedef struct Snake Snake;
struct Snake {
  V2 dir;
  SnakeSegment* head;
  SnakeSegment* tail;
};

typedef struct SoundEffect SoundEffect;
struct SoundEffect {
  stb_vorbis* vorbis;
  B32 is_looping;
  SoundEffect* next;
  SoundEffect* prev;
};
SoundEffect sound_memory[64];
static S32 next_sound_index = 0;

typedef struct AudioManager AudioManager;
struct AudioManager {
  Mutex mutex;
  AudioStreamHandle stream;
  SoundEffect* head;
  SoundEffect* tail;
  SoundEffect* free_sounds;
};
static AudioManager audio_manager;

static RandomSeries r;
static S32 apple_x, apple_y;

static S32 AudioEntry(void* user_data) {
  Notif* init_done = (Notif*) user_data;
  ASSERT(AudioInit());
  MutexInit(&audio_manager.mutex);
  NotifSignal(init_done);
  Arena* arena = ArenaAllocate();

  AudioStreamSpec spec;
  MEMORY_ZERO_STRUCT(&spec);
  spec.device_handle = AUDIO_DEFAULT_DEVICE;
  spec.channels = 2;
  spec.frequency = 44100;
  spec.format = AudioStreamFormat_F32;
  ASSERT(AudioStreamOpen(&audio_manager.stream, spec));

  U8* audio_buffer;
  U32 audio_buffer_size;
  while (true) {
    if (!AudioStreamAcquireBuffer(audio_manager.stream, &audio_buffer, &audio_buffer_size)) { continue; }
    MEMORY_ZERO(audio_buffer, audio_buffer_size);

    U8* temp_buffer = ARENA_PUSH_ARRAY(arena, U8, audio_buffer_size);
    MutexLock(&audio_manager.mutex);
    SoundEffect* effect = audio_manager.tail;
    while (effect != NULL) {
      MEMORY_ZERO(temp_buffer, audio_buffer_size);
      U32 read = stb_vorbis_get_samples_float_interleaved(
          effect->vorbis, spec.channels, (F32*) temp_buffer, audio_buffer_size / sizeof(F32));
      for (S32 i = 0; i < audio_buffer_size / sizeof(F32); i++) {
        ((F32*) audio_buffer)[i] += ((F32*) temp_buffer)[i];
      }
      if (read == 0) {
        if (effect->is_looping) {
          stb_vorbis_seek_start(effect->vorbis);
          effect = effect->next;
          continue;
        }
        else {
          SoundEffect* next_effect = effect->next;
          stb_vorbis_close(effect->vorbis);
          DLL_REMOVE(audio_manager.head, audio_manager.tail, effect, prev, next);
          SLL_STACK_PUSH(audio_manager.free_sounds, effect, next);
          effect = next_effect;
          continue;
        }
      } else {
        effect = effect->next;
        continue;
      }
    }
    MutexUnlock(&audio_manager.mutex);
    ArenaClear(arena);

    if (!AudioStreamReleaseBuffer(audio_manager.stream, audio_buffer, audio_buffer_size)) { continue; }
    if (!AudioStreamWait(audio_manager.stream)) { continue; }
  }

  AudioStreamClose(audio_manager.stream);
  AudioDeinit();
  return 0;
}

static void AudioQueueSound(char* sound, B32 is_looping) {
  MutexLock(&audio_manager.mutex);

  SoundEffect* effect;
  if (audio_manager.free_sounds != NULL) {
    effect = audio_manager.free_sounds;
    SLL_STACK_POP(audio_manager.free_sounds, next);
  } else {
    ASSERT(next_sound_index < STATIC_ARRAY_SIZE(sound_memory));
    effect = &sound_memory[next_sound_index++];
  }
  DLL_PUSH_FRONT(audio_manager.head, audio_manager.tail, effect, prev, next);

  S32 stb_err = 0;
  effect->vorbis = stb_vorbis_open_filename(sound, &stb_err, NULL);
  ASSERT(stb_err == 0);

  effect->is_looping = is_looping;

  MutexUnlock(&audio_manager.mutex);
}

static void DrawTile(S32 x, S32 y, V3 color) {
  S32 center_x = WINDOW_WIDTH / 2;
  S32 center_y = WINDOW_HEIGHT / 2;
  S32 play_area_width = (TILE_SIZE + TILE_PAD * 2) * NUM_TILES;
  S32 play_area_height = (TILE_SIZE + TILE_PAD * 2) * NUM_TILES;
  S32 start_x = center_x - play_area_width / 2;
  S32 start_y = center_y - play_area_height / 2;
  DrawRoundedRectangle(
    start_x + (x * (TILE_SIZE + TILE_PAD) + TILE_PAD),
    start_y + (y * (TILE_SIZE + TILE_PAD) + TILE_PAD),
    TILE_SIZE, TILE_SIZE,
    TILE_RADIUS,
    color.r, color.g, color.b);
}

static void PickAppleLocation(Snake* snake) {
  B32 intersects_snake;
  do {
    apple_x = RandS32(&r, 0, NUM_TILES);
    apple_y = RandS32(&r, 0, NUM_TILES);

    intersects_snake = false;
    for (SnakeSegment* node = snake->tail; node != NULL; node = node->next) {
      if (node->x == apple_x && node->y == apple_y) {
        intersects_snake = true;
        break;
      }
    }
  } while (intersects_snake);
}

static B32 MoveSnake(Snake* snake) {
  SnakeSegment* head = snake->head;
  SnakeSegment* tail = snake->tail;
  DLL_POP_BACK(snake->head, snake->tail, prev, next);

  S32 orig_x = tail->x;
  S32 orig_y = tail->y;
  tail->x = head->x + snake->dir.x;
  tail->y = head->y + snake->dir.y;
  if      (tail->x < 0)          { tail->x = NUM_TILES - 1; }
  else if (tail->x >= NUM_TILES) { tail->x = 0;             }
  if      (tail->y < 0)          { tail->y = NUM_TILES - 1; }
  else if (tail->y >= NUM_TILES) { tail->y = 0;             }

  if (tail->x == apple_x && tail->y == apple_y) {
    AudioQueueSound("Z:\\cdefault\\example\\data\\apple.ogg", false);
    SnakeSegment* node = &segments_memory[next_segment_index++];
    node->x = orig_x;
    node->y = orig_y;
    DLL_PUSH_BACK(snake->head, snake->tail, node, prev, next);
    PickAppleLocation(snake);
  }

  for (SnakeSegment* node = snake->tail; node != NULL; node = node->next) {
    if (node->x == tail->x && node->y == tail->y) { return false; }
  }

  DLL_PUSH_FRONT(snake->head, snake->tail, tail, prev, next);
  return true;
}

int main(void) {
  RandInit(&r, 12345);
  ASSERT(WindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, "snake"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  Notif audio_init_done;
  NotifInit(&audio_init_done);
  Thread audio_thread;
  ThreadCreate(&audio_thread, AudioEntry, &audio_init_done);
  ThreadDetach(&audio_thread);
  NotifWaitAndDeinit(&audio_init_done);

  AudioQueueSound("Z:\\cdefault\\example\\data\\test_audio.ogg", true);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);
  Stopwatch input_stopwatch;
  StopwatchInit(&input_stopwatch);

  Snake snake;
  MEMORY_ZERO_STRUCT(&snake);
  SnakeSegment* node = &segments_memory[next_segment_index++];
  node->x = NUM_TILES / 2;
  node->y = NUM_TILES / 2;
  DLL_PUSH_BACK(snake.head, snake.tail, node, prev, next);
  snake.dir = V2_Y_POS;
  PickAppleLocation(&snake);

  while (!WindowShouldClose()) {
    if (WindowIsKeyJustPressed(Key_W)) {
      snake.dir = V2_Y_POS;
    } else if (WindowIsKeyJustPressed(Key_S)) {
      snake.dir = V2_Y_NEG;
    }
    if (WindowIsKeyJustPressed(Key_A)) {
      snake.dir = V2_X_NEG;
    } else if (WindowIsKeyJustPressed(Key_D)) {
      snake.dir = V2_X_POS;
    }
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIG_INT received, closing.");
      exit(0);
    }

    if (StopwatchReadSeconds(&input_stopwatch) >= 0.3f) {
      StopwatchReset(&input_stopwatch);
      if (!MoveSnake(&snake)) {
        LOG_INFO("Game over!");
        exit(0);
      }
    }

    for (S32 i = 0; i < NUM_TILES; i++) {
      for (S32 j = 0; j < NUM_TILES; j++) {
        DrawTile(i, j, GRID_COLOR);
      }
    }
    for (SnakeSegment* node = snake.tail; node != NULL; node = node->next) {
      DrawTile(node->x, node->y, SNAKE_COLOR);
    }
    DrawTile(apple_x, apple_y, APPLE_COLOR);

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);
    // LOG_INFO("%.05f", dt_s);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
