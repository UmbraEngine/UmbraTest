#include "umbra/test_runner.h"
#include "umbra/test_group.h"
#include "umbra/test_log.hpp"
#include <source_location>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

static TestRunner* g_runner = NULL;
using namespace Umbra::Test::Log;

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
    fprintf(
        stderr, LOG_COLOR_CODE_RED TAG_ERROR "%s:%d: test failure outside of running test\n", file,
        line
    );
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, LOG_COLOR_CODE_DEFAULT "\n");
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
    //testLogPrint(LogColor::Blue, "Hello %d", summary->failed);
    // testLogPrint(
    //     LogColor::Red, "FAILED [%s] %s (%d failures)\n",
    //     check_group_name(group->name), *test->name, runner->is_failure
    // );
  }
  else {
    summary->passed += 1;
    TEST_LOG_PRINT(2, "\nPASS [%s] %s", check_group_name(group->name), test->name);
  }
}

static TestRunSummary*
test_runner_run_group(TestRunner* runner, TestGroup* group, TestRunSummary* summary)
{
  if (!group) {
    return summary;
  }

#ifdef ENABLE_DEBUG
  fprintf(
      stderr, "\n[DEBUG] [%s] head=%p tail=%p\n", group->name, (void*)group->head,
      (void*)group->tail
  );
#endif

#ifdef ENABLE_DEBUG
  TEST_LOG_PRINT(LOG_COLOR_YELLOW, "\n[%s] - Test Count: %zu\n", group->name, group->tests.count);
#else
  if (strcmp(group->name, "ROOT")) {
    TEST_LOG_PRINT_C(2, "\n[%s]", group->name);
  }
#endif

#ifdef ENABLE_DEBUG
  TEST_LOG_DEBUG(
      "[%s] tests: count=%zu cap=%zu data=%p\n", group->name, group->tests.count,
      group->tests.capacity, (void*)group->tests.data
  );
#endif

  if (group->tests.count > group->tests.capacity) {
    fprintf(
        stderr, LOG_COLOR_CODE_RED "BROKEN CONTAINER: count > capacity\n" LOG_COLOR_CODE_DEFAULT
    );
    abort();
  }
  if (group->tests.count > 0 && group->tests.data == NULL) {
    fprintf(
        stderr,
        LOG_COLOR_CODE_RED "BROKEN CONTAINER: count>0 but data==NULL\n" LOG_COLOR_CODE_DEFAULT
    );
    abort();
  }

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
    test_runner_run_group(runner, node->group, summary);
    printf("\n");
  }

  if (group->after_all) {
    begin_context(runner, group->name, "(after_all)");
    group->after_all(group->hook_user);
    end_context(runner);
  }

  return summary;
}

TestRunSummary test_runner_run_all(TestRunner* runner, const TestRegistry* registry)
{
  g_runner = runner;

  if (!g_runner) {
    TEST_LOG_ERROR("%s", "Runner was unable to be set");
    abort();
  }

  if (!registry && !registry->root) {
    TEST_LOG_ERROR("%s", "root TestGroup was unable to be set on default registry");
    abort();
  }

  TestRunSummary summary = {0};

  test_runner_run_group(runner, registry->root, &summary);

  g_runner = NULL;

  printf(
      LOG_COLOR_CODE_WHITE "\nSummary: " LOG_COLOR_CODE_YELLOW "%d total" LOG_COLOR_CODE_DEFAULT
                           ", " LOG_COLOR_CODE_GREEN "%d passed" LOG_COLOR_CODE_DEFAULT
                           ", " LOG_COLOR_CODE_RED "%d failed\n\n" LOG_COLOR_CODE_DEFAULT,
      summary.total, summary.passed, summary.failed
  );
  return summary;
}
