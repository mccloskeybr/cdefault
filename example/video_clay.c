#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_VIDEO_IMPLEMENTATION
#include "../cdefault_video.h"
#define CLAY_IMPLEMENTATION
#include "third_party/clay.h"

static const Clay_Color COLOR_LIGHT = {224, 215, 210, 255};
static const Clay_Color COLOR_RED = {168, 66, 28, 255};
static const Clay_Color COLOR_ORANGE = {225, 138, 50, 255};

static void HandleClayErrors(Clay_ErrorData error) {
  LOG_ERROR("%s", error.errorText.chars);
}

static void SidebarItemComponent() {
  Clay_ElementDeclaration sidebarItemConfig = {
    .layout = {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
    },
    .backgroundColor = COLOR_ORANGE,
  };
  CLAY(sidebarItemConfig) {
      // children go here...
  }
}

int main(void) {
  WindowInitOpts opts;
  MEMORY_ZERO_STRUCT(&opts);
  opts.title = "video example";
  opts.clear_rgb = 0x6495ed;
  ASSERT(WindowInit(opts));

  S32 width, height;
  WindowGetDims(NULL, NULL, &width, &height);

  uint64_t clay_mem_size = Clay_MinMemorySize();
  Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(clay_mem_size, malloc(clay_mem_size));
  Clay_Initialize(arena, (Clay_Dimensions) { width, height }, (Clay_ErrorHandler) { HandleClayErrors });

  F32 x = 0;
  while (true) {
    Clay_BeginLayout();

    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 }, .backgroundColor = {250,250,255,255} }) {
      CLAY({
        .id = CLAY_ID("SideBar"),
        .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16 },
        .backgroundColor = COLOR_LIGHT
      }) {
        /*
        CLAY({ .id = CLAY_ID("ProfilePictureOuter"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = COLOR_RED }) {
          CLAY({ .id = CLAY_ID("ProfilePicture"), .layout = { .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) }}, .image = { .imageData = &profilePicture } }) {}
          CLAY_TEXT(CLAY_STRING("Clay - UI Library"), CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = {255, 255, 255, 255} }));
        }
        */

        for (int i = 0; i < 5; i++) {
            SidebarItemComponent();
        }

        CLAY({ .id = CLAY_ID("MainContent"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } }, .backgroundColor = COLOR_LIGHT }) {}
      }
    }

    Clay_RenderCommandArray render_commands = Clay_EndLayout();
    for (S32 i = 0; i < render_commands.length; i++) {
      Clay_RenderCommand* cmd = &render_commands.internalArray[i];
      switch (cmd->commandType) {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
          DrawRectangle(
            cmd->boundingBox.x + cmd->boundingBox.width / 2,
            height - cmd->boundingBox.y - cmd->boundingBox.height / 2,
            cmd->boundingBox.width,
            cmd->boundingBox.height,
            0,
            cmd->renderData.rectangle.backgroundColor.r / 255.0f,
            cmd->renderData.rectangle.backgroundColor.g / 255.0f,
            cmd->renderData.rectangle.backgroundColor.b / 255.0f);
        } break;
        default: UNIMPLEMENTED();
      }
    }

    WindowSwapBuffers();
  }

  WindowDeinit();
  return 0;
}
