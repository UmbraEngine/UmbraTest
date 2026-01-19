#include "umbra/test_registry.h"
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
  for (size_t i = 0; i < registry->count; ++i) {
    free(registry->groups[i].tests);
    registry->groups[i].tests = NULL;
    registry->groups[i].count = 0;
    registry->groups[i].capacity = 0;
  }

  free(registry->groups);
  registry->groups = NULL;
  registry->count = 0;
  registry->capacity = 0;
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

TestGroup* get_group(TestRegistry* registry, const char* group_name)
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
    TestRegistry* registry,
    const char* group_name,
    const char* test_name,
    TestCaseFn fn,
    void* user
)
{
  TestGroup* group = get_group(registry, group_name);
  add_test_to_group(group, test_name, fn, user);
}
