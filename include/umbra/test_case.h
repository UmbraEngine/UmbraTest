#pragma once
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TestCaseFn)(void* user);

typedef struct {
  const char* name;
  TestCaseFn fn;
  void* user;
} TestCase;


void run_tests(void);

#ifdef __cplusplus
}
#endif
