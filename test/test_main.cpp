#include "umbra/test_registry.h"
#include <umbra/test.h>
#include <umbra/test_debug.hpp>

int main(int argc, char* argv[]) {
  check_debug(argc, argv);
  TestRunner runner;
  test_runner_init(&runner);
  TestRegistry* registry = test_registry_get_default_registry();
  TestRunSummary summary = test_runner_run_all(&runner, registry);
  return (summary.failed == 0) ? 0 : 1;
}

