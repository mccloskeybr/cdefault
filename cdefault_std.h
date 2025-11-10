#ifndef CDEFAULT_STD_H_
#define CDEFAULT_STD_H_

// TODO: wide strings (String16)
// TODO: str8 base64 conversion funcs, useful for encoding bin blobs in json
// TODO: assert --> message box w/ error?

#if defined(_WIN32)
#  define OS_WINDOWS 1
#elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
#else
#  error Unknown / unsupported operating system.
#endif

#if defined(__GNUC__) || defined(__GNUG__)
#  define COMPILER_GCC 1
#elif defined(__clang__)
#  define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#  define COMPILER_MSVC 1
#else
#  error Unknown / unsupported compiler.
#endif

#if defined(OS_WINDOWS)

#include <windows.h>

#elif defined(OS_LINUX)

#include <sys/mman.h>
#include <threads.h>
#include <time.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#elif defined(OS_MAC)

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/mman.h>
#include <threads.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#endif

#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

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
#define true     1
#define false    0

#define U8_MIN  0u
#define U8_MAX  255u
#define U16_MIN 0u
#define U16_MAX 65535u
#define U32_MIN 0u
#define U32_MAX 4294967295u
#define U64_MIN 0u
#define U64_MAX 18446744073709551615u
#define S8_MIN  -128
#define S8_MAX  127
#define S16_MIN -32768
#define S16_MAX 32767
#define S32_MIN -2147483648
#define S32_MAX 2147483647
#define S64_MIN -9223372036854775808
#define S64_MAX 9223372036854775807
#define F32_MIN -3.402823466e+38f
#define F32_MAX 3.402823466e+38f
#define F32_NAN (((F32)0.0f)/((F32)0.0f));
#define F32_MIN_POSITIVE 1.175494351e-38f
#define F32_POS_INFINITY (((F32)+1.0f)/((F32)0.0f));
#define F32_NEG_INFINITY (((F32)-1.0f)/((F32)0.0f));
#define F64_MIN -1.7976931348623157e+308f
#define F64_MAX 1.7976931348623157e+308f
#define F64_NAN (((F64)0.0f)/((F64)0.0f));
#define F64_MIN_POSITIVE 2.2250738585072014e-308f
#define F64_POS_INFINITY (((F64)+1.0f)/((F64)0.0f));
#define F64_NEG_INFINITY (((F64)-1.0f)/((F64)0.0f));

#define F32_PI    3.14159265358979323846264338327950288f
#define F32_TAU   6.28318530717958647692528676655900576f
#define F32_E     2.71828182845904523536028747135266249f
#define F32_SQRT2 1.41421356237309504880168872420969808f
#define F64_PI    F32_PI
#define F64_TAU   F32_TAU
#define F64_E     F32_E
#define F64_SQRT2 F32_SQRT2

///////////////////////////////////////////////////////////////////////////////
// NOTE: Gen purpose macros
///////////////////////////////////////////////////////////////////////////////

#if defined(COMPILER_MSVC)
#  define UNUSED(x) __pragma(warning(suppress:4100)) x
#elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#  define UNUSED(x) x __attribute__((unused))
#else
#  define UNUSED(x) x
#endif

#define GLUE(a, b) a ## b
#define FILENAME (strrchr(__FILE__, '/')  ? strrchr(__FILE__, '/')  + 1 : \
                 (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : \
                 __FILE__))

#define STATIC_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MIN3(a, b, c)   (MIN(MIN((a), (b)), (c)))
#define MAX(a, b)       (((a) < (b)) ? (b) : (a))
#define MAX3(a, b, c)   (MAX(MAX((a), (b)), (c)))
#define CLAMP(a, x, b)  (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define CLAMP_TOP(a, x) MIN((a), (x))
#define CLAMP_BOT(x, b) MAX((x), (b))
#define SIGN(x)         (((x) > 0) ? 1 : (((x) < 0) ? -1 : 0))
#define SWAP(t, a, b)   do { t _s=(a); (a)=(b); (b)=_s; } while(0)

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
// NOTE: Bit manipulation
///////////////////////////////////////////////////////////////////////////////

#define BIT(idx) (1 << (idx))
#define EXTRACT_BIT(word, idx) (((word) >> (idx)) & 1)
#define EXTRACT_U8(word, pos)  (((word) >> ((pos) * 8)) & 0xff)
#define EXTRACT_U16(word, pos) (((word) >> ((pos) * 16)) & 0xffff)
#define EXTRACT_U32(word, pos) (((word) >> ((pos) * 32)) & 0xffffffff)

S32 U32LsbPos(U32 x);    // E.g. 0110 -> 1. NOTE: Returns -1 on 0.
S32 U32MsbPos(U32 x);    // E.g. 0110 -> 2. NOTE: Returns -1 on 0.
U32 U32CountBits(U32 x); // E.g. 1111 -> 4.

///////////////////////////////////////////////////////////////////////////////
// NOTE: Branch prediction
///////////////////////////////////////////////////////////////////////////////

#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#  define BRANCH_EXPECT(expr, val) __builtin_expect((expr), (val))
#else
#  define BRANCH_EXPECT(expr, val) (expr)
#endif
#define LIKELY(expr) BRANCH_EXPECT(expr, true)
#define UNLIKELY(expr) BRANCH_EXPECT(expr, false)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Assertions
///////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#  define DEBUG_ASSERT(exp) assert(LIKELY(exp))
#  define ASSERT(exp) DEBUG_ASSERT(exp)
#else
#  define DEBUG_ASSERT(exp) (exp)
#  define ASSERT(exp) if (LIKELY(!(exp))) { *(int*)0 = 0; }
#endif
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#  define TRAP() __debugbreak()
#elif defined(COMPILER_MSVC)
#  define TRAP() __builtin_trap()
#else
#  error Unknown trap intrinsic for this compiler.
#endif
#define STATIC_ASSERT(exp, msg) static_assert((exp), msg)
#define UNIMPLEMENTED() ASSERT(false)
#define UNREACHABLE() DEBUG_ASSERT(false)
#define TODO() ASSERT(false)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Alignment and offsets
///////////////////////////////////////////////////////////////////////////////

#if defined(COMPILER_MSVC)
#  define PACKED_STRUCT(s) __pragma(pack(push, 1)) struct s __pragma(pack(pop))
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#  define PACKED_STRUCT(s) struct __attribute__((packed)) s
#else
#  error PACKED_STRUCT not defined for this compiler.
#endif

#if defined(COMPILER_MSVC) || defined(COMPILER_CLANG)
#  define ALIGN_OF(T) __alignof(T)
#elif defined(COMPILER_GCC)
#  define ALIGN_OF(T) __alignof__(T)
#else
#  error ALIGN_OF not defined for this compiler.
#endif
#define ALIGN_POW_2(x, b) (((x) + (b) - 1) & (~((b) - 1)))

#define MEMBER(T, m)                    (((T*) 0)->m)
#define OFFSET_OF(T, m)                 (U64) (&MEMBER(T,m))
#define MEMBER_FROM_OFFSET(T, ptr, off) (T)((((U8*) ptr) + (off)))
#define CAST_FROM_MEMBER(T, m, ptr)     (T*)(((U8*) ptr) - OFFSET_OF(T, m))

///////////////////////////////////////////////////////////////////////////////
// NOTE: Memory
///////////////////////////////////////////////////////////////////////////////

#define MEMORY_COPY(dst, src, size) memcpy((dst), (src), (size))
#define MEMORY_MOVE(dst, src, size) memmove((dst), (src), (size))
#define MEMORY_SET(dst, byte, size) memset((dst), (byte), (size))
#define MEMORY_COMPARE(a, b, size)  memcmp((a), (b), (size))

#define MEMORY_COPY_STRUCT(d, s)       MEMORY_COPY((d), (s), sizeof(*(d)))
#define MEMORY_COPY_STATIC_ARRAY(d, s) MEMORY_COPY((d), (s), sizeof(d))

#define MEMORY_ZERO(dest, size)     MEMORY_SET(dest, 0, size)
#define MEMORY_ZERO_STRUCT(s)       MEMORY_ZERO((s), sizeof(*(s)))
#define MEMORY_ZERO_STATIC_ARRAY(a) MEMORY_ZERO((a), sizeof(a))

#define IS_MEMORY_EQUAL(a, b, size)       (MEMORY_COMPARE((a), (b), (size)) == 0)
#define IS_MEMORY_EQUAL_STRUCT(a, b)      IS_MEMORY_EQUAL((a), (b), sizeof(*(a)))
#define IS_MEMORY_EQUAL_STATIC_ARRAY(a,b) IS_MEMORY_EQUAL((a), (b), sizeof(a))

void* MemoryReserve(U64 size);
B32   MemoryCommit(void* ptr, U64 size);
void  MemoryRelease(void* ptr, U64 size);
void  MemoryDecommit(void* ptr, U64 size);

///////////////////////////////////////////////////////////////////////////////
// NOTE: List macros
///////////////////////////////////////////////////////////////////////////////

// NOTE: Singly-linked stack
// E.g.
// struct Node { Node* next; }
// struct Stack { Node* head; }
//
// Stack stack = {0};
// Node node = {0};
// SLL_STACK_PUSH(stack.head, &node, next);
// SLL_STACK_POP(stack.head, next);

#define SLL_STACK_PUSH(head, curr_node, next) \
  do {                                        \
    (curr_node)->next = (head);               \
    (head) = (curr_node);                     \
  } while (0)
#define SLL_STACK_POP(head, next) \
  (head) = (head)->next

// NOTE: Singly-linked queue
//
//        ->next
//   o - o - o
// head     tail
//
// E.g.
// struct Node { Node* next; }
// struct Queue { Node* head, tail; }
//
// Queue queue = {0};
// Node node = {0};
// SLL_QUEUE_PUSH(queue.head, queue.tail, &node, next);
// SLL_QUEUE_POP(queue.head, queue.tail, next);

#define SLL_QUEUE_PUSH_FRONT(head, tail, curr_node, next) \
  if ((head) == NULL) {                                   \
    (head) = (curr_node);                                 \
    (tail) = (curr_node);                                 \
    (curr_node)->next = NULL;                             \
  }                                                       \
  else {                                                  \
    (curr_node)->next = (head);                           \
    (head) = (curr_node);                                 \
  }
#define SLL_QUEUE_PUSH_BACK(head, tail, curr_node, next) \
  if ((head) == NULL) {                                  \
    (head) = (curr_node);                                \
    (tail) = (curr_node);                                \
    (curr_node)->next = NULL;                            \
  }                                                      \
  else {                                                 \
    (tail)->next = (curr_node);                          \
    (tail) = (curr_node);                                \
  }
#define SLL_QUEUE_POP(head, tail, next) \
  if ((head) == (tail)) {               \
    (head) = NULL;                      \
    (tail) = NULL;                      \
  }                                     \
  else {                                \
    (head) = (head)->next;              \
  }

// NOTE: Doubly-linked list
//
// prev<- ->next
//   o - o - o
// head     tail
//
// E.g.
// struct Node { Node* prev, next; }
// struct NodeManager { Node* head, tail; }
//
// NodeManager man = {0};
// NodeManager node = {0};
// DLL_PUSH_BACK(man.head, man.tail, &node, prev, next);
// DLL_POP_BACK(man.head, man.tail, prev, next);

// NOTE: head, tail, prev_node, curr_node are node pointers.
// prev, next are the member field names.
#define DLL_INSERT(head, tail, prev_node, curr_node, prev, next) \
  if ((head) == NULL) {                                          \
    (head) = (curr_node);                                        \
    (tail) = (curr_node);                                        \
    (curr_node)->next = NULL;                                    \
    (curr_node)->prev = NULL;                                    \
  }                                                              \
  else if ((prev_node) == NULL) {                                \
    (curr_node)->next = (tail);                                  \
    (tail)->prev = (curr_node);                                  \
    (tail) = (curr_node);                                        \
    (curr_node)->prev = NULL;                                    \
  }                                                              \
  else if ((prev_node) == (head)) {                              \
    (head)->next = (curr_node);                                  \
    (curr_node)->prev = (head);                                  \
    (head) = (curr_node);                                        \
    (curr_node)->next = NULL;                                    \
  }                                                              \
  else {                                                         \
    (prev_node)->next->prev = (curr_node);                       \
    (curr_node)->next = (prev_node)->next;                       \
    (prev_node)->next = (curr_node);                             \
    (curr_node)->prev = (prev_node);                             \
  }
#define DLL_REMOVE(head, tail, curr_node, prev, next)   \
  if ((curr_node) == (tail) && (curr_node) == (head)) { \
    tail = NULL;                                        \
    head = NULL;                                        \
  }                                                     \
  else if ((curr_node) == (tail)) {                     \
    (tail) = (tail)->next;                              \
    tail->prev = NULL;                                  \
  }                                                     \
  else if ((curr_node) == (head)) {                     \
    (head) = (head)->prev;                              \
    head->next = NULL;                                  \
  }                                                     \
  else {                                                \
    if ((curr_node)->prev != NULL) {                    \
      (curr_node)->prev->next = (curr_node)->next;      \
    }                                                   \
    if ((curr_node)->next != NULL) {                    \
      (curr_node)->next->prev = (curr_node)->prev;      \
    }                                                   \
  }
#define DLL_PUSH_BACK(head, tail, curr_node, prev, next) \
  if ((tail) == NULL) {                                  \
    (head) = (curr_node);                                \
    (tail) = (curr_node);                                \
    (curr_node)->next = NULL;                            \
    (curr_node)->prev = NULL;                            \
  }                                                      \
  else {                                                 \
    (curr_node)->next = (tail);                          \
    (tail)->prev = (curr_node);                          \
    (tail) = (curr_node);                                \
    (curr_node)->prev = NULL;                            \
  }
#define DLL_PUSH_FRONT(head, tail, curr_node, prev, next) \
  if ((head) == NULL) {                                   \
    (head) = (curr_node);                                 \
    (tail) = (curr_node);                                 \
    (curr_node)->next = NULL;                             \
    (curr_node)->prev = NULL;                             \
  }                                                       \
  else {                                                  \
    (curr_node)->prev = (head);                           \
    (head)->next = (curr_node);                           \
    (head) = (curr_node);                                 \
    (curr_node)->next = NULL;                             \
  }
#define DLL_POP_FRONT(head, tail, prev, next) \
    DLL_REMOVE(head, tail, head, prev, next);
#define DLL_POP_BACK(head, tail, prev, next)  \
    DLL_REMOVE(head, tail, tail, prev, next);

// NOTE: Dynamic array.
//
// E.g.
// struct List {
//   U32* data;
//   U32 size;
//   U32 capacity;
// }
//
// Arena* arena = ArenaAllocate();
// List list;
// MEMORY_ZERO_STRUCT(&list);
// DA_PUSH_BACK(arena, &list, 10);
// DA_INSERT(arena, &list, 10, 0);
// DA_SWAP_REMOVE(&list, 0);
//
// NOTE: while the list is active, it must have *exclusive* access to the arena,
// otherwise data may moved around in a confusing / unexpected way. probably best
// to just have a dedicated arena per dynamic array to reduce the chance of confusion.

#ifndef DA_INITIAL_CAPACITY
#define DA_INITIAL_CAPACITY 10
#endif

#define DA_RESERVE(arena, list, expected_capacity) \
  DA_RESERVE_EX(arena, (list)->data, (list)->capacity, expected_capacity);
#define DA_PUSH_BACK(arena, list, item) \
  DA_PUSH_BACK_EX(arena, (list)->data, (list)->size, (list)->capacity, item);
#define DA_POP_BACK(list) \
  DA_POP_BACK_EX((list)->size);
#define DA_INSERT(arena, list, item, idx) \
  DA_INSERT_EX(arena, (list)->data, (list)->size, (list)->capacity, item, idx);
#define DA_SWAP_REMOVE(list, idx) \
  DA_SWAP_REMOVE_EX((list)->data, (list)->size, idx);
#define DA_SHIFT_REMOVE(list, idx) \
  DA_SHIFT_REMOVE_EX((list)->data, (list)->size, idx);

#define DA_RESERVE_EX(arena, data, capacity, expected_capacity) \
  if (capacity == 0) {                                          \
    capacity = DA_INITIAL_CAPACITY;                             \
    data = _ArenaPush(arena, sizeof(*data) * capacity, 1);      \
  }                                                             \
  while (capacity < expected_capacity) {                        \
    _ArenaPush(arena, sizeof(*data) * capacity, 1);             \
    capacity *= 2;                                              \
  }
#define DA_PUSH_BACK_EX(arena, data, size, capacity, item) \
  DA_RESERVE_EX(arena, data, capacity, size + 1);          \
  data[size++] = item;
#define DA_POP_BACK_EX(size) \
  DEBUG_ASSERT(size > 0);    \
  size--;
#define DA_INSERT_EX(arena, data, size, capacity, item, idx)       \
  DEBUG_ASSERT(idx <= size);                                       \
  DA_RESERVE_EX(arena, data, capacity, size + 1);                  \
  for (S32 _i = size; _i > idx; _i--) { data[_i] = data[_i - 1]; } \
  data[idx] = item;                                                \
  size++;
#define DA_SWAP_REMOVE_EX(data, size, idx) \
  DEBUG_ASSERT(idx < size);                \
  data[idx] = data[size - 1];              \
  size--;
#define DA_SHIFT_REMOVE_EX(data, size, idx)                              \
  DEBUG_ASSERT(idx < size);                                              \
  for (S32 _i = idx; _i < (S32) size; _i++) { data[_i] = data[_i + 1]; } \
  size--;

///////////////////////////////////////////////////////////////////////////////
// NOTE: Log
///////////////////////////////////////////////////////////////////////////////

// NOTE: If not initialized, will print to stderr.
void LogInit(FILE* fd);
void LogNoPrefix(char* fmt, ...);
void Log(char* level, char* filename, U32 loc, char* fmt, ...);
#define LOG_NO_PREFIX(fmt, ...)  LogNoPrefix(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  Log("INFO",  FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  Log(ANSI_COLOR_YELLOW "WARN" ANSI_COLOR_RESET, FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Log(ANSI_COLOR_RED "ERR " ANSI_COLOR_RESET, FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#ifndef NDEBUG
#  define LOG_DEBUG(fmt, ...) Log(ANSI_COLOR_BLUE "DEBUG" ANSI_COLOR_RESET, FILENAME, fmt, ##__VA_ARGS__)
#else
#  define LOG_DEBUG(fmt, ...)
#endif

///////////////////////////////////////////////////////////////////////////////
// NOTE: Arena
///////////////////////////////////////////////////////////////////////////////

#define PAGE_SIZE KB(4)
#define CDEFAULT_ARENA_RESERVE_SIZE ALIGN_POW_2(MB(64), PAGE_SIZE)
#define CDEFAULT_ARENA_COMMIT_SIZE  ALIGN_POW_2(PAGE_SIZE, PAGE_SIZE)

typedef struct Arena Arena;
struct Arena {
  U64 reserve_size;
  U64 commit_size;
  U64 commit;
  U64 pos;
};

#define ARENA_PUSH_ARRAY(arena, type, count) (type*) _ArenaPush(arena, sizeof(type) * (count), MAX(8, ALIGN_OF(type)))
#define ARENA_PUSH_STRUCT(arena, type) ARENA_PUSH_ARRAY(arena, type, 1)
#define ARENA_POP_ARRAY(arena, type, count) ArenaPop(arena, sizeof(type) * (count))
#define ARENA_POP_STRUCT(arena, type) ARENA_POP_ARRAY(arena, type, 1)

#define ArenaAllocate() _ArenaAllocate(CDEFAULT_ARENA_RESERVE_SIZE, CDEFAULT_ARENA_COMMIT_SIZE)
Arena* _ArenaAllocate(U64 reserve_size, U64 commit_size);
void  ArenaRelease(Arena* arena);
void* _ArenaPush(Arena* arena, U64 size, U64 align);
U64   ArenaPos(Arena* arena);
void  ArenaPopTo(Arena* arena, U64 pos);
void  ArenaPop(Arena* arena, U64 size);
void  ArenaClear(Arena* arena);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Thread
///////////////////////////////////////////////////////////////////////////////

#if defined(OS_WINDOWS)
typedef HANDLE Thread;
typedef CRITICAL_SECTION Mutex;
typedef CONDITION_VARIABLE CV;
#else
// TODO: use OS native structs for linux / mac
typedef thrd_t Thread;
typedef mtx_t Mutex;
typedef cnd_t CV;
#endif
typedef S32 ThreadStart_Fn(void*);
typedef S32 LockWitness;

void ThreadCreate(Thread* thread, ThreadStart_Fn* entry, void* arg);
void ThreadDetach(Thread* thread);
S32  ThreadJoin(Thread* thread);

void MutexInit(Mutex* mutex);
void MutexDeinit(Mutex* mutex);
LockWitness MutexLock(Mutex* mutex);
void MutexUnlock(Mutex* mutex);

void CVInit(CV* cv);
void CVDeinit(CV* cv);
void CVSignal(CV* cv);
void CVBroadcast(CV* cv);
void CVWait(CV* cv, Mutex* mutex);

typedef struct Sem Sem;
struct Sem {
  Mutex mutex;
  CV cv;
  S8 count;
};

void SemInit(Sem* sem, S8 count);
void SemDeinit(Sem* sem);
void SemSignal(Sem* sem);
void SemWait(Sem* sem);

// NOTE: Notifications are shorthand for semaphores init'd with count = 0.
typedef Sem Notif;
void NotifInit(Notif* notification);
void NotifSignal(Notif* notification);
void NotifWaitAndDeinit(Notif* notification);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Atomic
///////////////////////////////////////////////////////////////////////////////

#ifdef OS_WINDOWS
typedef volatile LONG64 AtomicS64;
typedef volatile LONG AtomicS32;
typedef volatile LONG AtomicB32;
#else
typedef _Atomic(S64) AtomicS64;
typedef _Atomic(S32) AtomicS32;
typedef _Atomic(B32) AtomicB32;
#endif

void AtomicS64Init(AtomicS64* a, S64 desired);
void AtomicS64Store(AtomicS64* a, S64 desired);
S64  AtomicS64Load(AtomicS64* a);
S64  AtomicS64Exchange(AtomicS64* a, S64 desired);
B8   AtomicS64CompareExchange(AtomicS64* a, S64* expected, S64 desired);
S64  AtomicS64FetchAdd(AtomicS64* a, S64 b);
S64  AtomicS64FetchSub(AtomicS64* a, S64 b);
S64  AtomicS64FetchOr(AtomicS64* a, S64 b);
S64  AtomicS64FetchXor(AtomicS64* a, S64 b);
S64  AtomicS64FetchAnd(AtomicS64* a, S64 b);

void AtomicS32Init(AtomicS32* a, S32 desired);
void AtomicS32Store(AtomicS32* a, S32 desired);
S32  AtomicS32Load(AtomicS32* a);
S32  AtomicS32Exchange(AtomicS32* a, S32 desired);
B8   AtomicS32CompareExchange(AtomicS32* a, S32* expected, S32 desired);
S32  AtomicS32FetchAdd(AtomicS32* a, S32 b);
S32  AtomicS32FetchSub(AtomicS32* a, S32 b);
S32  AtomicS32FetchOr(AtomicS32* a, S32 b);
S32  AtomicS32FetchXor(AtomicS32* a, S32 b);
S32  AtomicS32FetchAnd(AtomicS32* a, S32 b);

void AtomicB32Init(AtomicB32* a, B32 desired);
void AtomicB32Store(AtomicB32* a, B32 desired);
B32  AtomicB32Load(AtomicB32* a);
B32  AtomicB32Exchange(AtomicB32* a, B32 desired);
B8   AtomicB32CompareExchange(AtomicB32* a, B32* expected, B32 desired);
B32  AtomicB32FetchOr(AtomicB32* a, B32 b);
B32  AtomicB32FetchXor(AtomicB32* a, B32 b);
B32  AtomicB32FetchAnd(AtomicB32* a, B32 b);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Time
///////////////////////////////////////////////////////////////////////////////

// NOTE: TimeInit must be called before any time functions can be used.
void TimeInit();
F32  TimeSecondsSinceStart();
void SleepMs(S32 ms);

typedef struct Stopwatch Stopwatch;
struct Stopwatch { F32 start_time; };
void StopwatchInit(Stopwatch* stopwatch);
void StopwatchReset(Stopwatch* stopwatch);
F32  StopwatchReadSeconds(Stopwatch* stopwatch);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Random
///////////////////////////////////////////////////////////////////////////////

typedef struct RandomSeries RandomSeries;
struct RandomSeries { U64 state; };

// NOTE: Can pass NULL as rand to use a static global default.
void RandSeed(RandomSeries* rand, U64 seed);
B8   RandB8(RandomSeries* rand);
B16  RandB16(RandomSeries* rand);
B32  RandB32(RandomSeries* rand);
B64  RandB64(RandomSeries* rand);
U32  RandU8(RandomSeries* rand, U8 min, U8 max);
U32  RandU16(RandomSeries* rand, U16 min, U16 max);
U32  RandU32(RandomSeries* rand, U16 min, U32 max);
U64  RandU64(RandomSeries* rand, U64 min, U64 max);
S8   RandS8(RandomSeries* rand, S8 min, S8 max);
S16  RandS16(RandomSeries* rand, S16 min, S16 max);
S32  RandS32(RandomSeries* rand, S32 min, S32 max);
S64  RandS64(RandomSeries* rand, S64 min, S64 max);
F32  RandF32(RandomSeries* rand, F32 min, F32 max);
F64  RandF64(RandomSeries* rand, F64 min, F64 max);

///////////////////////////////////////////////////////////////////////////////
// NOTE: String
///////////////////////////////////////////////////////////////////////////////

typedef struct String8 String8;
struct String8 {
  U8* str;
  S32 size;
};

typedef struct String8ListNode String8ListNode;
struct String8ListNode {
  String8ListNode* next;
  String8 string;
};

typedef struct String8List String8List;
struct String8List {
  String8ListNode* head;
  String8ListNode* tail;
};

B32  CharIsWhitespace(U8 c);
B32  CharIsLower(U8 c);
B32  CharIsUpper(U8 c);
B32  CharIsAlpha(U8 c);
B32  CharIsDigit(U8 c);
B32  CharIsDigitEx(U8 c, U32 base);
U8   CharToLower(U8 c);
U8   CharToUpper(U8 c);

S32  CStrSize(U8* str);
U8*  CStrFromStr8(Arena* arena, String8 s);
U8*  CStrCopy(Arena* arena, U8* src); // TODO: Copy with sized buffer
U8*  CStrSubstring(Arena* arena, U8* s, S32 start, S32 one_past_last);
U8*  CStrConcat(Arena* arena, U8* a, U8* b);
U8*  CStrTrim(Arena* arena, U8* s);
U8*  CStrReplaceAll(Arena* arena, U8* src, U8* from, U8* to); // NOTE: Replaces occurrences of a with b in src, and places the result in dest.
B32  CStrReplaceAllChar(U8* str, U8 from, U8 to); // NOTE: Replaces occurrences of a with b inplace in str.
B32  CStrEq(U8* a, U8* b);
S32  CStrFind(U8* str, S32 start_pos, U8* needle); // NOTE: Returns -1 on failure.
S32  CStrFindReverse(U8* str, S32 start_pos, U8* needle); // NOTE: Returns -1 on failure.
B32  CStrStartsWith(U8* a, U8* b);
B32  CStrEndsWith(U8* a, U8* b);
void CStrToUpper(U8* s);
void CStrToLower(U8* s);
U8*  CStrFormatV(Arena* arena, U8* fmt, va_list args);
U8*  _CStrFormat(Arena* arena, U8* fmt, ...);
#define CStrFormat(a, fmt, ...) _CStrFormat(a, (U8) fmt, ##__VA_ARGS__)

String8 Str8(U8* str, S32 size);
String8 Str8Range(U8* str, U8* one_past_last);
String8 _Str8CStr(U8* c_str);
#define Str8CStr(s) _Str8CStr((U8*) s)
#define Str8Lit(s) Str8((U8*) s, sizeof(s) - 1)
String8 Str8Copy(Arena* arena, String8 string);
String8 Str8Substring(String8 s, S32 start, S32 one_past_last);
String8 Str8TrimFront(String8 s);
String8 Str8TrimBack(String8 s);
String8 Str8Trim(String8 s);
String8 Str8ReplaceAll(Arena* arena, String8 src, String8 from, String8 to);
B32     Str8ReplaceAllChar(String8* str, U8 from, U8 to);
void    Str8ToUpper(String8 s);
void    Str8ToLower(String8 s);
S32     Str8ToF32(String8 s, F32* f32); // NOTE: returns char width of float, -1 on no float.
S32     Str8ToS32(String8 s, S32* s32); // NOTE: returns char width of float, -1 on no float.
B32     Str8StartsWith(String8 a, String8 b); // NOTE: True iff a starts with b.
B32     Str8StartsWithChar(String8 a, U8 b);
B32     Str8EndsWith(String8 a, String8 b); // NOTE: True iff a ends with b.
B32     Str8Eq(String8 a, String8 b);
S32     Str8Find(String8 string, S32 start_pos, String8 needle); // NOTE: Returns -1 on failure.
S32     Str8FindReverse(String8 string, S32 reverse_start_pos, String8 needle); // NOTE: Returns -1 on failure.
String8 Str8Concat(Arena* arena, String8 a, String8 b);
String8 Str8FormatV(Arena* arena, U8* fmt, va_list args);
String8 _Str8Format(Arena* arena, U8* fmt, ...);
#define Str8Format(a, fmt, ...) _Str8Format(a, (U8*) fmt, ##__VA_ARGS__)

void    Str8ListPrepend(String8List* list, String8ListNode* node);
void    Str8ListAppend(String8List* list, String8ListNode* node);
String8 Str8ListJoin(Arena* arena, String8List* list);
String8List Str8Split(Arena* arena, String8 string, U8 c);

S32 Str8Hash(String8 s);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Sort
///////////////////////////////////////////////////////////////////////////////

// Single-threaded quicksort implementation

// E.g.
#if 0
U8 my_bytes[3] = { 3, 1, 2 };
SORT_ASC(U8, my_bytes, 3);  --> { 1, 2, 3 }
SORT_DESC(U8, my_bytes, 3); --> { 3, 2, 1 }
#endif

// Or:
#if 0
S32 MyComparison(void* a, void* b) { return *(U8*) a - *(U8*) b; }
U8 my_bytes[3] = { 3, 1, 2 };
SORT(U8, my_bytes, 3, MyComparison); --> { 1, 2, 3 }
#endif

// SORT_ASC and SORT_DESC work with cdefault types out of the box, other types
// will require custom comparison fn implementations.

#define SORT_DESC(type, items, items_len)                                    \
  SORT(type, items, items_len, SortCompare##type##Desc)
#define SORT_ASC(type, items, items_len)                                     \
  SORT(type, items, items_len, SortCompare##type##Asc)
#define SORT(type, items, items_len, compare_fn)                             \
  STATIC_ASSERT(sizeof(type) == sizeof(*items), "SORT type size mismatch!"); \
  do {                                                                       \
    type _t;                                                                 \
    _Sort((void*) items, items_len, sizeof(type), compare_fn, (void*) &_t);  \
  } while (0)

// NOTE: Result is negative if a < b, positive if a > b, and 0 if a == b. Ideally, return a - b.
typedef S32 SortCompare_Fn(void* a, void* b);
void _Sort(void* items, U32 items_len, U32 item_size, SortCompare_Fn* compare_fn, void* temp_buffer);

// NOTE: Comparison functions
S32 SortCompareS8Asc(void* a, void* b);
S32 SortCompareS16Asc(void* a, void* b);
S32 SortCompareS32Asc(void* a, void* b);
S32 SortCompareS64Asc(void* a, void* b);
S32 SortCompareU8Asc(void* a, void* b);
S32 SortCompareU16Asc(void* a, void* b);
S32 SortCompareU32Asc(void* a, void* b);
S32 SortCompareU64Asc(void* a, void* b);
S32 SortCompareF32Asc(void* a, void* b);
S32 SortCompareF64Asc(void* a, void* b);
S32 SortCompareB8Asc(void* a, void* b);
S32 SortCompareB16Asc(void* a, void* b);
S32 SortCompareB32Asc(void* a, void* b);
S32 SortCompareB64Asc(void* a, void* b);
S32 SortCompareString8Asc(void* a, void* b); // NOTE: Lexicographic ordering

S32 SortCompareS8Desc(void* a, void* b);
S32 SortCompareS16Desc(void* a, void* b);
S32 SortCompareS32Desc(void* a, void* b);
S32 SortCompareS64Desc(void* a, void* b);
S32 SortCompareU8Desc(void* a, void* b);
S32 SortCompareU16Desc(void* a, void* b);
S32 SortCompareU32Desc(void* a, void* b);
S32 SortCompareU64Desc(void* a, void* b);
S32 SortCompareF32Desc(void* a, void* b);
S32 SortCompareF64Desc(void* a, void* b);
S32 SortCompareB8Desc(void* a, void* b);
S32 SortCompareB16Desc(void* a, void* b);
S32 SortCompareB32Desc(void* a, void* b);
S32 SortCompareB64Desc(void* a, void* b);
S32 SortCompareString8Desc(void* a, void* b); // NOTE: Lexicographic ordering

///////////////////////////////////////////////////////////////////////////////
// NOTE: Bin read / write
///////////////////////////////////////////////////////////////////////////////

// NOTE: Unsafe conversion from raw bytes to some sized representation.

U8  BinRead8(U8* bytes);
U16 BinRead16LE(U8* bytes);
U16 BinRead16BE(U8* bytes);
U32 BinRead32LE(U8* bytes);
U32 BinRead32BE(U8* bytes);
U64 BinRead64LE(U8* bytes);
U64 BinRead64BE(U8* bytes);

// NOTE: Safe wrapper around the above. Throws debug (!!) assertions when exceeding bounds.
// Orients around a pos cursor, which is convenient for reading / writing file bin blobs.

typedef struct BinStream BinStream;
struct BinStream {
  U8* bytes;
  U64 bytes_size;
  U64 pos;
};

void BinStreamInit(BinStream* stream, U8* bytes, U32 bytes_size);
void BinStreamSeek(BinStream* stream, U32 pos);
void BinStreamSkip(BinStream* stream, U32 num, S32 size);
U8*  BinStreamDecay(BinStream* stream);

U8   BinStreamPull8(BinStream* stream);
U16  BinStreamPull16LE(BinStream* stream);
U16  BinStreamPull16BE(BinStream* stream);
U32  BinStreamPull32LE(BinStream* stream);
U32  BinStreamPull32BE(BinStream* stream);
U64  BinStreamPull64LE(BinStream* stream);
U64  BinStreamPull64BE(BinStream* stream);

void BinStreamPush8(BinStream* stream, U8 x);
void BinStreamPush16LE(BinStream* stream, U16 x);
void BinStreamPush16BE(BinStream* stream, U16 x);
void BinStreamPush32LE(BinStream* stream, U32 x);
void BinStreamPush32BE(BinStream* stream, U32 x);
void BinStreamPush64LE(BinStream* stream, U64 x);
void BinStreamPush64BE(BinStream* stream, U64 x);

U8   BinStreamPeek8(BinStream* stream, S32 offset, U32 size);
U16  BinStreamPeek16LE(BinStream* stream, S32 offset, U32 size);
U16  BinStreamPeek16BE(BinStream* stream, S32 offset, U32 size);
U32  BinStreamPeek32LE(BinStream* stream, S32 offset, U32 size);
U32  BinStreamPeek32BE(BinStream* stream, S32 offset, U32 size);
U64  BinStreamPeek64LE(BinStream* stream, S32 offset, U32 size);
U64  BinStreamPeek64BE(BinStream* stream, S32 offset, U32 size);

#endif // CDEFAULT_H_

#ifdef CDEFAULT_STD_IMPLEMENTATION
#undef CDEFAULT_STD_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Bit manipulation implementation
///////////////////////////////////////////////////////////////////////////////

S32 U32LsbPos(U32 x) {
  if (x == 0) { return -1; }
  int result = 0;
  if ((x & 0xFFFF) == 0) { x >>= 16; result += 16; }
  if ((x & 0xFF) == 0)   { x >>= 8;  result += 8;  }
  if ((x & 0xF) == 0)    { x >>= 4;  result += 4;  }
  if ((x & 0x3) == 0)    { x >>= 2;  result += 2;  }
  if ((x & 0x1) == 0)    {           result += 1;  }
  return result;
}

S32 U32MsbPos(U32 x) {
  if (x == 0) { return -1; }
  U32 result = 0;
  if (x >= (1U << 16)) { x >>= 16; result += 16; }
  if (x >= (1U << 8))  { x >>= 8;  result += 8;  }
  if (x >= (1U << 4))  { x >>= 4;  result += 4;  }
  if (x >= (1U << 2))  { x >>= 2;  result += 2;  }
  if (x >= (1U << 1))  {           result += 1;  }
  return result;
}

U32 U32CountBits(U32 x) {
  U32 result = 0;
  while (x != 0) { x &= (x - 1); result += 1; }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Memory implementation
///////////////////////////////////////////////////////////////////////////////

void* MemoryReserve(U64 size) {
#if defined(OS_WINDOWS)
  return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
#elif defined(OS_LINUX) || defined(OS_MAC)
  void* result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (result == MAP_FAILED) { result = NULL; }
  return result;
#endif
}

B32 MemoryCommit(void* ptr, U64 size) {
#if defined(OS_WINDOWS)
  return (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
#elif defined(OS_LINUX) || defined(OS_MAC)
  mprotect(ptr, size, PROT_READ | PROT_WRITE);
  return true;
#endif
}

void MemoryRelease(void* ptr, U64 size) {
#if defined(OS_WINDOWS)
  size = size;
  VirtualFree(ptr, 0, MEM_RELEASE);
#elif defined(OS_LINUX) || defined(OS_MAC)
  munmap(ptr, size);
#endif
}

void MemoryDecommit(void* ptr, U64 size) {
#if defined(OS_WINDOWS)
  VirtualFree(ptr, size, MEM_DECOMMIT);
#elif defined(OS_LINUX) || defined(OS_MAC)
  madvise(ptr, size, MADV_DONTNEED);
  mprotect(ptr, size, PROT_NONE);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Log implementation
///////////////////////////////////////////////////////////////////////////////

// TODO: use the in-house file api. need to handle stdout / stderr first.
typedef struct LogConfig LogConfig;
struct LogConfig {
  FILE* fd;
  Mutex mtx;
  B8 is_initialized;
};
static LogConfig _cdef_log_config;

void LogInit(FILE* fd) {
  MEMORY_ZERO_STRUCT(&_cdef_log_config);
  _cdef_log_config.fd = fd;
  MutexInit(&_cdef_log_config.mtx);
  _cdef_log_config.is_initialized = true;
}

static void LogNoPrefixV(char* fmt, va_list args) {
  if (UNLIKELY(!_cdef_log_config.is_initialized)) { LogInit(stderr); }
  MutexLock(&_cdef_log_config.mtx);
  vfprintf(_cdef_log_config.fd, fmt, args);
  fprintf(_cdef_log_config.fd, "\n");
  MutexUnlock(&_cdef_log_config.mtx);
}

static void LogV(char* level, char* filename, U32 loc, char* fmt, va_list args) {
  if (UNLIKELY(!_cdef_log_config.is_initialized)) { LogInit(stderr); }
  MutexLock(&_cdef_log_config.mtx);
  fprintf(_cdef_log_config.fd, "[%s | %s:%d]: ", level, filename, loc);
  vfprintf(_cdef_log_config.fd, fmt, args);
  fprintf(_cdef_log_config.fd, "\n");
  MutexUnlock(&_cdef_log_config.mtx);
}

void LogNoPrefix(char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  LogNoPrefixV(fmt, args);
  va_end(args);
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

Arena* _ArenaAllocate(U64 reserve_size, U64 commit_size) {
  DEBUG_ASSERT(sizeof(Arena) < commit_size);
  DEBUG_ASSERT(commit_size < reserve_size);
  DEBUG_ASSERT(reserve_size % commit_size == 0);

  void* base = MemoryReserve(reserve_size);
  ASSERT(base != NULL); // TODO: message box or some other kind of abortion.
  MemoryCommit(base, commit_size);

  Arena* arena = (Arena*) base;
  MEMORY_ZERO_STRUCT(arena);
  arena->reserve_size = reserve_size;
  arena->commit_size = commit_size;
  arena->commit = commit_size;
  arena->pos = sizeof(Arena);
  return arena;
}

void ArenaRelease(Arena* arena) {
  if (UNLIKELY(arena == NULL)) { return; }
  MemoryRelease(arena, CDEFAULT_ARENA_RESERVE_SIZE);
}

void* _ArenaPush(Arena* arena, U64 size, U64 align) {
  U64 pos = ALIGN_POW_2(arena->pos, align);
  U64 pos_after = pos + size;

  ASSERT(pos_after <= arena->reserve_size); // If hit, increase reserve size?
  if (arena->commit < pos_after) {
    U64 commit_size = 0;
    do {
      commit_size += arena->commit_size;
    } while (arena->commit + commit_size < pos_after);
    MemoryCommit((U8*) arena + arena->commit, commit_size);
    arena->commit += commit_size;
  }
  DEBUG_ASSERT(pos_after <= arena->commit);

  void* result = ((U8*) arena) + pos;
  arena->pos = pos_after;
  return result;
}

U64 ArenaPos(Arena* arena) {
  return arena->pos;
}

void ArenaPopTo(Arena* arena, U64 pos) {
  DEBUG_ASSERT(pos <= arena->pos);
  DEBUG_ASSERT(pos >= sizeof(Arena));
  arena->pos = pos;
}

void ArenaPop(Arena* arena, U64 size) {
  DEBUG_ASSERT(arena->pos - size >= sizeof(Arena));
  ArenaPopTo(arena, arena->pos - size);
}

void ArenaClear(Arena* arena) {
  ArenaPopTo(arena, sizeof(Arena));
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Thread Implementation
///////////////////////////////////////////////////////////////////////////////

void ThreadCreate(Thread* thread, ThreadStart_Fn* entry, void* arg) {
#if defined(OS_WINDOWS)
  *thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) entry, arg, 0, 0);
  ASSERT(*thread != NULL);
#else
  S32 result = thrd_create(thread, entry, arg);
  ASSERT(result == thrd_success);
#endif
}

void ThreadDetach(Thread* thread) {
#if defined(OS_WINDOWS)
  CloseHandle(*thread);
#else
  S32 result = thrd_detach(*thread);
  ASSERT(result == thrd_success);
#endif
}

S32 ThreadJoin(Thread* thread) {
#if defined(OS_WINDOWS)
  DWORD result = WaitForSingleObject(*thread, INFINITE);
  ASSERT(result == WAIT_OBJECT_0);
  DWORD exit_code;
  GetExitCodeThread(*thread, &exit_code);
  CloseHandle(*thread);
  return (S32) exit_code;
#else
  S32 out;
  S32 result = thrd_join(*thread, &out);
  ASSERT(result == thrd_success);
  return out;
#endif
}

void MutexInit(Mutex* mutex) {
#if defined(OS_WINDOWS)
  InitializeCriticalSection(mutex);
#else
  S32 result = mtx_init(mutex, mtx_plain);
  ASSERT(result == thrd_success);
#endif
}

void MutexDeinit(Mutex* mutex) {
#if defined(OS_WINDOWS)
  DeleteCriticalSection(mutex);
#else
  mtx_destroy(mutex);
#endif
}

S32 MutexLock(Mutex* mutex) {
#if defined(OS_WINDOWS)
  EnterCriticalSection(mutex);
  return 0;
#else
  S32 result = mtx_lock(mutex);
  ASSERT(result == thrd_success);
  return 0;
#endif
}

void MutexUnlock(Mutex* mutex) {
#if defined(OS_WINDOWS)
  LeaveCriticalSection(mutex);
#else
  S32 result = mtx_unlock(mutex);
  ASSERT(result == thrd_success);
#endif
}

void CVInit(CV* cv) {
#if defined(OS_WINDOWS)
  InitializeConditionVariable(cv);
#else
  S32 result = cnd_init(cv);
  ASSERT(result == thrd_success);
#endif
}

void CVDeinit(CV* cv) {
#if defined(OS_WINDOWS)
  cv = cv; // NOTE: silence unused compiler warnings.
#else
  cnd_destroy(cv);
#endif
}

void CVSignal(CV* cv) {
#if defined(OS_WINDOWS)
  WakeConditionVariable(cv);
#else
  S32 result = cnd_signal(cv);
  ASSERT(result == thrd_success);
#endif
}

void CVBroadcast(CV* cv) {
#if defined(OS_WINDOWS)
  WakeAllConditionVariable(cv);
#else
  S32 result = cnd_broadcast(cv);
  ASSERT(result == thrd_success);
#endif
}

void CVWait(CV* cv, Mutex* mutex) {
#if defined(OS_WINDOWS)
  BOOL result = SleepConditionVariableCS(cv, mutex, INFINITE);
  ASSERT(result == true);
#else
  S32 result = cnd_wait(cv, mutex);
  ASSERT(result == thrd_success);
#endif
}

void SemInit(Sem* sem, S8 count) {
  ASSERT(count >= 0);
  MEMORY_ZERO_STRUCT(sem);
  MutexInit(&sem->mutex);
  CVInit(&sem->cv);
  sem->count = count;
}

void SemDeinit(Sem* sem) {
  MutexDeinit(&sem->mutex);
  CVDeinit(&sem->cv);
}

void SemSignal(Sem* sem) {
  MutexLock(&sem->mutex);
  sem->count += 1;
  CVSignal(&sem->cv);
  MutexUnlock(&sem->mutex);
}

void SemWait(Sem* sem) {
  MutexLock(&sem->mutex);
  while (sem->count <= 0) { CVWait(&sem->cv, &sem->mutex); }
  sem->count -= 1;
  MutexUnlock(&sem->mutex);
}

void NotifInit(Notif* notification) {
  SemInit(notification, 0);
}

void NotifSignal(Notif* notification) {
  SemSignal(notification);
}

void NotifWaitAndDeinit(Notif* notification) {
  SemWait(notification);
  SemDeinit(notification);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Atomic Implementation
///////////////////////////////////////////////////////////////////////////////

#ifdef OS_WINDOWS

void AtomicS64Init(AtomicS64* a, S64 desired) { InterlockedExchange64(a, desired); }
void AtomicS64Store(AtomicS64* a, S64 desired) { InterlockedExchange64(a, desired); }
S64 AtomicS64Load(AtomicS64* a) { return InterlockedCompareExchange64(a, 0, 0); }
S64 AtomicS64Exchange(AtomicS64* a, S64 desired) { return InterlockedExchange64(a, desired); }
S64 AtomicS64FetchAdd(AtomicS64* a, S64 b) { return InterlockedExchangeAdd64(a, b); }
S64 AtomicS64FetchSub(AtomicS64* a, S64 b) { return InterlockedExchangeAdd64(a, -b); }
S64 AtomicS64FetchOr(AtomicS64* a, S64 b) { return InterlockedOr64(a, b); }
S64 AtomicS64FetchXor(AtomicS64* a, S64 b) { return InterlockedXor64(a, b); }
S64 AtomicS64FetchAnd(AtomicS64* a, S64 b) { return InterlockedAnd64(a, b); }
B8 AtomicS64CompareExchange(AtomicS64* a, S64* expected, S64 desired) {
  S64 result = InterlockedCompareExchange64(a, *expected, desired);
  return (result == *expected);
}

void AtomicS32Init(AtomicS32* a, S32 desired) { InterlockedExchange(a, desired); }
void AtomicS32Store(AtomicS32* a, S32 desired) { InterlockedExchange(a, desired); }
S32 AtomicS32Load(AtomicS32* a) { return InterlockedCompareExchange(a, 0, 0); }
S32 AtomicS32Exchange(AtomicS32* a, S32 desired) { return InterlockedExchange(a, desired); }
S32 AtomicS32FetchAdd(AtomicS32* a, S32 b) { return InterlockedExchangeAdd(a, b); }
S32 AtomicS32FetchSub(AtomicS32* a, S32 b) { return InterlockedExchangeAdd(a, -b); }
S32 AtomicS32FetchOr(AtomicS32* a, S32 b) { return InterlockedOr(a, b); }
S32 AtomicS32FetchXor(AtomicS32* a, S32 b) { return InterlockedXor(a, b); }
S32 AtomicS32FetchAnd(AtomicS32* a, S32 b) { return InterlockedAnd(a, b); }
B8 AtomicS32CompareExchange(AtomicS32* a, S32* expected, S32 desired) {
  S32 result = InterlockedCompareExchange(a, *expected, desired);
  return (result == *expected);
}

void AtomicB32Init(AtomicB32* a, B32 desired) { InterlockedExchange(a, desired); }
void AtomicB32Store(AtomicB32* a, B32 desired) { InterlockedExchange(a, desired); }
B32 AtomicB32Load(AtomicB32* a) { return InterlockedCompareExchange(a, 0, 0); }
B32 AtomicB32Exchange(AtomicB32* a, B32 desired) { return InterlockedExchange(a, desired); }
B32 AtomicB32FetchOr(AtomicB32* a, B32 b) { return InterlockedOr(a, b); }
B32 AtomicB32FetchXor(AtomicB32* a, B32 b) { return InterlockedXor(a, b); }
B32 AtomicB32FetchAnd(AtomicB32* a, B32 b) { return InterlockedAnd(a, b); }
B8 AtomicB32CompareExchange(AtomicB32* a, B32* expected, B32 desired) {
  B32 result = InterlockedCompareExchange(a, *expected, desired);
  return (result == *expected);
}

#else

void AtomicS64Init(AtomicS64* a, S64 desired) { atomic_init(a, desired); }
void AtomicS64Store(AtomicS64* a, S64 desired) { atomic_store(a, desired); }
S64 AtomicS64Load(AtomicS64* a) { return atomic_load(a); }
S64 AtomicS64Exchange(AtomicS64* a, S64 desired) { return atomic_exchange(a, desired); }
S64 AtomicS64FetchAdd(AtomicS64* a, S64 b) { return atomic_fetch_add(a, b); }
S64 AtomicS64FetchSub(AtomicS64* a, S64 b) { return atomic_fetch_sub(a, b); }
S64 AtomicS64FetchOr(AtomicS64* a, S64 b) { return atomic_fetch_or(a, b); }
S64 AtomicS64FetchXor(AtomicS64* a, S64 b) { return atomic_fetch_xor(a, b); }
S64 AtomicS64FetchAnd(AtomicS64* a, S64 b) { return atomic_fetch_and(a, b); }
B8 AtomicS64CompareExchange(AtomicS64* a, S64* expected, S64 desired) {
  return atomic_compare_exchange_strong(a, expected, desired);
}

void AtomicS32Init(AtomicS32* a, S32 desired) { atomic_init(a, desired); }
void AtomicS32Store(AtomicS32* a, S32 desired) { atomic_store(a, desired); }
S32 AtomicS32Load(AtomicS32* a) { return atomic_load(a); }
S32 AtomicS32Exchange(AtomicS32* a, S32 desired) { return atomic_exchange(a, desired); }
S32 AtomicS32FetchAdd(AtomicS32* a, S32 b) { return atomic_fetch_add(a, b); }
S32 AtomicS32FetchSub(AtomicS32* a, S32 b) { return atomic_fetch_sub(a, b); }
S32 AtomicS32FetchOr(AtomicS32* a, S32 b) { return atomic_fetch_or(a, b); }
S32 AtomicS32FetchXor(AtomicS32* a, S32 b) { return atomic_fetch_xor(a, b); }
S32 AtomicS32FetchAnd(AtomicS32* a, S32 b) { return atomic_fetch_and(a, b); }
B8 AtomicS32CompareExchange(AtomicS32* a, S32* expected, S32 desired) {
  return atomic_compare_exchange_strong(a, expected, desired);
}

void AtomicB32Init(AtomicB32* a, B32 desired) { atomic_init(a, desired); }
void AtomicB32Store(AtomicB32* a, B32 desired) { atomic_store(a, desired); }
B32 AtomicB32Load(AtomicB32* a) { return atomic_load(a); }
B32 AtomicB32Exchange(AtomicB32* a, B32 desired) { return atomic_exchange(a, desired); }
B32 AtomicB32FetchOr(AtomicB32* a, B32 b) { return atomic_fetch_or(a, b); }
B32 AtomicB32FetchXor(AtomicB32* a, B32 b) { return atomic_fetch_xor(a, b); }
B32 AtomicB32FetchAnd(AtomicB32* a, B32 b) { return atomic_fetch_and(a, b); }
B8 AtomicB32CompareExchange(AtomicB32* a, B32* expected, B32 desired) {
  return atomic_compare_exchange_strong(a, expected, desired);
}

#endif

///////////////////////////////////////////////////////////////////////////////
// NOTE: Time Implementation
///////////////////////////////////////////////////////////////////////////////

#if defined(OS_WINDOWS)

static B32           _cdef_time_init;
static LARGE_INTEGER _cdef_performance_freq;
static LARGE_INTEGER _cdef_start_time;

void TimeInit() {
  if (_cdef_time_init) { return; }
  QueryPerformanceFrequency(&_cdef_performance_freq);
  QueryPerformanceCounter(&_cdef_start_time);
  _cdef_time_init = true;
}

F32 TimeSecondsSinceStart() {
  DEBUG_ASSERT(_cdef_time_init);
  LARGE_INTEGER time_now;
  QueryPerformanceCounter(&time_now);
  return ((F32) (time_now.QuadPart - _cdef_start_time.QuadPart)) /
    ((F32) _cdef_performance_freq.QuadPart);
}

void SleepMs(S32 ms) {
  Sleep(ms);
}

#elif defined(OS_LINUX)

static B32             _cdef_time_init;
static struct timespec _cdef_start_time;

void TimeInit() {
  if (_cdef_time_init) { return; }
  clock_gettime(CLOCK_MONOTONIC, &_cdef_start_time);
  _cdef_time_init = true;
}

F32 TimeSecondsSinceStart() {
  DEBUG_ASSERT(_cdef_time_init);
  struct timespec time_now;
  clock_gettime(CLOCK_MONOTONIC, &time_now);
  return ((F32) (time_now.tv_sec - _cdef_start_time.tv_sec)) +
    ((F32) (time_now.tv_nsec - _cdef_start_time.tv_nsec) * 1e-9f);
}

void SleepMs(S32 ms) {
  usleep(ms * 1000);
}

#elif defined(OS_MAC)
#error std time module not tested on mac.

static B32 _cdef_time_init;
static U64 _cdef_start_time;
static mach_timebase_info_data_t _cdef_timebase_info;

void TimeInit() {
  if (_cdef_time_init) { return; }
  _cdef_start_time = mach_absolute_time();
  mach_timebase_info(&_cdef_timebase_info);
  _cdef_time_init = true;
}

F32 TimeSecondsSinceStart() {
  DEBUG_ASSERT(_cdef_time_init);
  U64 time_now = mach_absolute_time();
  return ((time_now - _cdef_start_time) * _cdef_timebase_info.numer) /
    _cdef_timebase_info.denom;
}

void SleepMs(S32 ms) {
  usleep(ms * 1000);
}

#endif

void StopwatchInit(Stopwatch* stopwatch) {
  StopwatchReset(stopwatch);
}

void StopwatchReset(Stopwatch* stopwatch) {
  stopwatch->start_time = TimeSecondsSinceStart();
}

F32 StopwatchReadSeconds(Stopwatch* stopwatch) {
  return TimeSecondsSinceStart() - stopwatch->start_time;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Random implementation
///////////////////////////////////////////////////////////////////////////////

static RandomSeries _cdef_rand;

// NOTE: xor shift
static void RandomSeriesShuffle(RandomSeries* rand) {
  rand->state ^= rand->state << 13;
  rand->state ^= rand->state >> 17;
  rand->state ^= rand->state << 5;
}

void RandSeed(RandomSeries* rand, U64 seed) {
  if (rand == NULL) { rand = &_cdef_rand; }
  rand->state = seed;
}

B8 RandB8(RandomSeries* rand) {
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  return rand->state % 2 == 0;
}

B16 RandB16(RandomSeries* rand) {
  return (B16) RandB8(rand);
}

B32 RandB32(RandomSeries* rand) {
  return (B32) RandB8(rand);
}

B64 RandB64(RandomSeries* rand) {
  return (B64) RandB8(rand);
}

U32 RandU8(RandomSeries* rand, U8 min, U8 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U8 r = *(U8*) &rand->state;
  return min + (U8) (r % (max - min));
}

U32 RandU16(RandomSeries* rand, U16 min, U16 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U16 r = *(U16*) &rand->state;
  return min + (U16) (r % (max - min));
}

U32 RandU32(RandomSeries* rand, U16 min, U32 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U32 r = *(U32*) &rand->state;
  return min + (U32) (r % (max - min));
}

U64 RandU64(RandomSeries* rand, U64 min, U64 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U64 r = rand->state;
  return min + (r % (max - min));
}

S8 RandS8(RandomSeries* rand, S8 min, S8 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U8 r = *(U8*) &rand->state;
  return min + (U8) (r % (max - min));
}

S16 RandS16(RandomSeries* rand, S16 min, S16 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U16 r = *(U16*) &rand->state;
  return min + (U16) (r % (max - min));
}

S32 RandS32(RandomSeries* rand, S32 min, S32 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U32 r = *(U32*) &rand->state;
  return min + (U32) (r % (max - min));
}

S64 RandS64(RandomSeries* rand, S64 min, S64 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  U64 r = rand->state;
  return min + (r % (max - min));
}

F32 RandF32(RandomSeries* rand, F32 min, F32 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  F32 r = (F32) rand->state / U64_MAX;
  return min + (r * (max - min));
}

F64 RandF64(RandomSeries* rand, F64 min, F64 max) {
  DEBUG_ASSERT(min < max);
  if (rand == NULL) { rand = &_cdef_rand; }
  RandomSeriesShuffle(rand);
  F64 r = (F64) (rand->state / U64_MAX);
  return min + (r * (max - min));
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

B32 CharIsDigit(U8 c) {
  return CharIsDigitEx(c, 10);
}

B32 CharIsDigitEx(U8 c, U32 base) {
  if (UNLIKELY(base == 0 || base > 16)) { return false; }
  if      ('0' <= c && c <= '9') { c -= '0'; }
  else if (CharIsLower(c))       { c -= 'a'; c += 10; }
  else if (CharIsUpper(c))       { c -= 'A'; c += 10; }
  return (c < base);
}

U8 CharToLower(U8 c) {
  if (CharIsUpper(c)) { c += (U8)('a' - 'A'); }
  return c;
}

U8 CharToUpper(U8 c) {
  if (CharIsLower(c)) { c += (U8)('A' - 'a'); }
  return c;
}

S32 CStrSize(U8* str) {
  U8* end = str;
  while (*end != '\0') { end++; }
  return (S32) (end - str);
}

U8* CStrFromStr8(Arena* arena, String8 s) {
  if (s.size < 0) { return (U8*) ""; }
  S32 size = s.size + 1;
  U8* result = ARENA_PUSH_ARRAY(arena, U8, size);
  MEMORY_COPY(result, s.str, size * sizeof(U8));
  result[s.size] = '\0';
  return result;
}

U8* CStrCopy(Arena* arena, U8* src) {
  U32 size_with_null_terminator = CStrSize(src) + 1;
  U8* result = ARENA_PUSH_ARRAY(arena, U8, size_with_null_terminator);
  MEMORY_COPY(result, src, size_with_null_terminator * sizeof(U8));
  return result;
}

U8* CStrSubstring(Arena* arena, U8* s, S32 start, S32 one_past_last) {
  if (one_past_last < start) { one_past_last = start; }
  S32 size = one_past_last - start;
  U8* result = ARENA_PUSH_ARRAY(arena, U8, size + 1);
  MEMORY_COPY(result, (s + start), size * sizeof(U8));
  result[size] = '\0';
  return result;
}

U8* CStrConcat(Arena* arena, U8* a, U8* b) {
  U32 a_size = CStrSize(a);
  U32 b_size = CStrSize(b);
  U32 size = a_size + b_size + 1;
  U8* result = ARENA_PUSH_ARRAY(arena, U8, size * sizeof(U8));
  MEMORY_COPY(result, a, a_size * sizeof(U8));
  MEMORY_COPY(result + a_size, b, b_size * sizeof(U8));
  result[a_size + b_size] = '\0';
  return result;
}

U8* CStrTrimFront(Arena* arena, U8* s) {
  S32 start = 0;
  S32 size = CStrSize(s);
  while (start < size && CharIsWhitespace(s[start])) { start++; }
  return CStrSubstring(arena, s, start, size);
}

U8* CStrTrimBack(Arena* arena, U8* s) {
  S32 end = CStrSize(s) - 1;
  while (end >= 0 && CharIsWhitespace(s[end])) { end--; }
  return CStrSubstring(arena, s, 0, end + 1);
}

U8* CStrTrim(Arena* arena, U8* s) {
  S32 start = 0;
  S32 size  = CStrSize(s);
  S32 end   = size - 1;
  while (start < size && CharIsWhitespace(s[start])) { start++; }
  if (start < size) { while (end >= start && CharIsWhitespace(s[end])) { end--; } }
  return CStrSubstring(arena, s, start, end + 1);
}

B32 CStrEq(U8* a, U8* b) {
  if (CStrSize(a) != CStrSize(b)) { return false; }
  return CStrStartsWith(a, b);
}

S32 CStrFind(U8* str, S32 start_pos, U8* needle) {
  if (start_pos < 0) { return -1; }
  U32 i = start_pos;
  while (true) {
    U32 i_copy = i;
    U32 j = 0;
    while (true) {
      if (needle[j] == '\0') { return i; }
      if (str[i_copy] == '\0') { return -1; }
      if (str[i_copy] != needle[j]) { break; }
      j++;
      i_copy++;
    }
    i++;
  }
  return -1;
}

S32 CStrFindReverse(U8* str, S32 start_pos, U8* needle) {
  if (start_pos < 0) { return -1; }
  U32 i = start_pos;
  while (true) {
    U32 i_copy = i;
    U32 j = 0;
    while (true) {
      if (needle[j] == '\0') { return i; }
      if (str[i_copy] == '\0') { return -1; }
      if (str[i_copy] != needle[j]) { break; }
      j++;
      i_copy++;
    }
    i--;
  }
  return -1;
}

U8* CStrReplaceAll(Arena* arena, U8* src, U8* from, U8* to) {
  S32 i = 0;
  S32 from_size = CStrSize(from);
  S32 to_size = CStrSize(to);
  S32 result_size = 0;
  while (src[i] != '\0') {
    if (CStrStartsWith(&src[i], from)) {
      i += from_size;
      result_size += to_size;
      continue;
    }
    result_size++;
    i++;
  }
  U8* result = ARENA_PUSH_ARRAY(arena, U8, result_size + 1);
  i = 0;
  S32 k = 0;
  while (src[i] != '\0') {
    if (CStrStartsWith(&src[i], from)) {
      for (S32 j = 0; j < to_size; j++) {
        result[k++] = to[j];
      }
      i += from_size;
      continue;
    }
    result[k++] = src[i++];
  }
  result[k] = '\0';
  return result;
}

B32 CStrReplaceAllChar(U8* str, U8 from, U8 to) {
  B32 result = false;
  U32 i = 0;
  while (str[i] != '\0'){
    if (str[i] == from) {
      result = true;
      str[i] = to;
    }
    i++;
  }
  return result;
}

B32 CStrStartsWith(U8* a, U8* b) {
  U32 i = 0;
  while (true) {
    if (b[i] == '\0') { return true;  }
    if (a[i] == '\0') { return false; }
    if (a[i] != b[i]) { return false; }
    i++;
  }
}

B32 CStrEndsWith(U8* a, U8* b) {
  U32 a_idx = CStrSize(a) - 1;
  U32 b_idx = CStrSize(b) - 1;
  U32 i = 0;
  while (true) {
    if (i > b_idx) { return true;  }
    if (i > a_idx) { return false; }
    if (a[a_idx - i] != b[b_idx - i]) { return false; }
    i++;
  }
}
void CStrToUpper(U8* s) {
  U32 i = 0;
  while (s[i] != '\0') { s[i] = CharToUpper(s[i]); i++; }
}

void CStrToLower(U8* s) {
  U32 i = 0;
  while (s[i] != '\0') { s[i] = CharToLower(s[i]); i++; }
}

U8* CStrFormatV(Arena* arena, U8* fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  U32 size = vsnprintf(NULL, 0, (const char* const) fmt, args_copy) + 1;
  va_end(args_copy);
  U8* result = ARENA_PUSH_ARRAY(arena, U8, size);
  va_copy(args_copy, args);
  vsnprintf((char* const) result, size, (char* const) fmt, args_copy);
  va_end(args_copy);
  return result;
}

U8* _CStrFormat(Arena* arena, U8* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  U8* result = CStrFormatV(arena, fmt, args);
  va_end(args);
  return result;
}

String8 Str8(U8* str, S32 size) {
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.str = str;
  result.size = size;
  return result;
}

String8 _Str8CStr(U8* c_str) {
  U8* c = c_str;
  while (*c != '\0') { c += 1; }
  return Str8Range(c_str, c);
}

String8 Str8Range(U8* str, U8* one_past_last) {
  DEBUG_ASSERT(one_past_last >= str);
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.str = str;
  result.size = (S32) (one_past_last - str);
  return result;
}

String8 Str8Copy(Arena* arena, String8 string) {
  DEBUG_ASSERT(string.size >= 0);
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = string.size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, string.size);
  MEMORY_COPY(result.str, string.str, string.size);
  return result;
}

String8 Str8Substring(String8 s, S32 start, S32 one_past_last) {
  DEBUG_ASSERT(s.size >= 0);
  if (one_past_last < start) { one_past_last = start; }
  start         = MIN(start, s.size);
  one_past_last = MIN(one_past_last, s.size);
  return Str8(s.str + start, one_past_last - start);
}

String8 Str8TrimFront(String8 s) {
  DEBUG_ASSERT(s.size >= 0);
  S32 start = 0;
  while (start < s.size && CharIsWhitespace(s.str[start])) { start++; }
  return Str8Substring(s, start, s.size);
}

String8 Str8TrimBack(String8 s) {
  DEBUG_ASSERT(s.size >= 0);
  S32 end = s.size - 1;
  while (end >= 0 && CharIsWhitespace(s.str[end])) { --end; }
  return Str8Substring(s, 0, end + 1);
}

String8 Str8Trim(String8 s) {
  DEBUG_ASSERT(s.size >= 0);
  return Str8TrimBack(Str8TrimFront(s));
}

String8 Str8ReplaceAll(Arena* arena, String8 src, String8 from, String8 to) {
  DEBUG_ASSERT(src.size >= 0 && from.size >= 0 && to.size >= 0);
  U32 result_size = 0;
  S32 i = 0;
  while (i < src.size) {
    String8 sub = Str8Substring(src, i, src.size);
    if (Str8StartsWith(sub, from)) {
      i += from.size;
      result_size += to.size;
      continue;
    }
    result_size++;
    i++;
  }
  String8 result;
  result.str = ARENA_PUSH_ARRAY(arena, U8, result_size);
  result.size = result_size;
  i = 0;
  S32 k = 0;
  while (i < src.size) {
    String8 sub = Str8Substring(src, i, src.size);
    if (Str8StartsWith(sub, from)) {
      for (S32 j = 0; j < to.size; j++) {
        result.str[k++] = to.str[j];
      }
      i += from.size;
      continue;
    }
    result.str[k++] = src.str[i++];
  }
  return result;
}

B32 Str8ReplaceAllChar(String8* str, U8 from, U8 to) {
  DEBUG_ASSERT(str->size >= 0);
  B32 result = false;
  for (S32 i = 0; i < str->size; i++) {
    if (str->str[i] == from) {
      result = true;
      str->str[i] = to;
    }
  }
  return result;
}

B32 Str8StartsWith(String8 a, String8 b) {
  DEBUG_ASSERT(a.size >= 0 && b.size >= 0);
  if (a.size < b.size) { return false; }
  for (S32 i = 0; i < b.size; ++i) {
    if (a.str[i] != b.str[i]) { return false; }
  }
  return true;
}

B32 Str8StartsWithChar(String8 a, U8 b) {
  DEBUG_ASSERT(a.size >= 0);
  if (a.size == 0) { return false; }
  return a.str[0] == b;
}

B32 Str8EndsWith(String8 a, String8 b) {
  DEBUG_ASSERT(a.size >= 0 && b.size >= 0);
  if (a.size < b.size) { return false; }
  S32 a_offset = a.size - b.size;
  for (S32 i = 0; i < b.size; ++i) {
    if (a.str[a_offset + i] != b.str[i]) { return false; }
  }
  return true;
}

B32 Str8Eq(String8 a, String8 b) {
  DEBUG_ASSERT(a.size >= 0 && b.size >= 0);
  if (a.size != b.size) { return false; }
  return Str8StartsWith(a, b);
}

S32 Str8Find(String8 string, S32 start_pos, String8 needle) {
  DEBUG_ASSERT(string.size >= 0 && needle.size >= 0);
  if (start_pos < 0) { return -1; }
  if (string.size < start_pos + needle.size) { return -1; }
  for (S32 i = 0; i < string.size - start_pos; ++i) {
    S32 offset = start_pos + i;
    String8 substr = Str8(string.str + offset, string.size - offset);
    if (Str8StartsWith(substr, needle)) { return offset; }
  }
  return -1;
}

S32 Str8FindReverse(String8 string, S32 reverse_start_pos, String8 needle) {
  DEBUG_ASSERT(string.size >= 0 && needle.size >= 0);
  if (reverse_start_pos < 0) { return -1; }
  if (string.size < reverse_start_pos + needle.size) { return -1; }
  for (S32 i = string.size - reverse_start_pos - needle.size; i > 0; --i) {
    String8 substr = Str8(string.str, i + needle.size);
    if (Str8EndsWith(substr, needle)) { return i; }
  }
  return -1;
}

void Str8ToUpper(String8 s) {
  DEBUG_ASSERT(s.size >= 0);
  for (S32 i = 0; i < s.size; ++i) {
    s.str[i] = CharToUpper(s.str[i]);
  }
}

void Str8ToLower(String8 s) {
  DEBUG_ASSERT(s.size >= 0);
  for (S32 i = 0; i < s.size; ++i) {
    s.str[i] = CharToLower(s.str[i]);
  }
}

// TODO: support e notation
S32 Str8ToF32(String8 s, F32* f32) {
  DEBUG_ASSERT(s.size >= 0);
  S32 i = 0;
  F32 result = 0.0f;
  B32 found_f32 = false;

  S32 sign = 1;
  if (s.str[i] == '+')      { sign = +1; i++; }
  else if (s.str[i] == '-') { sign = -1; i++; }

  while (i < s.size && CharIsDigit(s.str[i])) {
    result *= 10;
    result += s.str[i] - '0';
    i++;
    found_f32 = true;
  }
  if (s.str[i] != '.') { goto str8_to_f32_end; }
  i++;

  S32 exp = 1;
  while (i < s.size && CharIsDigit(s.str[i])) {
    result *= 10;
    result += s.str[i] - '0';
    i++;
    exp *= 10;
    found_f32 = true;
  }
  result /= exp;
str8_to_f32_end:
  if (found_f32) {
    *f32 = sign * result;
    return i;
  } else {
    *f32 = 0;
    return -1;
  }
}

S32 Str8ToS32(String8 s, S32* s32) {
  DEBUG_ASSERT(s.size >= 0);
  S32 i = 0;
  S32 result = 0;
  B32 found_s32 = false;

  S32 sign = 1;
  if (s.str[i] == '+')      { sign = +1; i++; }
  else if (s.str[i] == '-') { sign = -1; i++; }

  while (i < s.size && CharIsDigit(s.str[i])) {
    result *= 10;
    result += s.str[i] - '0';
    i++;
    found_s32 = true;
  }
  if (found_s32) {
    *s32 = sign * result;
    return i;
  } else {
    *s32 = 0;
    return -1;
  }
}

String8 Str8Concat(Arena* arena, String8 a, String8 b) {
  DEBUG_ASSERT(a.size >= 0 && b.size >= 0);
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = a.size + b.size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, a.size + b.size);
  MEMORY_COPY(result.str, a.str, a.size);
  MEMORY_COPY(result.str + a.size, b.str, b.size);
  return result;
}

String8 Str8FormatV(Arena* arena, U8* fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  U32 size = vsnprintf(NULL, 0, (const char* const) fmt, args_copy) + 1;
  va_end(args_copy);
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = size - 1;
  result.str = ARENA_PUSH_ARRAY(arena, U8, size);
  va_copy(args_copy, args);
  vsnprintf((char* const) result.str, size, (char* const) fmt, args_copy);
  va_end(args_copy);
  ArenaPop(arena, 1); // NOTE: null terminator.
  return result;
}

String8 _Str8Format(Arena* arena, U8* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  String8 result = Str8FormatV(arena, fmt, args);
  va_end(args);
  return result;
}

void Str8ListPrepend(String8List* list, String8ListNode* node) {
  SLL_QUEUE_PUSH_FRONT(list->head, list->tail, node, next);
}

void Str8ListAppend(String8List* list, String8ListNode* node) {
  SLL_QUEUE_PUSH_BACK(list->head, list->tail, node, next);
}

String8 Str8ListJoin(Arena* arena, String8List* list) {
  S32 size = 0;
  for (String8ListNode* node = list->head; node != NULL; node = node->next) {
    size += node->string.size;
  }

  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, size);
  size = 0;
  for (String8ListNode* node = list->head; node != NULL; node = node->next) {
    MEMORY_COPY(result.str + size, node->string.str, node->string.size);
    size += node->string.size;
  }
  return result;
}

String8List Str8Split(Arena* arena, String8 string, U8 c) {
  DEBUG_ASSERT(string.size >= 0);
  String8List list;
  MEMORY_ZERO_STRUCT(&list);
  U8* substring_start = string.str;
  for (S32 i = 0; i < string.size; ++i) {
    if (string.str[i] == c) {
      String8ListNode* node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      node->string = Str8Range(substring_start, &string.str[i]);
      Str8ListAppend(&list, node);
      substring_start = &string.str[++i];
    }
  }
  if (substring_start < string.str + string.size) {
    String8ListNode* node = ARENA_PUSH_STRUCT(arena, String8ListNode);
    node->string = Str8Range(substring_start, string.str + string.size);
    Str8ListAppend(&list, node);
  }
  return list;
}

S32 Str8Hash(String8 s) {
  DEBUG_ASSERT(s.size >= 0);
  S32 hash = 0;
  for (S32 i = 0; i < s.size; i++) {
    hash += s.str[i];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Sort Implementation
///////////////////////////////////////////////////////////////////////////////

static inline void SortSwap(void* a, void* b, void* temp_buffer, U32 item_size) {
  MEMORY_COPY(temp_buffer, a, item_size);
  MEMORY_COPY(a, b, item_size);
  MEMORY_COPY(b, temp_buffer, item_size);
}

static inline U32 SortPartition(void* items, U32 item_size, S32 low_idx, S32 high_idx, SortCompare_Fn* compare_fn, void* temp_buffer) {
  void* pivot = ((U8*) items) + (item_size * high_idx);
  S32 i = low_idx;
  for (S32 j = low_idx; j < high_idx; j++) {
    void* at_j = ((U8*) items) + (item_size * j);
    S32 compare = compare_fn(at_j, pivot);
    if (compare <= 0) {
      void* at_i = ((U8*) items) + (item_size * i);
      SortSwap(at_i, at_j, temp_buffer, item_size);
      i++;
    }
  }
  void* at_i = ((U8*) items) + (item_size * i);
  SortSwap(at_i, pivot, temp_buffer, item_size);
  return i;
}

static void SortHelper(void* items, U32 item_size, S32 low_idx, S32 high_idx, SortCompare_Fn* compare_fn, void* temp_buffer) {
  if (low_idx >= high_idx) { return; }
  S32 pivot = SortPartition(items, item_size, low_idx, high_idx, compare_fn, temp_buffer);
  SortHelper(items, item_size, low_idx, pivot - 1, compare_fn, temp_buffer);
  SortHelper(items, item_size, pivot + 1, high_idx, compare_fn, temp_buffer);
}

void _Sort(void* items, U32 items_len, U32 item_size, SortCompare_Fn* compare_fn, void* temp_buffer) {
  SortHelper(items, item_size, 0, items_len - 1, compare_fn, temp_buffer);
}

S32 SortCompareS8Asc(void* a, void* b)  { return *(S8*)  a - *(S8*)  b; }
S32 SortCompareS16Asc(void* a, void* b) { return *(S16*) a - *(S16*) b; }
S32 SortCompareS32Asc(void* a, void* b) { return *(S32*) a - *(S32*) b; }
S32 SortCompareS64Asc(void* a, void* b) { return *(S64*) a - *(S64*) b; }
S32 SortCompareU8Asc(void* a, void* b)  { return *(U8*)  a - *(U8*)  b; }
S32 SortCompareU16Asc(void* a, void* b) { return *(U16*) a - *(U16*) b; }
S32 SortCompareU32Asc(void* a, void* b) { return *(U32*) a - *(U32*) b; }
S32 SortCompareU64Asc(void* a, void* b) { return *(U64*) a - *(U64*) b; }
S32 SortCompareF32Asc(void* a, void* b) { return *(F32*) a - *(F32*) b; }
S32 SortCompareF64Asc(void* a, void* b) { return *(F64*) a - *(F64*) b; }
S32 SortCompareB8Asc(void* a, void* b)  { return *(B8*)  a - *(B8*)  b; }
S32 SortCompareB16Asc(void* a, void* b) { return *(B16*) a - *(B16*) b; }
S32 SortCompareB32Asc(void* a, void* b) { return *(B32*) a - *(B32*) b; }
S32 SortCompareB64Asc(void* a, void* b) { return *(B64*) a - *(B64*) b; }
S32 SortCompareString8Asc(void* a, void* b) {
  String8* a_cast = (String8*) a;
  String8* b_cast = (String8*) b;
  S32 i = 0;
  while (true) {
    if (i >= a_cast->size) { return -1 * (a_cast->size != b_cast->size); }
    if (i >= b_cast->size) { return +1; } // NOTE: contextually, a size != b size.
    if (a_cast->str[i] < b_cast->str[i]) { return -1; }
    if (a_cast->str[i] > b_cast->str[i]) { return +1; }
    i++;
  }
  UNREACHABLE();
  return 0;
}

S32 SortCompareS8Desc(void* a, void* b)  { return *(S8*)  b - *(S8*)  a; }
S32 SortCompareS16Desc(void* a, void* b) { return *(S16*) b - *(S16*) a; }
S32 SortCompareS32Desc(void* a, void* b) { return *(S32*) b - *(S32*) a; }
S32 SortCompareS64Desc(void* a, void* b) { return *(S64*) b - *(S64*) a; }
S32 SortCompareU8Desc(void* a, void* b)  { return *(U8*)  b - *(U8*)  a; }
S32 SortCompareU16Desc(void* a, void* b) { return *(U16*) b - *(U16*) a; }
S32 SortCompareU32Desc(void* a, void* b) { return *(U32*) b - *(U32*) a; }
S32 SortCompareU64Desc(void* a, void* b) { return *(U64*) b - *(U64*) a; }
S32 SortCompareF32Desc(void* a, void* b) { return *(F32*) b - *(F32*) a; }
S32 SortCompareF64Desc(void* a, void* b) { return *(F64*) b - *(F64*) a; }
S32 SortCompareB8Desc(void* a, void* b)  { return *(B8*)  b - *(B8*)  a; }
S32 SortCompareB16Desc(void* a, void* b) { return *(B16*) b - *(B16*) a; }
S32 SortCompareB32Desc(void* a, void* b) { return *(B32*) b - *(B32*) a; }
S32 SortCompareB64Desc(void* a, void* b) { return *(B64*) b - *(B64*) a; }
S32 SortCompareString8Desc(void* a, void* b) {
  String8* a_cast = (String8*) a;
  String8* b_cast = (String8*) b;
  S32 i = 0;
  while (true) {
    if (i >= a_cast->size) { return +1 * (a_cast->size != b_cast->size); }
    if (i >= b_cast->size) { return -1; } // NOTE: contextually, a size != b size.
    if (a_cast->str[i] < b_cast->str[i]) { return +1; }
    if (a_cast->str[i] > b_cast->str[i]) { return -1; }
    i++;
  }
  UNREACHABLE();
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Bin read / write Implementation
///////////////////////////////////////////////////////////////////////////////

U8 BinRead8(U8* bytes) {
  return *bytes;
}

U16 BinRead16LE(U8* bytes) {
  U8 x = BinRead8(bytes);
  U8 y = BinRead8(bytes + 1);
  return (((U16) x) + (((U16) y) << 8));
}

U16 BinRead16BE(U8* bytes) {
  U8 x = BinRead8(bytes);
  U8 y = BinRead8(bytes + 1);
  return ((((U16) x) << 8) + ((U16) y));
}

U32 BinRead32LE(U8* bytes) {
  U16 x = BinRead16LE(bytes);
  U16 y = BinRead16LE(bytes + 2);
  return (((U32) x) + (((U32) y) << 16));
}

U32 BinRead32BE(U8* bytes) {
  U16 x = BinRead16BE(bytes);
  U16 y = BinRead16BE(bytes + 2);
  return ((((U32) x) << 16) + ((U32) y));
}

U64 BinRead64LE(U8* bytes) {
  U32 x = BinRead32LE(bytes);
  U32 y = BinRead32LE(bytes + 4);
  return (((U64) x) + (((U64) y) << 32));
}

U64 BinRead64BE(U8* bytes) {
  U32 x = BinRead32BE(bytes);
  U32 y = BinRead32BE(bytes + 4);
  return ((((U64) x) << 32) + ((U64) y));
}

void BinStreamInit(BinStream* stream, U8* bytes, U32 bytes_size) {
  stream->bytes = bytes;
  stream->bytes_size = bytes_size;
  stream->pos = 0;
}

void BinStreamSeek(BinStream* stream, U32 pos) {
  DEBUG_ASSERT(pos <= stream->bytes_size);
  stream->pos = pos;
}

void BinStreamSkip(BinStream* stream, U32 num, S32 size) {
  stream->pos += num * size;
}

U8* BinStreamDecay(BinStream* stream) {
  return &stream->bytes[stream->pos];
}

U8 BinStreamPull8(BinStream* stream) {
  DEBUG_ASSERT(stream->pos + 1 <= stream->bytes_size);
  U8 result = BinRead8(stream->bytes + stream->pos);
  stream->pos += 1;
  return result;
}

U16 BinStreamPull16LE(BinStream* stream) {
  DEBUG_ASSERT(stream->pos + 2 <= stream->bytes_size);
  U16 result = BinRead16LE(stream->bytes + stream->pos);
  stream->pos += 2;
  return result;
}

U16 BinStreamPull16BE(BinStream* stream) {
  DEBUG_ASSERT(stream->pos + 2 <= stream->bytes_size);
  U16 result = BinRead16BE(stream->bytes + stream->pos);
  stream->pos += 2;
  return result;
}

U32 BinStreamPull32LE(BinStream* stream) {
  DEBUG_ASSERT(stream->pos + 4 <= stream->bytes_size);
  U32 result = BinRead32LE(stream->bytes + stream->pos);
  stream->pos += 4;
  return result;
}

U32 BinStreamPull32BE(BinStream* stream) {
  DEBUG_ASSERT(stream->pos + 4 <= stream->bytes_size);
  U32 result = BinRead32BE(stream->bytes + stream->pos);
  stream->pos += 4;
  return result;
}

U64 BinStreamPull64LE(BinStream* stream) {
  DEBUG_ASSERT(stream->pos + 8 <= stream->bytes_size);
  U64 result = BinRead64LE(stream->bytes + stream->pos);
  stream->pos += 8;
  return result;
}

U64 BinStreamPull64BE(BinStream* stream) {
  DEBUG_ASSERT(stream->pos + 8 <= stream->bytes_size);
  U64 result = BinRead64BE(stream->bytes + stream->pos);
  stream->pos += 8;
  return result;
}

void BinStreamPush8(BinStream* stream, U8 x) {
  DEBUG_ASSERT(stream->pos < stream->bytes_size);
  stream->bytes[stream->pos++] = x;
}

void BinStreamPush16LE(BinStream* stream, U16 x) {
  U8 a = x & 0xff;
  U8 b = (x >> 8) & 0xff;
  BinStreamPush8(stream, a);
  BinStreamPush8(stream, b);
}

void BinStreamPush16BE(BinStream* stream, U16 x) {
  U8 a = (x >> 8) & 0xff;
  U8 b = x & 0xff;
  BinStreamPush8(stream, a);
  BinStreamPush8(stream, b);
}

void BinStreamPush32LE(BinStream* stream, U32 x) {
  U16 a = x & 0xffff;
  U16 b = (x >> 16) & 0xffff;
  BinStreamPush16LE(stream, a);
  BinStreamPush16LE(stream, b);
}

void BinStreamPush32BE(BinStream* stream, U32 x) {
  U16 a = (x >> 16) & 0xffff;
  U16 b = x & 0xffff;
  BinStreamPush16BE(stream, a);
  BinStreamPush16BE(stream, b);
}

void BinStreamPush64LE(BinStream* stream, U64 x) {
  U32 a = x & 0xffffffff;
  U32 b = (x >> 32) & 0xffffffff;
  BinStreamPush32LE(stream, a);
  BinStreamPush32LE(stream, b);
}

void BinStreamPush64BE(BinStream* stream, U64 x) {
  U32 a = (x >> 32) & 0xffffffff;
  U32 b = x & 0xffffffff;
  BinStreamPush32BE(stream, a);
  BinStreamPush32BE(stream, b);
}

U8 BinStreamPeek8(BinStream* stream, S32 offset, U32 size) {
  offset = offset * size;
  if (offset < 0) { DEBUG_ASSERT(stream->pos >= (-1 * offset)); }
  DEBUG_ASSERT(stream->pos + offset + 1 <= stream->bytes_size);
  return BinRead8(stream->bytes + stream->pos + offset);
}

U16 BinStreamPeek16LE(BinStream* stream, S32 offset, U32 size) {
  offset = offset * size;
  if (offset < 0) { DEBUG_ASSERT(stream->pos >= (-1 * offset)); }
  DEBUG_ASSERT(stream->pos + offset + 2 <= stream->bytes_size);
  return BinRead16LE(stream->bytes + stream->pos + offset);
}

U16 BinStreamPeek16BE(BinStream* stream, S32 offset, U32 size) {
  offset = offset * size;
  if (offset < 0) { DEBUG_ASSERT(stream->pos >= (-1 * offset)); }
  DEBUG_ASSERT(stream->pos + offset + 2 <= stream->bytes_size);
  return BinRead16BE(stream->bytes + stream->pos + offset);
}

U32 BinStreamPeek32LE(BinStream* stream, S32 offset, U32 size) {
  offset = offset * size;
  if (offset < 0) { DEBUG_ASSERT(stream->pos >= (-1 * offset)); }
  DEBUG_ASSERT(stream->pos + offset + 4 <= stream->bytes_size);
  return BinRead32LE(stream->bytes + stream->pos + offset);
}

U32 BinStreamPeek32BE(BinStream* stream, S32 offset, U32 size) {
  offset = offset * size;
  if (offset < 0) { DEBUG_ASSERT(stream->pos >= (-1 * offset)); }
  DEBUG_ASSERT(stream->pos + offset + 4 <= stream->bytes_size);
  return BinRead32BE(stream->bytes + stream->pos + offset);
}

U64 BinStreamPeek64LE(BinStream* stream, S32 offset, U32 size) {
  offset = offset * size;
  if (offset < 0) { DEBUG_ASSERT(stream->pos >= (-1 * offset)); }
  DEBUG_ASSERT(stream->pos + offset + 8 <= stream->bytes_size);
  return BinRead64LE(stream->bytes + stream->pos + offset);
}

U64 BinStreamPeek64BE(BinStream* stream, S32 offset, U32 size) {
  offset = offset * size;
  if (offset < 0) { DEBUG_ASSERT(stream->pos >= (-1 * offset)); }
  DEBUG_ASSERT(stream->pos + offset + 8 <= stream->bytes_size);
  return BinRead64BE(stream->bytes + stream->pos + offset);
}

#endif // CDEFAULT_STD_IMPLEMENTATION
