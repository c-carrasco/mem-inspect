file (GLOB CXX_FILES FILES *.cxx)
list(REMOVE_ITEM CXX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test_hooks.cxx)

set (TEST_NAME_ALL "test_meminspect")
add_executable (${TEST_NAME_ALL} ${CXX_FILES})
target_include_directories(${TEST_NAME_ALL} PRIVATE ${GTEST_INCLUDE_DIRECTORIES} ../include)
target_link_libraries(${TEST_NAME_ALL}
  GTest::GTest
)

set (TEST_NAME_HOOK "test_meminspect_hooks")
add_executable (${TEST_NAME_HOOK} main.cxx test_hooks.cxx)
target_include_directories(${TEST_NAME_HOOK} PRIVATE ${GTEST_INCLUDE_DIRECTORIES})
target_link_libraries(${TEST_NAME_HOOK}
  meminspect
  GTest::GTest
)

add_test (NAME ${TEST_NAME_ALL} COMMAND $<TARGET_FILE:${TEST_NAME_ALL}>)
add_test (NAME ${TEST_NAME_HOOK} COMMAND $<TARGET_FILE:${TEST_NAME_HOOK}>)