#pragma once

extern "C" {
#include "test_runner.h"
#include "test_registry.h"
#include "test_case.h"
#include "test_group.h"
}

/* ################################# */
/* ========= DESCRIBE: ============ */
/* ################################# */

#define MACRO_CAT2(a, b) a##b
#define MACRO_CAT(a, b) MACRO_CAT2(a, b)

#define DESCRIBE(group_name, BODY)                                                                 \
  static void MACRO_CAT(_test_framework_desc_body_, __LINE__)();                                   \
  static const bool MACRO_CAT(_test_framework_desc_reg_, __LINE__) = []() {                        \
    MACRO_CAT(_test_framework_desc_body_, __LINE__)();                                             \
    return true;                                                                                   \
  }();                                                                                             \
  static void MACRO_CAT(_test_framework_desc_body_, __LINE__)()                                    \
  {                                                                                                \
    TestRegistry* _test_registry = test_default_registry();                                        \
    TestGroup* _test_group = test_registry_get_group(_test_registry, (group_name));                \
    (void)_test_registry;                                                                          \
    (void)_test_group;                                                                             \
    BODY                                                                                           \
  }

/* ################################# */
/* ========= HOOKS: ============ */
/* ################################# */
// NOTE: Must be used inside Describe body
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

/* ################################# */
/* =========== TESTS: ============== */
/* ################################# */
#define TEST(testName, FN)                                                                         \
  do {                                                                                             \
    register_test(_test_group, (testName), (FN), NULL, NULL);                                      \
  } while (0)

#define IT(testName, FN) TEST(testName, FN)

/* ################################# */
/* ========= ASSERTIONS: ============ */
/* ################################# */
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
