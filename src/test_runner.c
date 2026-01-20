#include "umbra/test_runner.h"
#include "umbra/test_group.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


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

void test_fail(const char* file, int line, const char* fmt, ...)
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

  g_runner->current_failures += 1;
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

void run_one(
    TestRunner* runner,
    const TestGroup* group,
    const TestCase* test,
    TestRunSummary* summary
)
{
  runner->current_failures = 0;

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
  if (runner->current_failures > 0) {
    summary->failed += 1;
    fprintf(
        stderr, "FAILED [%s] %s (%d failures)\n", check_group_name(group->name), test->name,
        runner->current_failures
    );
  }
  else {
    summary->passed += 1;
    printf("PASS [%s] %s\n", check_group_name(group->name), test->name);
  }
}

TestRunSummary test_runner_run_all(TestRunner* runner, const TestRegistry* registry)
{
  TestRunSummary summary = {0};
  g_runner = runner;

  for (size_t gi = 0; gi < registry->count; ++gi) {
    const TestGroup* group = &registry->groups[gi];
    printf("\n[%s]\n", group->name);
    if (group->before_all) {
      begin_context(runner, group->name, "(after_all)");
      group->before_all(group->hook_user);
      end_context(runner);
    }
    for (size_t ti = 0; ti < group->count; ++ti) {
      begin_context(runner, group->name, "(after_all)");
      run_one(runner, group, &group->tests[ti], &summary);
      end_context(runner);
    }
    if (group->after_all) {
      group->after_all(group->hook_user);
    }
  }

  g_runner = NULL;

  printf(
      "\nSummary: %d total, %d passed, %d failed\n", summary.total, summary.passed, summary.failed
  );
  return summary;
}
