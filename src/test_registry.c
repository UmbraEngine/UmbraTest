#include "umbra/test_registry.h"
#include "umbra/test_case.h"
#include "umbra/test_group.h"
#include "umbra/test_container.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static TestGroup* test_registry_find_group(TestGroup* parent, const char* group_name)
{
  for (TestGroupNode* it = parent->head; it; it = it->next) {
    if (strcmp(it->group->name, group_name) == 0) {
      return it->group;
    }
  }
  return NULL;
}

static TestGroup*
test_registry_add_child_to_group(TestRegistry* registry, TestGroup* parent, const char* group_name)
{
  TestGroup* group = (TestGroup*)test_registry_alloc(registry, sizeof(TestGroup));
  memset(group, 0, sizeof(*group));

  group->name = test_registry_strdup(registry, group_name);
  group->parent = parent;

  test_container_init(&group->tests);

  TestGroupNode* node = test_registry_alloc(registry, sizeof(TestGroupNode));
  node->group = group;
  node->next = NULL;

  if (parent->tail) {
    parent->tail->next = node;
  }
  else {
    parent->head = node;
  }
  parent->tail = node;
  parent->length += 1;

  return group;
}

void test_registry_init(TestRegistry* registry)
{
  memset(registry, 0, sizeof(*registry));

  registry->root = test_registry_alloc(registry, sizeof(TestGroup));
  memset(registry->root, 0, sizeof(TestGroup));

  registry->root->name = test_registry_strdup(registry, "ROOT");
  registry->root->parent = NULL;

  registry->root->head = NULL;
  registry->root->tail = NULL;
  registry->root->length = 0;

  test_container_init(&registry->root->tests);

  registry->count = 0;
  registry->capacity = 0;
}

void test_registry_free(TestRegistry* registry)
{
  if (!registry)
    return;

  for (size_t gi = 0; gi < registry->count; ++gi) {
    TestGroup* group = &registry->root[gi];

    for (size_t ti = 0; ti < group->count; ++ti) {
      TestCase* test = &group->tests.data[ti];
      if (test->teardown && test->user) {
        test->teardown(test->user);
      }
    }

    if (group->hook_teardown && group->hook_user) {
      group->hook_teardown(group->hook_user);
    }

    test_container_free(&group->tests);
  }

  free(registry->root);
  registry->root = NULL;
  registry->count = 0;
  registry->capacity = 0;
}

void* test_registry_alloc(TestRegistry* registry, size_t size)
{
  (void)registry;
  void* ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "UmbraTest: allocation failed (%zu bytes)\n", size);
    abort();
  }
  return ptr;
}
char* test_registry_strdup(TestRegistry* registry, const char* str)
{
  (void)registry;
  size_t len = strlen(str) + 1;
  char* copy = (char*)malloc(len);
  if (!copy) {
    fprintf(stderr, "UmbraTest: strdup failed\n");
    abort();
  }
  memcpy(copy, str, len);
  return copy;
}

TestRegistry* test_registry_get_default_registry(void)
{
  static TestRegistry registry;
  static int initialized = 0;
  if (!initialized) {
    test_registry_init(&registry);
    initialized = 1;
  }
  return &registry;
}

void test_registry_register_test(
    TestRegistry* registry,
    TestGroup* group,
    const char* test_name,
    TestCaseFn fn,
    void* user,
    TestTeardownFn teardownFn
)
{
  if (!group) {
    fprintf(stderr, "UmbraTest: attempted to register test '%s' with NULL group\n", test_name);
    abort();
  }
  TestCase test = {0};
  test.name = test_registry_strdup(registry, test_name);
  test.fn = fn;
  test.user = user;
  test.teardown = teardownFn;

  test_container_push(&group->tests, test);

  fprintf(
      stderr, "REGISTER TEST: '%s' into group=%p (%s)\n", test_name, (void*)group,
      group ? group->name : "NULL"
  );
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

TestGroup*
test_registry_get_child_group(TestRegistry* registry, TestGroup* parent, const char* group_name)
{
  TestGroup* existing = test_registry_find_group(parent, group_name);
  if (existing)
    return existing;
  return test_registry_add_child_to_group(registry, parent, group_name);
}
