#pragma once
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TestCaseFn)(void* user);
typedef void (*TestTeardownFn)(void* user);

typedef struct {
  const char* name;
  TestCaseFn fn;
  void* user;
  TestTeardownFn teardown;
} TestCase;

#ifdef __cplusplus
}
#endif
