find_package(GTest REQUIRED)

add_executable(date_unittest DateUnittest.cpp)
set(UNITTEST_TARGETS
    date_unittest
)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD 14)

include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
    target_link_libraries(${T} PRIVATE xiaoLog GTest::GTest)
    gtest_discover_tests(${T})
endforeach(T ${UNITTEST_TARGETS})
