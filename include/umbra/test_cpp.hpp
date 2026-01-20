#pragma once

extern "C" {
#include "test_runner.h"
#include "test_registry.h"
#include "test_case.h"
#include "test_group.h"
}

#define UMBRA_CAT2(a, b) a##b
#define UMBRA_CAT(a, b) UMBRA_CAT2(a, b)

/*
  File-scope safe C++ auto-registration:

  DESCRIBE expands to:
    - a unique static function: void _umbra_desc_body_LINE()
    - a static bool whose initializer calls that function (runs before main)
  Inside the function body we have normal block scope, so TEST/HOOK macros
  can be statement macros and can use lambdas as function pointers.
*/

#define DESCRIBE(group_name, BODY)                                                                 \
  static void UMBRA_CAT(_umbra_desc_body_, __LINE__)();                                            \
  static const bool UMBRA_CAT(_umbra_desc_reg_, __LINE__) = []() {                                 \
    UMBRA_CAT(_umbra_desc_body_, __LINE__)();                                                      \
    return true;                                                                                   \
  }();                                                                                             \
  static void UMBRA_CAT(_umbra_desc_body_, __LINE__)()                                             \
  {                                                                                                \
    TestRegistry* _test_registry = test_default_registry();                                        \
    TestGroup* _test_group = test_registry_get_group(_test_registry, (group_name));                \
    (void)_test_registry;                                                                          \
    (void)_test_group;                                                                             \
    BODY                                                                                           \
  }

/* Hooks: statement macros (must be used inside DESCRIBE body) */
#define BEFORE_EACH(BODY)                                                                          \
  do {                                                                                             \
    test_registry_set_before_each(_test_group, +[](void* user) BODY, NULL, NULL);                  \
  } while (0)

#define AFTER_EACH(BODY)                                                                           \
  do {                                                                                             \
    test_registry_set_after_each(_test_group, +[](void* user) BODY, NULL, NULL);                   \
  } while (0)

#define BEFORE_ALL(BODY)                                                                           \
  do {                                                                                             \
    test_registry_set_before_all(_test_group, +[](void* user) BODY, NULL, NULL);                   \
  } while (0)

#define AFTER_ALL(BODY)                                                                            \
  do {                                                                                             \
    test_registry_set_after_all(_test_group, +[](void* user) BODY, NULL, NULL);                    \
  } while (0)

#define TEST(testName, FN)                                                                         \
  do {                                                                                             \
    register_test(_test_group, (testName), (FN), NULL, NULL);                                \
  } while (0)

#define IT(testName, FN) TEST(testName, FN)

/* Assertions */
#define ASSERT_TRUE(cond)                                                                          \
  do {                                                                                             \
    if (!(cond))                                                                                   \
      test_fail(__FILE__, __LINE__, "ASSERT_TRUE failed: %s", #cond);                              \
  } while (0)

#define ASSERT_EQUAL_INT(a, b)                                                                     \
  do {                                                                                             \
    int _a = (a);                                                                                  \
    int _b = (b);                                                                                  \
    if (_a != _b)                                                                                  \
      test_fail(__FILE__, __LINE__, "ASSERT_EQUAL_INT failed: %d != %d", _a, _b);                  \
  } while (0)

#define REQUIRE_TRUE(cond)                                                                         \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      test_fail(__FILE__, __LINE__, "REQUIRE_TRUE failed: %s", #cond);                             \
      return;                                                                                      \
    }                                                                                              \
  } while (0)
