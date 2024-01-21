include(GoogleTest)

essautil_add_test(EMLLayout LIBS GTest::gtest_main Essa::GUI)
if (TARGET TestEMLLayout)
    target_compile_definitions(TestEMLLayout PRIVATE TESTCASES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/testcases")
endif()
