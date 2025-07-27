#ifndef CDEFAULT_H_
#define CDEFAULT_H_

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
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

#define STATIC_ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#define BIT(i) 1 << i

#define KB(n)       (((U64)(n)) << 10)
#define MB(n)       (((U64)(n)) << 20)
#define GB(n)       (((U64)(n)) << 30)
#define TB(n)       (((U64)(n)) << 40)
#define THOUSAND(n) ((n)*1000)
#define MILLION(n)  ((n)*1000000)
#define BILLION(n)  ((n)*1000000000)

#ifndef NDEBUG
#  define ASSERT(exp) assert(exp)
#  define STATIC_ASSERT(exp, msg) static_assert((exp), msg)
#else
#  define ASSERT(exp) (exp)
#  define STATIC_ASSERT(exp)
#endif

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
#define DLL_POP_FRONT(list)                                  \
  do {                                                       \
    typeof((list)->dll_cont.head) n = (list)->dll_cont.head; \
    DLL_REMOVE(list, n);                                     \
  } while(0)
#define DLL_POP_BACK(list)                                   \
  do {                                                       \
    typeof((list)->dll_cont.tail) n = (list)->dll_cont.tail; \
    DLL_REMOVE(list, n);                                     \
  } while(0)
#define DLL_FRONT(list) (list)->dll_cont.head
#define DLL_BACK(list) (list)->dll_cont.tail
#define DLL_NEXT(node) (node)->dll_node.next
#define DLL_PREV(node) (node)->dll_node.prev

#endif // CDEFAULT_H_

#ifdef CDEFAULT_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Log
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

#endif // CDEFAULT_IMPLEMENTATION
