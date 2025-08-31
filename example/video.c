#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_VIDEO_IMPLEMENTATION
#include "../cdefault_video.h"

int main(void) {
  VideoCreateWindow(NULL, "title");
  while (true) {}
  return 0;
}
