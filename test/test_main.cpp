#include "umbra/test_registry.h"
#include <umbra/test.h>
#include <umbra/test_debug.hpp>
#include <source_location>
#include <umbra/test_log.hpp>

using namespace Umbra::Test::Log;

int main(int argc, char* argv[]) {
  testLogPrint(LogColor::Blue, "Hello %d", 2);
  check_debug(argc, argv);
  TestRunner runner;
  test_runner_init(&runner);
  TestRegistry* registry = test_registry_get_default_registry();
  TestRunSummary summary = test_runner_run_all(&runner, registry);
  return (summary.failed == 0) ? 0 : 1;
}

