#define CDEFAULT_STRING_IMPL

#include "../cdefault_assert.h"
#include "../cdefault_log.h"
#include "../cdefault_string.h"

int main() {
  String str = StringCreate(NULL, "hello");
  StringConcat(NULL, str, C_SV(" test"));
  printf("%s\n", str.buf);

  return 0;
}
