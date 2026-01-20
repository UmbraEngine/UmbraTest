#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "test_case.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TestHookFn)(void* user);

typedef struct {
  const char* name;
  TestCase* tests;
  size_t count;
  size_t capacity;

  TestHookFn before_all;
  TestHookFn after_all;
  TestHookFn before_each;
  TestHookFn after_each;
  void* hook_user;
  void (*hook_teardown)(void*);
} TestGroup;

void add_test_to_group(TestGroup* group, const char* test_name, TestCaseFn fn, void* user, TestTeardownFn teardownFn);
const char* check_group_name(const char* group_name);

#ifdef __cplusplus
}
#endif
