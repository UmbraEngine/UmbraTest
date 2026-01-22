#pragma once
#include <stdlib.h>
#include "test_case.h"
#include "test_group.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  TestGroup* root;
  size_t count;
  size_t capacity;
} TestRegistry;

void test_registry_init(TestRegistry* registry);
void test_registry_free(TestRegistry* registry);
void* test_registry_alloc(TestRegistry* registry, size_t size);
char* test_registry_strdup(TestRegistry* registry, const char* string);
TestRegistry* test_registry_get_default_registry(void);
TestGroup* test_registry_find_or_create_group(TestRegistry* registry, const char* group_name);
TestGroup*
test_registry_get_child_group(TestRegistry* registry, TestGroup* parent, const char* group_name);
void test_registry_register_test(
    TestRegistry* registry,
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
