#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080
#define FONT_SIZE     24.0f

static FontAtlas font_atlas;
static U32       font_handle;

void UiTextMeasure(String8 text, V2* size) {
  DEBUG_ASSERT(FontAtlasMeasureString(&font_atlas, FONT_SIZE, text, size));
}

void UiFontGetAttributes(F32* descent) {
  FontAtlasGetAttributes(&font_atlas, FONT_SIZE, NULL, descent);
}

int main(void) {
  TimeInit();
  DEBUG_ASSERT(WindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, "ui example"));
  RendererDisableDepthTest();
  UiInit(UiTextMeasure, UiFontGetAttributes);

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

  U32 whatever = 0;

  B32 test_bool = false;
  F32 test_float = 0.5f;
  V2 mouse_pos;
  while (!WindowShouldClose()) {
    whatever++;
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIG_INT received, closing.");
      exit(0);
    }

    WindowGetMousePositionV(&mouse_pos);
    UiPointerStateUpdate(mouse_pos, WindowIsMouseButtonPressed(MouseButton_Left), WindowIsMouseButtonPressed(MouseButton_Right));
    UiBegin(dt_s);
      if (UiWindowFloatingBegin(UIID(), Str8Lit("window 1"), V2Assign(50, 750), V2Assign(0, 0)).open) {
        UiPanelVerticalBegin(UIID(), V2Assign(2, 2), 2);
          UiInteraction toggle = UiButtonToggle(UIID(), Str8Lit("hello"), V2Assign(500, 50), &test_bool);
          if (toggle.hovered) {
            V2 hover_pos;
            V2 offset = V2Assign(5, -5);
            V2AddV2(&hover_pos, &mouse_pos, &offset);
            UiPopupBegin(UIID(), hover_pos, V2Assign(0, 0));
              UiText(UIID(), Str8Lit("popup!"));
            UiPopupEnd();
          }
          UiPanelHorizontalBegin(UIID(), V2_ZEROES, 0);
            String8 options[] = {
              Str8Lit("option 1"),
              Str8Lit("super long option 2"),
              Str8Lit("option 3"),
            };
            // UiButtonRadio(UIID(), options, 3, V2Assign(25, 25), V2Assign(0, 0));
            UiComboBox(UIID(), options, 3);
            UiSliderF32(UIID(), &test_float, 0, 1);
            UiGrow(UIID());
          UiPanelEnd();
        UiPanelEnd();
      }
      UiWindowEnd();
    UiDrawCommand* ui_draw_commands = UiEnd();
    for (UiDrawCommand* cmd = ui_draw_commands; cmd != NULL; cmd = cmd->next) {
      switch (cmd->type) {
        case UiDrawCommand_Rect: { DrawRectangleV(cmd->rect.center, cmd->rect.size, cmd->rect.color);                                     } break;
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
