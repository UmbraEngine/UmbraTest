#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "test_case.h"
#include "test_group.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  TestGroup* groups;
  size_t count;
  size_t capacity;
} TestRegistry;


void test_registry_init(TestRegistry* registry);
void test_registry_free(TestRegistry* registry);
TestGroup* get_group(TestRegistry* registry, const char* group_name);
void register_test(
    TestRegistry* registry,
    const char* group_name,
    const char* test_name,
    TestCaseFn fn,
    void* user
);

#ifdef __cplusplus
}
#endif
