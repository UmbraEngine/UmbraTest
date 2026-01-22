#include "umbra/test_runner.h"
#include "umbra/test_group.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

static TestRunner* g_runner = NULL;

static void begin_context(TestRunner* runner, const char* group_name, const char* test_name)
{
  runner->current_group = group_name;
  runner->current_test = test_name;
  runner->in_test = 1;
}

static void end_context(TestRunner* runner)
{
  runner->in_test = 0;
}

void test_runner_init(TestRunner* runner)
{
  memset(runner, 0, sizeof(*runner));
}

void test_runner_test_fail(const char* file, int line, const char* fmt, ...)
{
  if (!g_runner || !g_runner->in_test) {
    fprintf(stderr, "%s:%d: test failure outside of running test\n", file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    return;
  }

  g_runner->is_failure += 1;
  g_runner->total_failures += 1;

  fprintf(
      stderr, "%s:%d: FAIL [%s] %s: ", file, line, check_group_name(g_runner->current_group),
      g_runner->current_test ? g_runner->current_test : "(unknown)"
  );
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

void test_runner_run_one(
    TestRunner* runner,
    const TestGroup* group,
    const TestCase* test,
    TestRunSummary* summary
)
{
  runner->is_failure = 0;

  if (group->before_each) {
    begin_context(runner, group->name, "(before_each)");
    group->before_each(group->hook_user);
    end_context(runner);
  }

  begin_context(runner, group->name, test->name);
  test->fn(test->user);
  end_context(runner);

  if (group->after_each) {
    begin_context(runner, group->name, "(after_each)");
    group->after_each(group->hook_user);
    end_context(runner);
  }

  summary->total += 1;
  if (runner->is_failure > 0) {
    summary->failed += 1;
    fprintf(
        stderr, "FAILED [%s] %s (%d failures)\n", check_group_name(group->name), test->name,
        runner->is_failure
    );
  }
  else {
    summary->passed += 1;
    printf("PASS [%s] %s\n", check_group_name(group->name), test->name);
  }
}

static TestRunSummary test_runner_run_group(TestRunner* runner, TestGroup* group)
{

  TestRunSummary* summary = {0};
  if (!group) {
    return *summary;
  }

  printf("\n[%s] - Test Count: %zu\n", group->name, group->tests.count);

  if (group->before_all) {
    begin_context(runner, group->name, "(before_all)");
    group->before_all(group->hook_user);
    end_context(runner);
  }

  for (size_t i = 0; i < group->tests.count; ++i) {
    TestCase* test = &group->tests.data[i];
    test_runner_run_one(runner, group, test, summary);
  }

  for (TestGroupNode* node = group->head; node; node = node->next) {
    printf("\nThis message should not be visable\n");
    test_runner_run_group(runner, node->group);
  }

  if (group->after_all) {
    begin_context(runner, group->name, "(after_all)");
    group->after_all(group->hook_user);
    end_context(runner);
  }

  return *summary;
}

TestRunSummary test_runner_run_all(TestRunner* runner, const TestRegistry* registry)
{
  g_runner = runner;

  if (!g_runner) {
    fprintf(stderr, "[ERROR]: Runner was unable to be set\n");
    abort();
  }

  if (!registry && !registry->root) {
    fprintf(stderr, "[ERROR]: root TestGroup was unable to be set on default registry\n");
    abort();
  }

  TestRunSummary summary = test_runner_run_group(runner, registry->root);

  g_runner = NULL;

  printf(
      "\nSummary: %d total, %d passed, %d failed\n", summary.total, summary.passed, summary.failed
  );
  return summary;
}
