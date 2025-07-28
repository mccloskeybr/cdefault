#ifndef CDEFAULT_H_
#define CDEFAULT_H_

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

///////////////////////////////////////////////////////////////////////////////
// NOTE: Types
///////////////////////////////////////////////////////////////////////////////

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;
#define true 1
#define false 0

///////////////////////////////////////////////////////////////////////////////
// NOTE: Gen purpose macros
///////////////////////////////////////////////////////////////////////////////

#define FILENAME (strrchr(__FILE__, '/')  ? strrchr(__FILE__, '/')  + 1 : \
                 (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : \
                 __FILE__))

#define STATIC_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define BIT(idx) (1 << (idx))
#define EXTRACT_BIT(word, idx) (((word) >> (idx)) & 1)
#define EXTRACT_U8(word, pos) (((word) >> ((pos) * 8)) & 0xff)
#define EXTRACT_U16(word, pos) (((word) >> ((pos) * 16)) & 0xffff)
#define EXTRACT_U32(word, pos) (((word) >> ((pos) * 32)) & 0xffffffff)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define CLAMP(a, x, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

#define KB(n)       (((U64)(n)) << 10)
#define MB(n)       (((U64)(n)) << 20)
#define GB(n)       (((U64)(n)) << 30)
#define TB(n)       (((U64)(n)) << 40)
#define THOUSAND(n) ((n)*1000)
#define MILLION(n)  ((n)*1000000)
#define BILLION(n)  ((n)*1000000000)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

///////////////////////////////////////////////////////////////////////////////
// NOTE: Assertions
///////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#  define ASSERT(exp) assert(exp)
#else
#  define ASSERT(exp) (exp)
#endif
#if defined(__GNUC__) || defined(__clang__)
#  define TRAP() __debugbreak()
#elif defined(__MSC_VER)
#  define TRAP() __builtin_trap()
#else
#  error Unknown trap intrinsic for this compiler.
#endif
#define STATIC_ASSERT(exp, msg) static_assert((exp), msg)
#define UNIMPLEMENTED() ASSERT(false)
#define UNREACHABLE() ASSERT(false)
#define TODO() ASSERT(false)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Branch prediction
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUC__) || defined(__clang__)
#  define BRANCH_EXPECT(expr, val) __builtin_expect((expr), (val))
#else
#  define BRANCH_EXPECT(expr) (expr)
#endif
#define LIKELY(expr) BRANCH_EXPECT(expr, true)
#define UNLIKELY(expr) BRANCH_EXPECT(expr, false)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Alignment and offsets
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) || defined(__clang__)
#  define ALIGN_OF(T) __alignof(T)
#elif defined(__GNUC__)
#  define ALIGN_OF(T) __alignof__(T)
#else
#  error ALIGN_OF not defined for this compiler.
#endif
#define ALIGN_POW_2(x, b) (((x) + (b) - 1) & (~((b) - 1)))

#define MEMBER(T, m)                    (((T*) 0)->m)
#define OFFSET_OF(T, m)                 (size_t) (&MEMBER(T,m))
#define MEMBER_FROM_OFFSET(T, ptr, off) (T)((((U8*) ptr) + (off)))
#define CAST_FROM_MEMBER(T, m, ptr)     (T*)(((U8*) ptr) - OFFSET_OF(T, m))

///////////////////////////////////////////////////////////////////////////////
// NOTE: Memory
///////////////////////////////////////////////////////////////////////////////

#define MEMORY_COPY(dst, src, size) memmove((dst), (src), (size))
#define MEMORY_SET(dst, byte, size) memset((dst), (byte), (size))
#define MEMORY_COMPARE(a, b, size)  memcmp((a), (b), (size))

#define MEMORY_COPY_STRUCT(d, s)       MEMORY_COPY((d), (s), sizeof(*(d)))
#define MEMORY_COPY_STATIC_ARRAY(d, s) MEMORY_COPY((d), (s), sizeof(d))

#define MEMORY_ZERO(dest, size)     MEMORY_SET(dst, 0, size)
#define MEMORY_ZERO_STRUCT(s)       MEMORY_ZERO((s), sizeof(*(s)))
#define MEMORY_ZERO_STATIC_ARRAY(a) MEMORY_ZERO((a), sizeof(a))

#define IS_MEMORY_EQUAL(a, b, size)       (MEMORY_COMPARE((a), (b), (size)) == 0)
#define IS_MEMORY_EQUAL_STRUCT(a, b)      IS_MEMORY_EQUAL((a), (b), sizeof(*(a)))
#define IS_MEMORY_EQUAL_STATIC_ARRAY(a,b) IS_MEMORY_EQUAL((a), (b), sizeof(a))

///////////////////////////////////////////////////////////////////////////////
// NOTE: List macros
///////////////////////////////////////////////////////////////////////////////

// Doubly-linked list
// E.g.
// struct MyNodeManager { DLL_CONTROLLER(MyNode); }
// struct MyNode { DLL_NODE(MyNode); S32 data; }
//
// MyNodeManager manager = {0};
// MyNodeManager node = {0};
// DLL_PUSH_FRONT(&manager, &node);
// DLL_POP_FRONT(&manager);

#define DLL_CONTROLLER(Node) \
  struct {                   \
    struct Node* head;       \
    struct Node* tail;       \
  } dll_cont;
#define DLL_NODE(Node) \
  struct {             \
    struct Node* prev; \
    struct Node* next; \
  } dll_node;

#define DLL_INSERT(list, prev_node, node)                 \
  do {                                                    \
    if ((list)->dll_cont.head == NULL) {                  \
      (list)->dll_cont.head = (node);                     \
      (list)->dll_cont.tail = (node);                     \
      (node)->dll_node.next = NULL;                       \
      (node)->dll_node.prev = NULL;                       \
    }                                                     \
    else if ((prev_node) == NULL) {                       \
      (node)->dll_node.next = (list)->dll_cont.head;      \
      (list)->dll_cont.head->dll_node.prev = (node);      \
      (list)->dll_cont.head = (node);                     \
      (node)->dll_node.prev = NULL;                       \
    }                                                     \
    else if ((prev_node) == (list)->dll_cont.tail) {      \
      (list)->dll_cont.tail->dll_node.next = (node);      \
      (node)->dll_node.prev = (list)->dll_cont.tail;      \
      (list)->dll_cont.tail = (node);                     \
      (node)->dll_node.next = NULL;                       \
    }                                                     \
    else {                                                \
      (prev_node)->dll_node.next->dll_node.prev = (node); \
      (node)->dll_node.next = (prev_node)->dll_node.next; \
      (prev_node)->dll_node.next = (node);                \
      (node)->dll_node.prev = (prev_node);                \
    }                                                     \
  } while (0)
#define DLL_REMOVE(list, node)                                      \
  do {                                                              \
    if ((node) == (list)->dll_cont.head) {                          \
      (list)->dll_cont.head = (list)->dll_cont.head->dll_node.next; \
    }                                                               \
    if ((node) == (list)->dll_cont.tail) {                          \
      (list)->dll_cont.tail = (list)->dll_cont.tail->dll_node.prev; \
    }                                                               \
    if ((node)->dll_node.prev != NULL) {                            \
      (node)->dll_node.prev->dll_node.next = (node)->dll_node.next; \
    }                                                               \
    if ((node)->dll_node.next != NULL) {                            \
      (node)->dll_node.next->dll_node.prev = (node)->dll_node.prev; \
    }                                                               \
  } while (0)
#define DLL_PUSH_FRONT(list, node) DLL_INSERT(list, (typeof(node)) NULL, node)
#define DLL_PUSH_BACK(list, node)  DLL_INSERT(list, DLL_BACK(list), node)
#define DLL_POP_FRONT(list)                                           \
  do {                                                                \
    typeof((list)->dll_cont.head) _cdef_node = (list)->dll_cont.head; \
    DLL_REMOVE(list, _cdef_node);                                     \
  } while(0)
#define DLL_POP_BACK(list)                                            \
  do {                                                                \
    typeof((list)->dll_cont.tail) _cdef_node = (list)->dll_cont.tail; \
    DLL_REMOVE(list, _cdef_node);                                     \
  } while(0)
#define DLL_FRONT(list) (list)->dll_cont.head
#define DLL_BACK(list) (list)->dll_cont.tail
#define DLL_NEXT(node) (node)->dll_node.next
#define DLL_PREV(node) (node)->dll_node.prev
#define DLL_FOR_EACH(T, node, list) \
  for (T (node) = DLL_FRONT(list); (node) != NULL; (node) = DLL_NEXT(node))

///////////////////////////////////////////////////////////////////////////////
// NOTE: Log
///////////////////////////////////////////////////////////////////////////////

// NOTE: If not initialized, will print to stderr.
void LogInit(FILE* fd);
void Log(char* level, char* filename, U32 loc, char* fmt, ...);
#define LOG_INFO(fmt, ...) Log("INFO",  FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Log("WARN",  FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Log("ERROR", FILENAME, __LINE__, fmt, ##__VA_ARGS__)

#ifndef NDEBUG
#  define LOG_DEBUG(fmt, ...) Log("DEBUG", FILENAME, fmt, __VA_ARGS__)
#else
#  define LOG_DEBUG(fmt, ...)
#endif

///////////////////////////////////////////////////////////////////////////////
// NOTE: Test
///////////////////////////////////////////////////////////////////////////////

// NOTE: Test functions should return a B32, true on success.
// NOTE: Wrap test functions in TEST() from main, expectations in TEST_EXPECT().

#define TEST(func) \
  if (func) { LOG_INFO ("\t[" ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET "]: "#func); } \
  else      { LOG_ERROR("\t[" ANSI_COLOR_RED   "FAILURE" ANSI_COLOR_RESET "]: "#func); }
#define TEST_EXPECT(expr) \
  if (!(expr)) { LOG_ERROR("\t" ANSI_COLOR_RED "Expectation failed: " ANSI_COLOR_RESET #expr); return false; }

///////////////////////////////////////////////////////////////////////////////
// NOTE: Arena
///////////////////////////////////////////////////////////////////////////////

typedef struct Arena Arena;
struct Arena {
  U64 capacity;
  U64 pos;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
  Arena* arena;
  U64 pos;
};

Arena* ArenaCreate(U64 capacity);
void ArenaRelease(Arena* arena);
void* ArenaPush(Arena* arena, U64 size, U64 align);
void ArenaPop(Arena* arena, U64 size);
void ArenaClear(Arena* arena);

ArenaTemp ArenaTempBegin(Arena* arena);
void ArenaTempEnd(ArenaTemp* temp);

#define ARENA_PUSH_ARRAY(arena, type, count) (type*) ArenaPush(arena, sizeof(type) * count, MAX(8, ALIGN_OF(type)))
#define ARENA_PUSH_STRUCT(arena, type) ARENA_PUSH_ARRAY(arena, type, 1)

///////////////////////////////////////////////////////////////////////////////
// NOTE: String
///////////////////////////////////////////////////////////////////////////////

typedef struct String8 String8;
struct String8 {
  U8* str;
  U64 size;
};

typedef struct String8Node String8Node;
struct String8Node {
  DLL_NODE(String8Node);
  String8 string;
};

typedef struct String8List String8List;
struct String8List {
  DLL_CONTROLLER(String8Node);
};

B32 CharIsWhitespace(U8 c);
B32 CharIsLower(U8 c);
B32 CharIsUpper(U8 c);
B32 CharIsAlpha(U8 c);
B32 CharIsDigit(U8 c, U32 base);
U8 CharToLower(U8 c);
U8 CharToUpper(U8 c);

String8 String8Create(U8* str, U64 size);
String8 String8CreateCString(U8* c_str);
String8 String8CreateRange(U8* str, U8* one_past_last);
#define String8CreateStatic(s) String8Create(s, sizeof(s) - 1)
String8 String8Copy(Arena* arena, String8* string);

String8 String8Substring(String8* s, U64 start, U64 end);
String8 String8Trim(String8* s);

B32 String8StartsWith(String8* a, String8* b); // NOTE: True iff a starts with b.
B32 String8EndsWith(String8* a, String8* b); // NOTE: True iff a ens with b.
B32 String8Equals(String8* a, String8* b);
S64 String8Find(String8* string, U64 start_pos, String8* needle); // NOTE: Returns -1 on failure.
S64 String8FindReverse(String8* string, U64 reverse_start_pos, String8* needle); // NOTE: Returns -1 on failure.

String8 String8ToUpper(Arena* arena, String8* s);
String8 String8ToLower(Arena* arena, String8* s);
String8 String8Concat(Arena* arena, String8* a, String8* b);
String8 String8FormatV(Arena* arena, U8* fmt, va_list args);
String8 String8Format(Arena* arena, U8* fmt, ...);

void String8ListPrepend(String8List* list, String8Node* node);
void String8ListAppend(String8List* list, String8Node* node);
String8 String8ListJoin(Arena* arena, String8List* list);
String8List String8Split(Arena* arena, String8* string, U8 c);

#endif // CDEFAULT_H_

///////////////////////////////////////////////////////////////////////////////
// NOTE: Implementation
///////////////////////////////////////////////////////////////////////////////

#ifdef CDEFAULT_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Log implementation
///////////////////////////////////////////////////////////////////////////////

typedef struct LogConfig LogConfig;
struct LogConfig {
  FILE* fd;
  mtx_t mtx;
  B8 is_initialized;
};
static LogConfig g_cdefault_log_config = {0};

void LogInit(FILE* fd) {
  g_cdefault_log_config = (LogConfig) {0};
  g_cdefault_log_config.fd = fd;
  mtx_init(&g_cdefault_log_config.mtx, mtx_plain);
  g_cdefault_log_config.is_initialized = true;
}

static void LogV(char* level, char* filename, U32 loc, char* fmt, va_list args) {
  if (UNLIKELY(!g_cdefault_log_config.is_initialized)) { LogInit(stderr); }
  mtx_lock(&g_cdefault_log_config.mtx);
  fprintf(g_cdefault_log_config.fd, "[%s | %s:%d]: ", level, filename, loc);
  vfprintf(g_cdefault_log_config.fd, fmt, args);
  fprintf(g_cdefault_log_config.fd, "\n");
  mtx_unlock(&g_cdefault_log_config.mtx);
}

void Log(char* level, char* filename, U32 loc, char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  LogV(level, filename, loc, fmt, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Arena implementation
///////////////////////////////////////////////////////////////////////////////

Arena* ArenaCreate(U64 capacity) {
  Arena* arena = (Arena*) malloc(capacity + sizeof(Arena));
  ASSERT(arena != NULL);
  arena->capacity = capacity;
  arena->pos = 0;
}

void ArenaRelease(Arena* arena) {
  free(arena);
}

void* ArenaPush(Arena* arena, U64 size, U64 align) {
  arena->pos = ALIGN_POW_2(arena->pos, align);
  ASSERT(arena->pos + size < arena->capacity);
  void* chunk = ((U8*) arena) + sizeof(Arena) + arena->pos;
  arena->pos += size;
  return chunk;
}

void ArenaPop(Arena* arena, U64 size) {
  ASSERT(arena->pos > size);
  arena->pos -= size;
}

void ArenaClear(Arena* arena) {
  arena->pos = 0;
}

ArenaTemp ArenaTempBegin(Arena* arena) {
  ArenaTemp temp = {0};
  temp.arena = arena;
  temp.pos = arena->pos;
  return temp;
}

void ArenaTempEnd(ArenaTemp* temp) {
  temp->arena->pos = temp->pos;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: String Implementation
///////////////////////////////////////////////////////////////////////////////

B32 CharIsWhitespace(U8 c) {
  return (c == ' '  || c == '\n' ||
          c == '\t' || c == '\r' ||
          c == '\f' || c == '\v');
}

B32 CharIsLower(U8 c) {
  return ('a' <= c && c <= 'z');
}

B32 CharIsUpper(U8 c) {
  return ('A' <= c && c <= 'Z');
}

B32 CharIsAlpha(U8 c) {
  return CharIsLower(c) || CharIsUpper(c);
}

B32 CharIsDigit(U8 c, U32 base) {
  if (UNLIKELY(base == 0 || base > 16)) { return false; }
  if      ('0' <= c && c <= '9') { c -= '0'; }
  else if (CharIsLower(c))       { c -= 'a'; c += 10; }
  else if (CharIsUpper(c))       { c -= 'A'; c += 10; }
  return (c < base);
}

U8 CharToLower(U8 c) {
  if (CharIsUpper(c)) { c += 'a' - 'A'; }
  return c;
}

U8 CharToUpper(U8 c) {
  if (CharIsLower(c)) { c += 'A' - 'a'; }
  return c;
}

String8 String8Create(U8* str, U64 size) {
  String8 result = {0};
  result.str = str;
  result.size = size;
  return result;
}

String8 String8CreateCString(U8* c_str) {
  U8* c = c_str;
  while (*c != '\0') { c += 1; }
  return String8CreateRange(c_str, c);
}

String8 String8CreateRange(U8* str, U8* one_past_last) {
  String8 result = {0};
  result.str = str;
  result.size = (U64) (one_past_last - str);
  return result;
}

String8 String8Copy(Arena* arena, String8* string) {
  String8 result = {0};
  result.size = string->size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, string->size);
  MEMORY_COPY(result.str, string->str, string->size);
  return result;
}

String8 String8Substring(String8* s, U64 start, U64 end) {
  ASSERT(end > start);
  start = MIN(start, s->size - 1);
  end = MIN(end, s->size - 1);
  String8 result = {0};
  result.str = s->str + start;
  result.size = end - start + 1;
  return result;
}

String8 String8Trim(String8* s) {
  U64 start = 0;
  while (CharIsWhitespace(s->str[start])) { ++start; }
  U64 end = s->size - 1;
  while (CharIsWhitespace(s->str[end])) { --end; }
  return String8Substring(s, start, end);
}

B32 String8StartsWith(String8* a, String8* b) {
  if (a->size < b->size) { return false; }
  for (U64 i = 0; i < b->size; ++i) {
    if (a->str[i] != b->str[i]) { return false; }
  }
  return true;
}

B32 String8EndsWith(String8* a, String8* b) {
  if (a->size < b->size) { return false; }
  U64 a_offset = a->size - b->size;
  for (U64 i = 0; i < b->size; ++i) {
    if (a->str[a_offset + i] != b->str[i]) { return false; }
  }
  return true;
}

B32 String8Equals(String8* a, String8* b) {
  if (a->size != b->size) { return false; }
  return String8StartsWith(a, b);
}

S64 String8Find(String8* string, U64 start_pos, String8* needle) {
  if (string->size < start_pos + needle->size) { return -1; }
  for (U64 i = 0; i < string->size - start_pos; ++i) {
    U64 offset = start_pos + i;
    String8 substr = String8Create(string->str + offset, string->size - offset);
    if (String8StartsWith(&substr, needle)) { return offset; }
  }
  return -1;
}

S64 String8FindReverse(String8* string, U64 reverse_start_pos, String8* needle) {
  if (string->size < reverse_start_pos + needle->size) { return -1; }
  for (U64 i = string->size - reverse_start_pos - needle->size; i > 0; --i) {
    String8 substr = String8Create(string->str, i + needle->size);
    if (String8EndsWith(&substr, needle)) { return i; }
  }
  return -1;
}

String8 String8ToUpper(Arena* arena, String8* s) {
  String8 result = String8Copy(arena, s);
  for (U64 i = 0; i < result.size; ++i) {
    result.str[i] = CharToUpper(result.str[i]);
  }
  return result;
}

String8 String8ToLower(Arena* arena, String8* s) {
  String8 result = String8Copy(arena, s);
  for (U64 i = 0; i < result.size; ++i) {
    result.str[i] = CharToLower(result.str[i]);
  }
  return result;
}

String8 String8Concat(Arena* arena, String8* a, String8* b) {
  String8 result = {0};
  result.size = a->size + b->size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, a->size + b->size);
  MEMORY_COPY(result.str, a->str, a->size);
  MEMORY_COPY(result.str + a->size, b->str, b->size);
  return result;
}

String8 String8FormatV(Arena* arena, U8* fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  U32 size = vsnprintf(NULL, 0, fmt, args_copy) + 1;
  String8 result = {0};
  result.size = size - 1;
  result.str = ARENA_PUSH_ARRAY(arena, U8, size);
  vsnprintf(result.str, size, fmt, args_copy);
  va_end(args_copy);
  ArenaPop(arena, 1); // NOTE: null terminator.
  return result;
}

String8 String8Format(Arena* arena, U8* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  String8 result = String8FormatV(arena, fmt, args);
  va_end(args);
  return result;
}

void String8ListPrepend(String8List* list, String8Node* node) {
  DLL_PUSH_FRONT(list, node);
}

void String8ListAppend(String8List* list, String8Node* node) {
  DLL_PUSH_BACK(list, node);
}

String8 String8ListJoin(Arena* arena, String8List* list) {
  U64 size = 0;
  DLL_FOR_EACH(String8Node*, node, list) { size += node->string.size; }
  String8 result = {0};
  result.size = size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, size);
  size = 0;
  DLL_FOR_EACH(String8Node*, node, list) {
    MEMORY_COPY(result.str + size, node->string.str, node->string.size);
    size += node->string.size;
  }
  return result;
}

String8List String8Split(Arena* arena, String8* string, U8 c) {
  String8List list = {0};
  U8* substring_start = string->str;
  for (U64 i = 0; i < string->size; ++i) {
    if (string->str[i] == c) {
      String8Node* node = ARENA_PUSH_STRUCT(arena, String8Node);
      node->string = String8CreateRange(substring_start, &string->str[i]);
      String8ListAppend(&list, node);
      substring_start = &string->str[++i];
    }
  }
  if (substring_start < string->str + string->size) {
    String8Node* node = ARENA_PUSH_STRUCT(arena, String8Node);
    node->string = String8CreateRange(substring_start, string->str + string->size);
    String8ListAppend(&list, node);
  }
  return list;
}

#endif // CDEFAULT_IMPLEMENTATION
