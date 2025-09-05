#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_VIDEO_IMPLEMENTATION
#include "../cdefault_video.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"
#include "third_party/stb_vorbis.c"

#define TILE_SIZE 62
#define TILE_PAD 1
#define TILE_RADIUS 20 // NOTE: rounded corner radius
#define NUM_TILES 10 // NOTE: number of tiles per row or column.
#define WINDOW_WIDTH 1920
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
static Snake snake;

static RandomSeries r;
static S32 apple_x, apple_y;

static S32 AudioEntry(void* UNUSED(user_data)) {
  ASSERT(AudioInit());

  S32 stb_err = 0;
  stb_vorbis* bg_music = stb_vorbis_open_filename("Z:\\cdefault\\example\\data\\test_audio.ogg", &stb_err, NULL);
  ASSERT(stb_err == 0);
  stb_vorbis_info bg_music_info = stb_vorbis_get_info(bg_music);

  AudioStreamSpec spec;
  MEMORY_ZERO_STRUCT(&spec);
  spec.device_handle = AUDIO_DEFAULT_DEVICE;
  spec.channels = bg_music_info.channels;
  spec.frequency = bg_music_info.sample_rate;
  spec.format = AudioStreamFormat_F32;
  AudioStreamHandle stream;
  ASSERT(AudioStreamOpen(&stream, spec));

  U8* audio_buffer;
  U32 audio_buffer_size;

  while (true) {
    if (!AudioStreamAcquireBuffer(stream, &audio_buffer, &audio_buffer_size)) { continue; }
    uint32_t read = stb_vorbis_get_samples_float_interleaved(
        bg_music, bg_music_info.channels, (F32*) audio_buffer, audio_buffer_size / sizeof(F32));
    if (!AudioStreamReleaseBuffer(stream, audio_buffer, audio_buffer_size)) { continue; }
    if (read == 0) { stb_vorbis_seek_start(bg_music); }
    if (!AudioStreamWait(stream)) { continue; }
  }

  AudioStreamClose(stream);
  stb_vorbis_close(bg_music);
  AudioDeinit();
  return 0;
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

static void AddSnakeSegment(S32 x, S32 y) {
  SnakeSegment* node = &segments_memory[next_segment_index++];
  node->x = x;
  node->y = y;
  DLL_PUSH_BACK(snake.head, snake.tail, node, prev, next);
}

static void PickAppleLocation() {
  B32 intersects_snake;
  do {
    apple_x = RandS32(&r, 0, NUM_TILES);
    apple_y = RandS32(&r, 0, NUM_TILES);

    intersects_snake = false;
    for (SnakeSegment* node = snake.tail; node != NULL; node = node->next) {
      if (node->x == apple_x && node->y == apple_y) {
        intersects_snake = true;
        break;
      }
    }
  } while (intersects_snake);
}

static B32 MoveSnake() {
  SnakeSegment* head = snake.head;
  SnakeSegment* tail = snake.tail;
  DLL_POP_BACK(snake.head, snake.tail, prev, next);

  S32 orig_x = tail->x;
  S32 orig_y = tail->y;
  tail->x = head->x + snake.dir.x;
  tail->y = head->y + snake.dir.y;
  if      (tail->x < 0)          { tail->x = NUM_TILES - 1; }
  else if (tail->x >= NUM_TILES) { tail->x = 0;             }
  if      (tail->y < 0)          { tail->y = NUM_TILES - 1; }
  else if (tail->y >= NUM_TILES) { tail->y = 0;             }

  if (tail->x == apple_x && tail->y == apple_y) {
    SnakeSegment* node = &segments_memory[next_segment_index++];
    node->x = orig_x;
    node->y = orig_y;
    DLL_PUSH_BACK(snake.head, snake.tail, node, prev, next);
    PickAppleLocation();
  }

  for (SnakeSegment* node = snake.tail; node != NULL; node = node->next) {
    if (node->x == tail->x && node->y == tail->y) {
      return false;
    }
  }

  DLL_PUSH_FRONT(snake.head, snake.tail, tail, prev, next);
  return true;
}

int main(void) {
  RandInit(&r, 12345);
  ASSERT(WindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, "video example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  Thread audio_thread;
  ThreadCreate(&audio_thread, AudioEntry, NULL);
  ThreadDetach(&audio_thread);

  // TODO: create a std timer struct and a way to measure time.
  F32 dt_s = 0.016f;
  F32 input_timer = 0;
  F32 input_timer_max_s = 0.3f;

  SnakeSegment* node = &segments_memory[next_segment_index++];
  node->x = NUM_TILES / 2;
  node->y = NUM_TILES / 2;
  DLL_PUSH_BACK(snake.head, snake.tail, node, prev, next);
  snake.dir = V2_Y_POS;
  PickAppleLocation();

  while (!WindowShouldClose()) {
    for (S32 i = 0; i < NUM_TILES; i++) {
      for (S32 j = 0; j < NUM_TILES; j++) {
        DrawTile(i, j, GRID_COLOR);
      }
    }

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

    if (WindowIsKeyJustPressed(Key_Space)) {
      AddSnakeSegment(0, 0);
    }

    input_timer += dt_s;
    if (input_timer >= input_timer_max_s) {
      input_timer = 0;
      if (!MoveSnake()) { exit(1); }
    }

    for (SnakeSegment* node = snake.tail; node != NULL; node = node->next) {
      DrawTile(node->x, node->y, SNAKE_COLOR);
    }
    DrawTile(apple_x, apple_y, APPLE_COLOR);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
