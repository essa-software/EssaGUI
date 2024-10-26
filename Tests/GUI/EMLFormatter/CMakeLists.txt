include(GoogleTest)

essautil_add_test(EMLFormatter LIBS GTest::gtest_main Essa::GUI)
if (TARGET TestEMLFormatter)
    target_compile_definitions(TestEMLFormatter PRIVATE TESTCASES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/testcases")
endif()
