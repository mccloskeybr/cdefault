#ifndef CDEFAULT_STATUS_H
#define CDEFAULT_STATUS_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#include <cdefault_log.h>

typedef enum {
  StatusCode_Ok,
  StatusCode_Cancelled,
  StatusCode_Unknown,
  StatusCode_InvalidArgument,
  StatusCode_DeadlineExceeded,
  StatusCode_NotFound,
  StatusCode_AlreadyExists,
  StatusCode_PermissionDenied,
  StatusCode_ResourceExhausted,
  StatusCode_FailedPrecondition,
  StatusCode_Aborted,
  StatusCode_OutOfRange,
  StatusCode_Internal,
  StatusCode_Unavailable,
  StatusCode_DataLoss,
  StatusCode_Unauthenticated,
  StatusCode_Unimplemented,
} StatusCode;

typedef struct {
  StatusCode code;
  char msg[256];
} Status;

Status StatusCreate(StatusCode code, char* fmt, ...);
char* StatusCodeToString(StatusCode code);
bool IsOk(Status* status);

#define OK() StatusCreate(StatusCode_Ok, NULL)
#define CANCELLED(fmt, ...) StatusCreate(StatusCode_Cancelled, fmt, __VA_ARGS__)
#define UNKNOWN(fmt, ...) StatusCreate(StatusCode_Unknown, fmt, __VA_ARGS__)
#define INVALID_ARGUMENT(fmt, ...) StatusCreate(StatusCode_InvalidArgument, fmt, __VA_ARGS__)
#define DEADLINE_EXCEEDED(fmt, ...) StatusCreate(StatusCode_DeadlineExceeded, fmt, __VA_ARGS__)
#define NOT_FOUND(fmt, ...) StatusCreate(StatusCode_NotFound, fmt, __VA_ARGS__)
#define ALREADY_EXISTS(fmt, ...) StatusCreate(StatusCode_AlreadyExists, fmt, __VA_ARGS__)
#define PERMISSION_DENIED(fmt, ...) StatusCreate(StatusCode_PermissionDenied, fmt, __VA_ARGS__)
#define RESOURCE_EXHAUSTED(fmt, ...) StatusCreate(StatusCode_ResourceExhausted, fmt, __VA_ARGS__)
#define FAILED_PRECONDITION(fmt, ...) StatusCreate(StatusCode_FailedPrecondition, fmt, __VA_ARGS__)
#define ABORTED(fmt, ...) StatusCreate(StatusCode_Aborted, fmt, __VA_ARGS__)
#define OUT_OF_RANGE(fmt, ...) StatusCreate(StatusCode_OutOfRange, fmt, __VA_ARGS__)
#define INTERNAL(fmt, ...) StatusCreate(StatusCode_Internal, fmt, __VA_ARGS__)
#define UNAVAILABLE(fmt, ...) StatusCreate(StatusCode_Unavailable, fmt, __VA_ARGS__)
#define DATA_LOSS(fmt, ...) StatusCreate(StatusCode_DataLoss, fmt, __VA_ARGS__)
#define UNAUTHENTICATED(fmt, ...) StatusCreate(StatusCode_Unauthenticated, fmt, __VA_ARGS__)
#define UNIMPLEMENTED(fmt, ...) StatusCreate(StatusCode_Unimplemented, fmt, __VA_ARGS__)

#define RETURN_IF_ERROR(exp)               \
  {                                        \
    Status status = (exp);                 \
    if (!IsOk(&status)) { return status; } \
  }
#define FATAL_IF_ERROR(exp)                \
  {                                        \
    Status status = (exp);                 \
    if (!IsOk(&status)) {                  \
      LOG_FATAL(status.msg);               \
    }                                      \
  }

#ifdef CDEFAULT_STATUS_IMPL

Status StatusCreate(StatusCode code, char* fmt, ...) {
  Status status;
  status.code = code;

  if (code == StatusCode_Ok) {
    return status;
  } else {
    int32_t offset = snprintf(status.msg, sizeof(status.msg), "%s: ", StatusCodeToString(code));
    va_list args;
    va_start(args, fmt);
    vsnprintf(status.msg + offset, sizeof(status.msg) - offset, fmt, args);
    va_end(args);
    return status;
  }
}

char* StatusCodeToString(StatusCode code) {
  switch(code) {
    case StatusCode_Ok: return "OK";
    case StatusCode_Cancelled: return "CANCELLED";
    case StatusCode_Unknown: return "UNKNOWN";
    case StatusCode_InvalidArgument: return "INVALID_ARGUMENT";
    case StatusCode_DeadlineExceeded: return "DEADLINE_EXCEEDED";
    case StatusCode_NotFound: return "NOT_FOUND";
    case StatusCode_AlreadyExists: return "ALREADY_EXISTS";
    case StatusCode_PermissionDenied: return "PERMISSION_DENIED";
    case StatusCode_ResourceExhausted: return "RESOURCE_EXHAUSTED";
    case StatusCode_FailedPrecondition: return "FAILED_PRECONDITION";
    case StatusCode_Aborted: return "ABORTED";
    case StatusCode_OutOfRange: return "OUT_OF_RANGE";
    case StatusCode_Internal: return "INTERNAL";
    case StatusCode_Unavailable: return "UNAVAILABLE";
    case StatusCode_DataLoss: return "DATA_LOSS";
    case StatusCode_Unauthenticated: return "UNAUTHENTICATED";
    case StatusCode_Unimplemented: return "UNIMPLEMENTED";
    default: return "UNKNOWN_STATUS_CODE";
  }
}

inline bool IsOk(Status* status) {
  return status->code == StatusCode_Ok;
}

#endif
#endif
