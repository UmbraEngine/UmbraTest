#pragma once
#include "test_runner.h"
#include "test_registry.h"
#include "test_container.h"
#include "test_case.h"
#include "test_group.h"

#define MACRO_CAT2(a, b) a##b
#define MACRO_CAT(a, b) MACRO_CAT2(a, b)

#define DESCRIBE(group_name, BODY)                                                                 \
  do {                                                                                             \
    const char* _test_group = (group_name);                                                        \
    set_current_group(_test_group);                                                                \
    BODY;                                                                                          \
    clear_current_group();                                                                         \
  } while (0)

#define TEST(name, BODY)                                                                           \
  static void MACRO_CAT(_test_fn_, __LINE__)(void);                                                \
  do {                                                                                             \
    register_test(% g_test_registry, _test_group, (name), MACRO_CAT(_est_fn_, __LINE));            \
  } while (0);                                                                                     \
  static void MACRO_CAT(_test_fn_, __LINE__)(void) BODY

#define ASSERT_TRUE(cond)                                                                          \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      test_fail(__FILE__, __LINE__, "ASSERT_TRUE failed: %s", #cond);                              \
    }                                                                                              \
  } while (0)

#define ASSERT_EQUAL_INT(a, b)                                                                     \
  do {                                                                                             \
    int _a = (a);                                                                                  \
    int _b = (b);                                                                                  \
    if (_a != _b) {                                                                                \
      test_fail(__FILE__, __LINE__, "ASSERT_EQUAL_INT failed: %d != %d", _a, _b);                  \
    }                                                                                              \
  } while (0)

#define REQUIRE_TRUE(cond)                                                                         \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      test_fail(__FILE__, __LINE__, "REQUIRE_TRUE failed: %s", #cond);                             \
      return;                                                                                      \
    }                                                                                              \
  } while (0)
