#ifndef CDEFAULT_STD_H_
#define CDEFAULT_STD_H_

#if defined(_WIN32)
#  define OS_WINDOWS 1
#elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
#else
#  error Unknown operating system.
#endif

#if defined(__GNUC__) || defined(__GNUG__)
#  define COMPILER_GCC 1
#elif defined(__clang__)
#  define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#  define COMPILER_MSVC 1
#else
#  error Unknown compiler.
#endif

#if defined(OS_WINDOWS)

#include <windows.h>

#elif defined(OS_LINUX) || defined(OS_MAC)

#include <sys/mman.h>
#include <threads.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

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

///////////////////////////////////////////////////////////////////////////////
// NOTE: Gen purpose macros
///////////////////////////////////////////////////////////////////////////////

#if defined(COMPILER_MSVC)
#  define UNUSED(x) x [[maybe_unused]]
#elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#  define UNUSED(x) x __attribute__((unused))
#else
#  define UNUSED(x) x
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
#define CLAMP_TOP(a, x) MIN((a), (x))
#define CLAMP_BOT(x, b) MAX((x), (b))

#define BIT(idx) (1 << (idx))
#define EXTRACT_BIT(word, idx) (((word) >> (idx)) & 1)
#define EXTRACT_U8(word, pos)  (((word) >> ((pos) * 8)) & 0xff)
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
#  define ASSERT(exp) DEBUG_ASSERT(LIKELY(exp))
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
#define UNREACHABLE() ASSERT(false)
#define TODO() ASSERT(false)

///////////////////////////////////////////////////////////////////////////////
// NOTE: Alignment and offsets
///////////////////////////////////////////////////////////////////////////////

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

void* MemoryReserve(U64 size);
B32 MemoryCommit(void* ptr, U64 size);
void MemoryRelease(void* ptr, U64 size);
void MemoryDecommit(void* ptr, U64 size);

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
#  define LOG_DEBUG(fmt, ...) Log("DEBUG", FILENAME, fmt, ##__VA_ARGS__)
#else
#  define LOG_DEBUG(fmt, ...)
#endif

///////////////////////////////////////////////////////////////////////////////
// NOTE: Test
///////////////////////////////////////////////////////////////////////////////

// NOTE: Test functions should return a B32, true on success.
// NOTE: Wrap test functions in TEST() from main, expectations in TEST_EXPECT().

#define TEST(func) \
  if ((func))  { LOG_INFO ("\t[" ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET "]: "#func); } \
  else         { LOG_ERROR("\t[" ANSI_COLOR_RED   "FAILURE" ANSI_COLOR_RESET "]: "#func); }
#define TEST_EXPECT(expr) \
  if (!(expr)) { LOG_ERROR("\t" ANSI_COLOR_RED "Expectation failed: " ANSI_COLOR_RESET #expr); return false; }

///////////////////////////////////////////////////////////////////////////////
// NOTE: Arena
///////////////////////////////////////////////////////////////////////////////

#define PAGE_SIZE KB(4)
#define CDEFAULT_ARENA_RESERVE_SIZE ALIGN_POW_2(MB(64), PAGE_SIZE)
#define CDEFAULT_ARENA_COMMIT_SIZE  ALIGN_POW_2(KB(64), PAGE_SIZE)

typedef struct Arena Arena;
struct Arena {
  U64 reserve_size;
  U64 commit_size;
  U64 commit;
  U64 pos;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
  Arena* arena;
  U64 pos;
};

Arena* _ArenaAllocate(U64 reserve_size, U64 commit_size);
#define ArenaAllocate() _ArenaAllocate(CDEFAULT_ARENA_RESERVE_SIZE, CDEFAULT_ARENA_COMMIT_SIZE)
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

#if defined(OS_WINDOWS)
typedef HANDLE Thread;
typedef CRITICAL_SECTION Mutex;
typedef CONDITION_VARIABLE CV;
#else
typedef thrd_t Thread;
typedef mtx_t Mutex;
typedef cnd_t CV;
#endif
typedef S32 ThreadStartFunc(void*);
typedef S32 LockWitness;

void ThreadCreate(Thread* thread, ThreadStartFunc* entry, void* arg);
void ThreadDetach(Thread* thread);
S32 ThreadJoin(Thread* thread);

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
S64 AtomicS64Load(AtomicS64* a);
S64 AtomicS64Exchange(AtomicS64* a, S64 desired);
B8 AtomicS64CompareExchange(AtomicS64* a, S64* expected, S64 desired);
S64 AtomicS64FetchAdd(AtomicS64* a, S64 b);
S64 AtomicS64FetchSub(AtomicS64* a, S64 b);
S64 AtomicS64FetchOr(AtomicS64* a, S64 b);
S64 AtomicS64FetchXor(AtomicS64* a, S64 b);
S64 AtomicS64FetchAnd(AtomicS64* a, S64 b);

void AtomicS32Init(AtomicS32* a, S32 desired);
void AtomicS32Store(AtomicS32* a, S32 desired);
S32 AtomicS32Load(AtomicS32* a);
S32 AtomicS32Exchange(AtomicS32* a, S32 desired);
B8 AtomicS32CompareExchange(AtomicS32* a, S32* expected, S32 desired);
S32 AtomicS32FetchAdd(AtomicS32* a, S32 b);
S32 AtomicS32FetchSub(AtomicS32* a, S32 b);
S32 AtomicS32FetchOr(AtomicS32* a, S32 b);
S32 AtomicS32FetchXor(AtomicS32* a, S32 b);
S32 AtomicS32FetchAnd(AtomicS32* a, S32 b);

void AtomicB32Init(AtomicB32* a, B32 desired);
void AtomicB32Store(AtomicB32* a, B32 desired);
B32 AtomicB32Load(AtomicB32* a);
B32 AtomicB32Exchange(AtomicB32* a, B32 desired);
B8 AtomicB32CompareExchange(AtomicB32* a, B32* expected, B32 desired);
B32 AtomicB32FetchOr(AtomicB32* a, B32 b);
B32 AtomicB32FetchXor(AtomicB32* a, B32 b);
B32 AtomicB32FetchAnd(AtomicB32* a, B32 b);

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
String8 String8CreateRange(U8* str, U8* one_past_last);
String8 _String8CreateCString(U8* c_str);
#define String8CreateCString(s) _String8CreateCString((U8*) s)
#define String8CreateStatic(s) String8Create((U8*)s, sizeof(s) - 1)
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
String8 _String8Format(Arena* arena, U8* fmt, ...);
#define String8Format(a, fmt, ...) _String8Format(a, (U8*) fmt, ##__VA_ARGS__)

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

STATIC_ASSERT(
    sizeof(Arena) < CDEFAULT_ARENA_COMMIT_SIZE,
    "sizeof(Arena) must be less than CDEFAULT_ARENA_COMMIT_SIZE.");
STATIC_ASSERT(
    CDEFAULT_ARENA_COMMIT_SIZE < CDEFAULT_ARENA_RESERVE_SIZE,
    "CDEFAULT_ARENA_COMMIT_SIZE must be less than CDEFAULT_ARENA_RESERVE_SIZE.");

Arena* _ArenaAllocate(U64 reserve_size, U64 commit_size) {
  reserve_size = ALIGN_POW_2(reserve_size, PAGE_SIZE);
  commit_size = ALIGN_POW_2(commit_size, PAGE_SIZE);

  void* base = MemoryReserve(CDEFAULT_ARENA_RESERVE_SIZE);
  DEBUG_ASSERT(base != NULL);
  MemoryCommit(base, CDEFAULT_ARENA_COMMIT_SIZE);

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

void* ArenaPush(Arena* arena, U64 size, U64 align) {
  U64 pos = ALIGN_POW_2(arena->pos, align);
  U64 pos_after = pos + size;

  ASSERT(pos_after <= arena->reserve_size); // If hit, increase reserve size?
  if (arena->commit < pos_after) {
    U64 commit = pos_after + arena->commit_size - 1;
    commit -= commit % arena->commit;
    U64 commit_clamped = CLAMP_TOP(commit, arena->reserve_size);
    U64 commit_size = commit_clamped - arena->commit;
    MemoryCommit((U8*) arena + arena->commit, commit_size);
    arena->commit = commit_clamped;
  }
  ASSERT(pos_after <= arena->commit);

  void* result = ((U8*)arena) + pos;
  arena->pos = pos_after;
  return result;
}

void ArenaPopTo(Arena* arena, U64 pos) {
  ASSERT(pos < arena->pos);
  arena->pos = pos;
}

void ArenaPop(Arena* arena, U64 size) {
  ArenaPopTo(arena, arena->pos - size);
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

String8 _String8CreateCString(U8* c_str) {
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

String8 _String8Format(Arena* arena, U8* fmt, ...) {
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
