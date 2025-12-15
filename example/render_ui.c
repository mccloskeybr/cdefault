#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080
#define FONT_SIZE     22.0f

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
  Font font;
  String8 font_file_data;
  Image font_atlas_image;
  DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("c:/windows/fonts/consola.ttf"), &font_file_data.str, &font_file_data.size));
  DEBUG_ASSERT(FontInit(&font, font_file_data.str, font_file_data.size));
  DEBUG_ASSERT(FontAtlasBakeSdf(font_arena, &font, &font_atlas, &font_atlas_image, 100.0f, FONT_SIZE, 4.0f, FontCharSetLatin()));
  RendererRegisterImage(&font_handle, &font_atlas_image);

  B32 test_b32 = false;
  F32 test_f32 = 0.5f;
  S32 test_s32 = 5;
  F32 plot_values[50];

  V2 mouse_pos;
  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIG_INT received, closing.");
      exit(0);
    }

    for (U32 i = 0; i < STATIC_ARRAY_SIZE(plot_values); i++) {
      plot_values[i] = F32Sin(TimeSecondsSinceStart() + i);
    }

    WindowGetMousePositionV(&mouse_pos);
    UiPointerStateUpdate(mouse_pos, WindowIsMouseButtonPressed(MouseButton_Left), WindowIsMouseButtonPressed(MouseButton_Right));
    UiBegin(dt_s);
      if (UiWindowFloatingBegin(UIID(), Str8Lit("window 1"), V2Assign(50, 750), V2Assign(0, 0)).open) {
        UiPanelVerticalBegin(UIID(), V2Assign(10, 10), 10);

          UiPanelHorizontalBegin(UIID(), V2_ZEROES, 0);
            UiText(UIID(), Str8Lit("button:"), V2_ZEROES);
            UiGrow(UIID());
            UiButton(UIID(), Str8Lit("button"), V2Assign(200, 50));
            if (UiButtonToggle(UIID(), Str8Lit("toggle button"), &test_b32, V2Assign(200, 50)).hovered) {
              V2 hover_pos;
              V2 offset = V2Assign(5, -5);
              V2AddV2(&hover_pos, &mouse_pos, &offset);
              UiPopupBegin(UIID(), hover_pos, V2Assign(0, 0));
                UiText(UIID(), Str8Lit("popup!"), V2_ZEROES);
              UiPopupEnd();
            }
          UiPanelEnd();

          UiPanelHorizontalBegin(UIID(), V2_ZEROES, 0);
            UiText(UIID(), Str8Lit("combo box:"), V2_ZEROES);
            UiGrow(UIID());
            String8 combo_options[] = {
              Str8Lit("option 1"),
              Str8Lit("super long option 2"),
              Str8Lit("option 3"),
            };
            UiComboBox(UIID(), combo_options, STATIC_ARRAY_SIZE(combo_options), V2_ZEROES);
          UiPanelEnd();

          UiPanelHorizontalBegin(UIID(), V2_ZEROES, 0);
            UiText(UIID(), Str8Lit("radio buttons:"), V2_ZEROES);
            UiGrow(UIID());
            String8 radio_options[] = {
              Str8Lit("radio 1"),
              Str8Lit("radio 2"),
              Str8Lit("radio 3"),
            };
            UiButtonRadio(UIID(), radio_options, STATIC_ARRAY_SIZE(radio_options), V2Assign(25, 25), V2Assign(0, 0));
          UiPanelEnd();

          UiPanelHorizontalBegin(UIID(), V2_ZEROES, 0);
            UiText(UIID(), Str8Lit("sliders:"), V2_ZEROES);
            UiGrow(UIID());
            UiPanelVerticalBegin(UIID(), V2_ZEROES, 0);
              UiSliderF32(UIID(), &test_f32, 0, 1, 0.01f, V2_ZEROES);
              UiSliderS32(UIID(), &test_s32, -10, 10, 0.5f, V2_ZEROES);
            UiPanelEnd();
          UiPanelEnd();

          UiPanelHorizontalBegin(UIID(), V2_ZEROES, 0);
            UiText(UIID(), Str8Lit("plot:"), V2_ZEROES);
            UiGrow(UIID());
            UiPlotBar(UIID(), plot_values, STATIC_ARRAY_SIZE(plot_values), V2Assign(500, 100));
          UiPanelEnd();

        UiPanelEnd();
      }
      UiWindowEnd();
    UiDrawCommand* ui_draw_commands = UiEnd();
    for (UiDrawCommand* cmd = ui_draw_commands; cmd != NULL; cmd = cmd->next) {
      switch (cmd->type) {
        case UiDrawCommand_Line: { DrawLineV(cmd->line.start, cmd->line.end, 1, cmd->rect.color);                                         } break;
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
