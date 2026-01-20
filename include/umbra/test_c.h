#warning "INCLUDING test_c.h"
#include "test_runner.h"
#include "test_registry.h"
#include "test_case.h"
#include "test_group.h"

#define MACRO_CAT2(a, b) a##b
#define MACRO_CAT(a, b) MACRO_CAT2(a, b)

/*
  DESCRIBE binds:
    - _test_registry : TestRegistry*
    - _test_group    : TestGroup*
*/
#define DESCRIBE(group_name, BODY)                                                                 \
  do {                                                                                             \
    TestRegistry* _test_registry = test_default_registry();                                        \
    TestGroup* _test_group = test_registry_get_group(_test_registry, (group_name));                \
    BODY;                                                                                          \
  } while (0)

/* ---------------------------
   Hooks
   --------------------------- */

#define BEFORE_EACH(BODY)                                                                          \
  static void MACRO_CAT(_before_each_fn_, __LINE__)(void*);                                        \
  do {                                                                                             \
    test_registry_set_before_each(_test_group, MACRO_CAT(_before_each_fn_, __LINE__), NULL, NULL); \
  } while (0);                                                                                     \
  static void MACRO_CAT(_before_each_fn_, __LINE__)(void* user) BODY

#define AFTER_EACH(BODY)                                                                           \
  static void MACRO_CAT(_after_each_fn_, __LINE__)(void*);                                         \
  do {                                                                                             \
    test_registry_set_after_each(_test_group, MACRO_CAT(_after_each_fn_, __LINE__), NULL, NULL);   \
  } while (0);                                                                                     \
  static void MACRO_CAT(_after_each_fn_, __LINE__)(void* user) BODY

#define BEFORE_ALL(BODY)                                                                           \
  static void MACRO_CAT(_before_all_fn_, __LINE__)(void*);                                         \
  do {                                                                                             \
    test_registry_set_before_all(_test_group, MACRO_CAT(_before_all_fn_, __LINE__), NULL, NULL);   \
  } while (0);                                                                                     \
  static void MACRO_CAT(_before_all_fn_, __LINE__)(void* user) BODY

#define AFTER_ALL(BODY)                                                                            \
  static void MACRO_CAT(_after_all_fn_, __LINE__)(void*);                                          \
  do {                                                                                             \
    test_registry_set_after_all(_test_group, MACRO_CAT(_after_all_fn_, __LINE__), NULL, NULL);     \
  } while (0);                                                                                     \
  static void MACRO_CAT(_after_all_fn_, __LINE__)(void* user) BODY

/* ---------------------------
   Tests
   --------------------------- */

#define TEST(name, BODY)                                                                           \
  static void MACRO_CAT(_test_fn_, __LINE__)(void*);                                               \
  do {                                                                                             \
    register_test(                                                                                 \
        _test_registry, _test_group->name, (name), MACRO_CAT(_test_fn_, __LINE__), NULL, NULL      \
    );                                                                                             \
  } while (0);                                                                                     \
  static void MACRO_CAT(_test_fn_, __LINE__)(void* user) BODY

#define IT(name, BODY) TEST(name, BODY)

/* ---------------------------
   Assertions
   --------------------------- */

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
