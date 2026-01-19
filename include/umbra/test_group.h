#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "test_case.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  const char* name;
  TestCase* tests;
  size_t count;
  size_t capacity;
} TestGroup;

void add_test_to_group(TestGroup* group, const char* test_name, TestCaseFn fn, void* user);
const char* check_group_name(const char* group_name);

#ifdef __cplusplus
}
#endif
