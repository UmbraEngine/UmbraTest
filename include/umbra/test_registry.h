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
TestRegistry* test_default_registry(void);
TestGroup* test_registry_get_group(TestRegistry* registry, const char* group_name);
void register_test(
    TestGroup* group,
    const char* test_name,
    TestCaseFn fn,
    void* user,
    TestTeardownFn teardownFn
);

void test_registry_set_before_all(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
);

void test_registry_set_after_all(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
);

void test_registry_set_before_each(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
);

void test_registry_set_after_each(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
);

#ifdef __cplusplus
}
#endif
