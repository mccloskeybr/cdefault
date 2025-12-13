#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080
#define FONT_SIZE     24.0f

static FontAtlas font_atlas;
static U32       font_handle;

V2 MeasureText(String8 text) {
  V2 result;
  DEBUG_ASSERT(FontAtlasMeasureString(&font_atlas, FONT_SIZE, text, &result));
  return result;
}

int main(void) {
  TimeInit();
  DEBUG_ASSERT(WindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, "ui example"));
  RendererDisableDepthTest();
  UiInit(MeasureText);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  Arena* font_arena = ArenaAllocate();
  Font  font;
  String8 font_file_data;
  Image font_atlas_image;
  DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("c:/windows/fonts/consola.ttf"), &font_file_data.str, &font_file_data.size));
  DEBUG_ASSERT(FontInit(&font, font_file_data.str, font_file_data.size));
  DEBUG_ASSERT(FontAtlasBakeSdf(font_arena, &font, &font_atlas, &font_atlas_image, 100.0f, FONT_SIZE, 4.0f, FontCharSetLatin()));
  RendererRegisterImage(&font_handle, &font_atlas_image);

  V2 mouse_pos;
  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIG_INT received, closing.");
      exit(0);
    }

    WindowGetMousePositionV(&mouse_pos);
    UiPointerStateUpdate(mouse_pos, WindowIsMouseButtonPressed(MouseButton_Left), WindowIsMouseButtonPressed(MouseButton_Right));
    UiBegin(dt_s);
      UiWindowFitBegin(UIID(), V2Assign(50, 750));
        UiPanelVerticalBegin(UIID(), V2Assign(20, 20), 20);
          UiButtonLabel(UIID(), Str8Lit("hello"), V2Assign(100, FONT_SIZE));
          UiButton(UIID(), V2Assign(200, 50));
          if (UiButton(UIID(), V2Assign(100, 50)).clicked) {
            LOG_INFO("clicked");
          }

          UiPanelHorizontalBegin(UIID(), V2Assign(0, 0), 5);
            UiButton(UIID(), V2Assign(50, 100));
            UiButton(UIID(), V2Assign(50, 50));
          UiPanelEnd();

          UiButton(UIID(), V2Assign(100, 50));
          UiButton(UIID(), V2Assign(100, 50));
        UiPanelEnd();
      UiWindowEnd();
    UiDrawCommand* ui_draw_commands = UiEnd();
    for (UiDrawCommand* cmd = ui_draw_commands; cmd != NULL; cmd = cmd->next) {
      switch (cmd->type) {
        case UiDrawCommand_Rect: { DrawRoundedRectangleV(cmd->rect.center, cmd->rect.size, 5, cmd->rect.color);                           } break;
        case UiDrawCommand_Text: { DrawStringSdfV(cmd->text.string, &font_atlas, font_handle, FONT_SIZE, cmd->text.pos, cmd->text.color); } break;
        default: UNIMPLEMENTED();
      }
    }

    dt_s = StopwatchReadSeconds(&frame_stopwatch);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  return 0;
}
