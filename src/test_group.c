#include "umbra/test_group.h"
#include "umbra/test_container.h"

static char* DEFAULT_GROUP_NAME = "Ungrouped";

void add_test_to_group(TestGroup* group, const char* test_name, TestCaseFn fn, void* user, TestTeardownFn teardownFn)
{
  if (group->count >= group->capacity) {
    group->tests = (TestCase*)test_container_grow(group->tests, sizeof(TestCase), &group->capacity);
  }
  group->tests[group->count++] = (TestCase){.name = test_name, .fn = fn, .user = user, .teardown = teardownFn};
}

const char* check_group_name(const char* group_name){
  return group_name ? group_name : DEFAULT_GROUP_NAME;
}

