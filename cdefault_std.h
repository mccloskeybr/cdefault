#ifndef CDEFAULT_STD_H_
#define CDEFAULT_STD_H_

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <stdatomic.h>

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

#if defined(_WIN32)
#  define OS_WINDOWS 1
#elif defined(_gnu_linux_) || defined(_linux_)
#  define OS_LINUX 1
#elif defined(_APPLE_) && defined(_MACH_)
#  define OS_MAC 1
#else
#  error Unknown operating system.
#endif

#if defined(_GNUC_) || defined(_GNUG_)
#  define COMPILER_GCC 1
#elif defined(_clang_)
#  define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#  define COMPILER_MSVC 1
#else
#  error Unknown compiler.
#endif

#define GLUE(a, b) a ## b
#define FILENAME (strrchr(__FILE__, '/')  ? strrchr(__FILE__, '/')  + 1 : \
                 (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : \
                 __FILE__))

#define STATIC_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MIN3(a, b, c) (MIN(MIN((a), (b)), (c)))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define MAX3(a, b, c) (MAX(MAX((a), (b)), (c)))
#define CLAMP(a, x, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

#define BIT(idx) (1 << (idx))
#define EXTRACT_BIT(word, idx) (((word) >> (idx)) & 1)
#define EXTRACT_U8(word, pos) (((word) >> ((pos) * 8)) & 0xff)
#define EXTRACT_U16(word, pos) (((word) >> ((pos) * 16)) & 0xffff)
#define EXTRACT_U32(word, pos) (((word) >> ((pos) * 32)) & 0xffffffff)

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
// NOTE: Branch prediction
///////////////////////////////////////////////////////////////////////////////

#if defined(_GNUC_) || defined(_clang_)
#  define BRANCH_EXPECT(expr, val) _builtin_expect((expr), (val))
#else
#  define BRANCH_EXPECT(expr, val) (expr)
#endif
#define LIKELY(expr) BRANCH_EXPECT(expr, true)
#define UNLIKELY(expr) BRANCH_EXPECT(expr, false)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Assertions
///////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#  define DEBUG_ASSERT(exp) assert(UNLIKELY(exp))
#  define ASSERT(exp) DEBUG_ASSERT(UNLIKELY(exp))
#else
#  define DEBUG_ASSERT(exp) (exp)
#  define ASSERT(exp) if (UNLIKELY(!(exp))) { *(int*)0 = 0; }
#endif
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#  define TRAP() _debugbreak()
#elif defined(COMPILER_MSVC)
#  define TRAP() _builtin_trap()
#else
#  error Unknown trap intrinsic for this compiler.
#endif
#define STATIC_ASSERT(exp, msg) static_assert((exp), msg)
#define UNIMPLEMENTED() ASSERT(false)
#define UNREACHABLE() ASSERT(false)
#define TODO() ASSERT(false)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Alignment and offsets
///////////////////////////////////////////////////////////////////////////////

#if defined(COMPILER_MSVC) || defined(COMPILER_CLANG)
#  define ALIGN_OF(T) _alignof(T)
#elif defined(COMPILER_GCC)
#  define ALIGN_OF(T) _alignof_(T)
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

#define MEMORY_ZERO(dest, size)     MEMORY_SET(dest, 0, size)
#define MEMORY_ZERO_STRUCT(s)       MEMORY_ZERO((s), sizeof(*(s)))
#define MEMORY_ZERO_STATIC_ARRAY(a) MEMORY_ZERO((a), sizeof(a))

#define IS_MEMORY_EQUAL(a, b, size)       (MEMORY_COMPARE((a), (b), (size)) == 0)
#define IS_MEMORY_EQUAL_STRUCT(a, b)      IS_MEMORY_EQUAL((a), (b), sizeof(*(a)))
#define IS_MEMORY_EQUAL_STATIC_ARRAY(a,b) IS_MEMORY_EQUAL((a), (b), sizeof(a))

///////////////////////////////////////////////////////////////////////////////
// NOTE: List macros
///////////////////////////////////////////////////////////////////////////////

// NOTE: Singly-linked stack
// E.g.
// struct Node { Node* next; }
// struct Stack { Node* front; }
//
// Stack stack = {0};
// Node node = {0};
// SLL_STACK_PUSH(stack.front, &node, next);
// SLL_STACK_POP(stack.front, next);

#define SLL_STACK_PUSH(front, curr_node, next) \
  do {                                         \
    (curr_node)->next = (front);               \
    (front) = (curr_node);                     \
  } while (0)
#define SLL_STACK_POP(front, next) \
  (front) = (front)->next

// NOTE: Singly-linked queue
// E.g.
// struct Node { Node* next; }
// struct Queue { Node* front, back; }
//
// Queue queue = {0};
// Node node = {0};
// SLL_QUEUE_PUSH(queue.front, queue.back, &node, next);
// SLL_QUEUE_POP(queue.front, queue.back next);

#define SLL_QUEUE_PUSH(front, back, curr_node, next) \
  do {                                               \
    if ((front) == NULL) {                           \
      (front) = (curr_node);                         \
      (back) = (curr_node);                          \
      (curr_node)->next = NULL;                      \
    }                                                \
    else {                                           \
      (back)->next = (curr_node);                    \
      (back) = (curr_node);                          \
      (curr_node)->next = NULL;                      \
    }                                                \
  } while (0)
#define SLL_QUEUE_PUSH_FRONT(front, back, curr_node, next) \
  do {                                                     \
    if ((front) == NULL) {                                 \
      (front) = (curr_node);                               \
      (back) = (curr_node);                                \
      (curr_node)->next = NULL;                            \
    }                                                      \
    else {                                                 \
      (curr_node)->next = (front);                         \
      (front) = (curr_node);                               \
    }                                                      \
  } while (0)
#define SLL_QUEUE_POP(front, back, next) \
  do {                                   \
    if ((front) == (back)) {             \
      (front) = NULL;                    \
      (back) = NULL;                     \
    }                                    \
    else {                               \
      (front) = (front)->next;           \
    }                                    \
  } while (0)

// NOTE: Doubly-linked list
// E.g.
// struct Node { Node* prev, next; }
// struct NodeManager { Node* front, back; }
//
// NodeManager man = {0};
// NodeManager node = {0};
// DLL_PUSH_BACK(man.front, man.back, &node, prev, next);
// DLL_POP_BACK(man.front, man.back, prev, next);

// NOTE: front, back, prev_node, curr_node are node pointers.
// prev, next are the member field names.
#define DLL_INSERT(front, back, prev_node, curr_node, prev, next) \
  do {                                                            \
    if ((front) == NULL) {                                        \
      (front) = (curr_node);                                      \
      (back) = (curr_node);                                       \
      (curr_node)->next = NULL;                                   \
      (curr_node)->prev = NULL;                                   \
    }                                                             \
    else if ((prev_node) == NULL) {                               \
      (curr_node)->next = (front);                                \
      (front)->prev = (curr_node);                                \
      (front) = (curr_node);                                      \
      (curr_node)->prev = NULL;                                   \
    }                                                             \
    else if ((prev_node) == (back)) {                             \
      (back)->next = (curr_node);                                 \
      (curr_node)->prev = (back);                                 \
      (back) = (curr_node);                                       \
      (curr_node)->next = NULL;                                   \
    }                                                             \
    else {                                                        \
      (prev_node)->next->prev = (curr_node);                      \
      (curr_node)->next = (prev_node)->next;                      \
      (prev_node)->next = (curr_node);                            \
      (curr_node)->prev = (prev_node);                            \
    }                                                             \
  } while (0)
#define DLL_REMOVE(front, back, curr_node, prev, next) \
  do {                                                 \
    if ((curr_node) == (front)) {                      \
      (front) = (front)->next;                         \
    }                                                  \
    if ((curr_node) == (back)) {                       \
      (back) = (back)->prev;                           \
    }                                                  \
    if ((curr_node)->prev != NULL) {                   \
      (curr_node)->prev->next = (curr_node)->next;     \
    }                                                  \
    if ((curr_node)->next != NULL) {                   \
      (curr_node)->next->prev = (curr_node)->prev;     \
    }                                                  \
  } while (0)
#define DLL_PUSH_FRONT(front, back, curr_node, prev, next) \
  DLL_INSERT(front, back, (typeof(curr_node)) NULL, curr_node, prev, next)
#define DLL_PUSH_BACK(front, back, curr_node, prev, next) \
  DLL_INSERT(front, back, back, curr_node, prev, next)
#define DLL_POP_FRONT(front, back, prev, next)                  \
  do {                                                          \
    typeof(front) _cdefault_front_copy = front;                \
    DLL_REMOVE(front, back, _cdefault_front_copy, prev, next); \
  } while (0)
#define DLL_POP_BACK(front, back, prev, next)                   \
  do {                                                          \
    typeof(front) _cdefault_back_copy = back;                  \
    DLL_REMOVE(front, back, _cdefault_back_copy, prev, next);  \
  } while (0)

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
#  define LOG_DEBUG(fmt, ...) Log("DEBUG", FILENAME, fmt, _VA_ARGS_)
#else
#  define LOG_DEBUG(fmt, ...)
#endif

///////////////////////////////////////////////////////////////////////////////
// NOTE: Test
///////////////////////////////////////////////////////////////////////////////

// NOTE: Test functions should return a B32, true on success.
// NOTE: Wrap test functions in TEST() from main, expectations in TEST_EXPECT().

#define TEST(func) \
  if ((func)) { LOG_INFO ("\t[" ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET "]: "#func); } \
  else      { LOG_ERROR("\t[" ANSI_COLOR_RED   "FAILURE" ANSI_COLOR_RESET "]: "#func); }
#define TEST_EXPECT(expr) \
  if (!(expr)) { LOG_ERROR("\t" ANSI_COLOR_RED "Expectation failed: " ANSI_COLOR_RESET #expr); return false; }

///////////////////////////////////////////////////////////////////////////////
// NOTE: Arena
///////////////////////////////////////////////////////////////////////////////

typedef struct ArenaConfig ArenaConfig;
struct ArenaConfig {
  U64 capacity; // NOTE: How much space is allocated (bytes).
};

typedef struct Arena Arena;
struct Arena {
  Arena* current;
  U64 capacity;
  U64 base_pos;
  U64 pos;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
  Arena* arena;
  U64 pos;
};

void ArenaConfigure(ArenaConfig config);
Arena* ArenaAllocate(void);
void ArenaRelease(Arena* arena);
void* ArenaPush(Arena* arena, U64 size, U64 align);
void ArenaPopTo(Arena* arena, U64 pos);
void ArenaPop(Arena* arena, U64 size);
void ArenaClear(Arena* arena);

ArenaTemp ArenaTempBegin(Arena* arena);
void ArenaTempEnd(ArenaTemp* temp);

#define ARENA_PUSH_ARRAY(arena, type, count) (type*) ArenaPush(arena, sizeof(type) * count, MAX(8, ALIGN_OF(type)))
#define ARENA_PUSH_STRUCT(arena, type) ARENA_PUSH_ARRAY(arena, type, 1)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Thread
///////////////////////////////////////////////////////////////////////////////

typedef struct Thread Thread;
struct Thread { thrd_t thread; };
typedef S32 ThreadStartFunc(void*);

void ThreadCreate(Thread* thread, ThreadStartFunc* entry, void* arg);
void ThreadDetatch(Thread* thread);
S32 ThreadJoin(Thread* thread);
B8 ThreadEqual(Thread* a, Thread* b);

typedef struct Mutex Mutex;
struct Mutex { mtx_t mutex; };

void MutexInit(Mutex* mutex);
void MutexDeinit(Mutex* mutex);
void MutexLock(Mutex* mutex);
void MutexUnlock(Mutex* mutex);

typedef struct CV CV;
struct CV { cnd_t cv; };

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

///////////////////////////////////////////////////////////////////////////////
// NOTE: Atomic
///////////////////////////////////////////////////////////////////////////////

typedef _Atomic(U32) AtomicU32;
void AtomicU32Init(AtomicU32* a, U32 desired);
void AtomicU32Store(AtomicU32* a, U32 desired);
U32 AtomicU32Load(AtomicU32* a);
U32 AtomicU32Exchange(AtomicU32* a, U32 desired);
B8 AtomicU32CompareExchange(AtomicU32* a, U32* expected, U32 desired);
U32 AtomicU32FetchAdd(AtomicU32* a, U32 b);
U32 AtomicU32FetchSub(AtomicU32* a, U32 b);
U32 AtomicU32FetchOr(AtomicU32* a, U32 b);
U32 AtomicU32FetchXor(AtomicU32* a, U32 b);
U32 AtomicU32FetchAnd(AtomicU32* a, U32 b);

///////////////////////////////////////////////////////////////////////////////
// NOTE: String
///////////////////////////////////////////////////////////////////////////////

typedef struct String8 String8;
struct String8 {
  U8* str;
  U64 size;
};

typedef struct String8ListNode String8ListNode;
struct String8ListNode {
  String8ListNode* next;
  String8 string;
};

typedef struct String8List String8List;
struct String8List {
  String8ListNode* front;
  String8ListNode* back;
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
String8 String8ToUpper(Arena* arena, String8* s);
String8 String8ToLower(Arena* arena, String8* s);

B32 String8StartsWith(String8* a, String8* b); // NOTE: True iff a starts with b.
B32 String8EndsWith(String8* a, String8* b); // NOTE: True iff a ens with b.
B32 String8Equals(String8* a, String8* b);
S64 String8Find(String8* string, U64 start_pos, String8* needle); // NOTE: Returns -1 on failure.
S64 String8FindReverse(String8* string, U64 reverse_start_pos, String8* needle); // NOTE: Returns -1 on failure.

String8 String8Concat(Arena* arena, String8* a, String8* b);
String8 String8FormatV(Arena* arena, U8* fmt, va_list args);
String8 String8Format(Arena* arena, U8* fmt, ...);

void String8ListPrepend(String8List* list, String8ListNode* node);
void String8ListAppend(String8List* list, String8ListNode* node);
String8 String8ListJoin(Arena* arena, String8List* list);
String8List String8Split(Arena* arena, String8* string, U8 c);

S32 String8Hash(String8* s);

#endif // CDEFAULT_H_

///////////////////////////////////////////////////////////////////////////////
// NOTE: Implementation
///////////////////////////////////////////////////////////////////////////////

#ifdef CDEFAULT_STD_IMPLEMENTATION
#undef CDEFAULT_STD_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Log implementation
///////////////////////////////////////////////////////////////////////////////

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

static void LogV(char* level, char* filename, U32 loc, char* fmt, va_list args) {
  if (UNLIKELY(!_cdef_log_config.is_initialized)) { LogInit(stderr); }
  MutexLock(&_cdef_log_config.mtx);
  fprintf(_cdef_log_config.fd, "[%s | %s:%d]: ", level, filename, loc);
  vfprintf(_cdef_log_config.fd, fmt, args);
  fprintf(_cdef_log_config.fd, "\n");
  MutexUnlock(&_cdef_log_config.mtx);
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

static ArenaConfig _cdef_arena_config = {
  .capacity = KB(4),
};
void ArenaConfigure(ArenaConfig config) {
  _cdef_arena_config = config;
}

Arena* ArenaAllocate(void) {
  Arena* arena = (Arena*) malloc(_cdef_arena_config.capacity);
  DEBUG_ASSERT(arena != NULL);
  MEMORY_ZERO_STRUCT(arena);
  arena->current = arena;
  arena->capacity = _cdef_arena_config.capacity;
  return arena;
}

void ArenaRelease(Arena* arena) {
  if (UNLIKELY(arena == NULL)) { return; }
  ArenaClear(arena);
  free(arena);
}

void* ArenaPush(Arena* arena, U64 size, U64 align) {
  U64 pos_aligned = ALIGN_POW_2(arena->current->pos, align);
  if (pos_aligned - arena->current->base_pos + size >
      arena->current->capacity - sizeof(Arena)) {
    Arena* new_arena = NULL;
    if (_cdef_arena_config.capacity < sizeof(Arena) + size) {
      LOG_WARN(
          "Allocating custom arena for extra large size: "
          "%d (normal capacity is: %d).",
          size, _cdef_arena_config.capacity - sizeof(Arena));
      U64 capacity_hold = _cdef_arena_config.capacity;
      _cdef_arena_config.capacity = sizeof(Arena) + size;
      new_arena = ArenaAllocate();
      _cdef_arena_config.capacity = capacity_hold;
    } else {
      new_arena = ArenaAllocate();
    }
    new_arena->base_pos = pos_aligned;
    new_arena->current = arena->current;
    arena->current = new_arena;
  }
  arena->current->pos = pos_aligned;
  DEBUG_ASSERT((arena->current->pos - arena->current->base_pos +
      sizeof(Arena) + size) <= arena->current->capacity);
  void* chunk = ((U8*) arena) + sizeof(Arena) +
    (arena->current->pos - arena->current->base_pos);
  arena->current->pos += size;

  return chunk;
}

void ArenaPopTo(Arena* arena, U64 pos) {
  Arena* current = arena->current;
  while (pos < current->base_pos) {
    Arena* next = current->current;
    free(current);
    DEBUG_ASSERT(current != next);
    current = next;
  }
  current->pos = pos;
  arena->current = current;
}

void ArenaPop(Arena* arena, U64 size) {
  ArenaPopTo(arena, arena->current->pos - size);
}

void ArenaClear(Arena* arena) {
  ArenaPopTo(arena, 0);
}

ArenaTemp ArenaTempBegin(Arena* arena) {
  ArenaTemp temp;
  MEMORY_ZERO_STRUCT(&temp);
  temp.arena = arena;
  temp.pos = arena->pos;
  return temp;
}

void ArenaTempEnd(ArenaTemp* temp) {
  ArenaPopTo(temp->arena, temp->pos);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Thread Implementation
///////////////////////////////////////////////////////////////////////////////

void ThreadCreate(Thread* thread, ThreadStartFunc* entry, void* arg) {
  MEMORY_ZERO_STRUCT(thread);
  S32 result = thrd_create(&thread->thread, entry, arg);
  ASSERT(result == thrd_success);
}

void ThreadDetatch(Thread* thread) {
  S32 result = thrd_detach(thread->thread);
  ASSERT(result == thrd_success);
}

S32 ThreadJoin(Thread* thread) {
  S32 out;
  S32 result = thrd_join(thread->thread, &out);
  ASSERT(result == thrd_success);
  return out;
}

B8 ThreadEqual(Thread* a, Thread* b) {
  B8 result = thrd_equal(a->thread, b->thread) != 0;
  return result;
}

void MutexInit(Mutex* mutex) {
  MEMORY_ZERO_STRUCT(mutex);
  S32 result = mtx_init(&mutex->mutex, mtx_plain);
  ASSERT(result == thrd_success);
}

void MutexDeinit(Mutex* mutex) {
  mtx_destroy(&mutex->mutex);
}

void MutexLock(Mutex* mutex) {
  S32 result = mtx_lock(&mutex->mutex);
  ASSERT(result == thrd_success);
}

void MutexUnlock(Mutex* mutex) {
  S32 result = mtx_unlock(&mutex->mutex);
  ASSERT(result == thrd_success);
}

void CVInit(CV* cv) {
  MEMORY_ZERO_STRUCT(cv);
  S32 result = cnd_init(&cv->cv);
  ASSERT(result == thrd_success);
}

void CVDeinit(CV* cv) {
  cnd_destroy(&cv->cv);
}

void CVSignal(CV* cv) {
  S32 result = cnd_signal(&cv->cv);
  ASSERT(result == thrd_success);
}

void CVBroadcast(CV* cv) {
  S32 result = cnd_broadcast(&cv->cv);
  ASSERT(result == thrd_success);
}

void CVWait(CV* cv, Mutex* mutex) {
  S32 result = cnd_wait(&cv->cv, &mutex->mutex);
  ASSERT(result == thrd_success);
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

///////////////////////////////////////////////////////////////////////////////
// NOTE: Atomic Implementation
///////////////////////////////////////////////////////////////////////////////

void AtomicU32Init(AtomicU32* a, U32 desired) { atomic_init(a, desired); }
void AtomicU32Store(AtomicU32* a, U32 desired) { atomic_store(a, desired); }
U32 AtomicU32Load(AtomicU32* a) { return atomic_load(a); }
U32 AtomicU32Exchange(AtomicU32* a, U32 desired) { return atomic_exchange(a, desired); }
B8 AtomicU32CompareExchange(AtomicU32* a, U32* expected, U32 desired) {
  return atomic_compare_exchange_strong(a, expected, desired); }
U32 AtomicU32FetchAdd(AtomicU32* a, U32 b) { return atomic_fetch_add(a, b); }
U32 AtomicU32FetchSub(AtomicU32* a, U32 b) { return atomic_fetch_sub(a, b); }
U32 AtomicU32FetchOr(AtomicU32* a, U32 b) { return atomic_fetch_or(a, b); }
U32 AtomicU32FetchXor(AtomicU32* a, U32 b) { return atomic_fetch_xor(a, b); }
U32 AtomicU32FetchAnd(AtomicU32* a, U32 b) { return atomic_fetch_and(a, b); }

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
  if (CharIsUpper(c)) { c += (U8)('a' - 'A'); }
  return c;
}

U8 CharToUpper(U8 c) {
  if (CharIsLower(c)) { c += (U8)('A' - 'a'); }
  return c;
}

String8 String8Create(U8* str, U64 size) {
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
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
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.str = str;
  result.size = (U64) (one_past_last - str);
  return result;
}

String8 String8Copy(Arena* arena, String8* string) {
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = string->size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, string->size);
  MEMORY_COPY(result.str, string->str, string->size);
  return result;
}

String8 String8Substring(String8* s, U64 start, U64 end) {
  DEBUG_ASSERT(end > start);
  start = MIN(start, s->size - 1);
  end = MIN(end, s->size - 1);
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
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
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = a->size + b->size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, a->size + b->size);
  MEMORY_COPY(result.str, a->str, a->size);
  MEMORY_COPY(result.str + a->size, b->str, b->size);
  return result;
}

String8 String8FormatV(Arena* arena, U8* fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  U32 size = vsnprintf(NULL, 0, (const char* const) fmt, args_copy) + 1;
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = size - 1;
  result.str = ARENA_PUSH_ARRAY(arena, U8, size);
  vsnprintf((char* const) result.str, size, (char* const) fmt, args_copy);
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

void String8ListPrepend(String8List* list, String8ListNode* node) {
  SLL_QUEUE_PUSH_FRONT(list->front, list->back, node, next);
}

void String8ListAppend(String8List* list, String8ListNode* node) {
  SLL_QUEUE_PUSH(list->front, list->back, node, next);
}

String8 String8ListJoin(Arena* arena, String8List* list) {
  U64 size = 0;
  for (String8ListNode* node = list->front; node != NULL; node = node->next) {
    size += node->string.size;
  }
  String8 result;
  MEMORY_ZERO_STRUCT(&result);
  result.size = size;
  result.str = ARENA_PUSH_ARRAY(arena, U8, size);
  size = 0;
  for (String8ListNode* node = list->front; node != NULL; node = node->next) {
    MEMORY_COPY(result.str + size, node->string.str, node->string.size);
    size += node->string.size;
  }
  return result;
}

String8List String8Split(Arena* arena, String8* string, U8 c) {
  String8List list;
  MEMORY_ZERO_STRUCT(&list);
  U8* substring_start = string->str;
  for (U64 i = 0; i < string->size; ++i) {
    if (string->str[i] == c) {
      String8ListNode* node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      node->string = String8CreateRange(substring_start, &string->str[i]);
      String8ListAppend(&list, node);
      substring_start = &string->str[++i];
    }
  }
  if (substring_start < string->str + string->size) {
    String8ListNode* node = ARENA_PUSH_STRUCT(arena, String8ListNode);
    node->string = String8CreateRange(substring_start, string->str + string->size);
    String8ListAppend(&list, node);
  }
  return list;
}

S32 String8Hash(String8* s) {
  S32 hash = 0;
  for (S32 i = 0; i < s->size; i++) {
    hash += s->str[i];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

#endif // CDEFAULT_STD_IMPLEMENTATION
