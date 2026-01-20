#include "umbra/test_registry.h"
#include "umbra/test_case.h"
#include "umbra/test_group.h"
#include "umbra/test_container.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void test_registry_init(TestRegistry* registry)
{
  registry->groups = NULL;
  registry->count = 0;
  registry->capacity = 0;
}

void test_registry_free(TestRegistry* registry)
{
  if (!registry)
    return;

  for (size_t gi = 0; gi < registry->count; ++gi) {
    TestGroup* group = &registry->groups[gi];

    for (size_t ti = 0; ti < group->count; ++ti) {
      TestCase* test = &group->tests[ti];
      if (test->teardown && test->user) {
        test->teardown(test->user);
      }
    }

    if (group->hook_teardown && group->hook_user) {
      group->hook_teardown(group->hook_user);
    }

    free(group->tests);
    group->tests = NULL;
    group->count = 0;
    group->capacity = 0;
  }

  free(registry->groups);
  registry->groups = NULL;
  registry->count = 0;
  registry->capacity = 0;
}

TestRegistry* test_default_registry(void)
{
  static TestRegistry registry;
  static int initialized = 0;
  if (!initialized) {
    test_registry_init(&registry);
    initialized = 1;
  }
  return &registry;
}

static TestGroup* test_registry_push_group(TestRegistry* registry, const char* name)
{
  if (registry->count >= registry->capacity) {
    registry->groups =
        (TestGroup*)test_container_grow(registry->groups, sizeof(TestGroup), &registry->capacity);
  }
  TestGroup* group = &registry->groups[registry->count++];
  group->name = name;
  group->tests = NULL;
  group->count = 0;
  group->capacity = 0;
  return group;
}

TestGroup* test_registry_get_group(TestRegistry* registry, const char* group_name)
{
  if (!group_name)
    group_name = "Ungrouped";
  for (size_t i = 0; i < registry->count; ++i) {
    if (strcmp(registry->groups[i].name, group_name) == 0) {
      return &registry->groups[i];
    }
  }
  return test_registry_push_group(registry, group_name);
}

void register_test(
    TestGroup* group,
    const char* test_name,
    TestCaseFn fn,
    void* user,
    TestTeardownFn teardownFn
)
{
  add_test_to_group(group, test_name, fn, user, teardownFn);
}

static void set_hook(
    TestGroup* group,
    TestHookFn* slot,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
)
{

  if (group->hook_teardown && group->hook_user && group->hook_user != hook_user) {
    group->hook_teardown(group->hook_user);
  }

  *slot = fn;

  group->hook_user = hook_user;
  group->hook_teardown = hook_teardown;
}

void test_registry_set_before_each(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
)
{
  set_hook(group, &group->before_each, fn, hook_user, hook_teardown);
}

void test_registry_set_after_each(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
)
{
  set_hook(group, &group->after_each, fn, hook_user, hook_teardown);
}

void test_registry_set_before_all(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
)
{
  set_hook(group, &group->before_all, fn, hook_user, hook_teardown);
}

void test_registry_set_after_all(
    TestGroup* group,
    TestHookFn fn,
    void* hook_user,
    void (*hook_teardown)(void*)
)
{
  set_hook(group, &group->after_all, fn, hook_user, hook_teardown);
}
