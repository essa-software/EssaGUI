include(GoogleTest)

essautil_add_test(Line2D LIBS GTest::gtest_main Essa::Geometry)
essautil_add_test(Intersection LIBS GTest::gtest_main Essa::Geometry)
essautil_add_test(Distance LIBS GTest::gtest_main Essa::Geometry)
