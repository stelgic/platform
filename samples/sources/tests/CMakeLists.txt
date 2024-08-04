cmake_minimum_required(VERSION 3.14)
project(modules_tests)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0135 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TESTS_INSTALL_DIR ${CMAKE_SOURCE_DIR}/binaries/tests)
set(TESTS_BIN_DIR ${TESTS_INSTALL_DIR}/bin)

# add tests sub directories
add_subdirectory (modules)

set_target_properties("gmock" PROPERTIES FOLDER "tests")
set_target_properties("gmock_main" PROPERTIES FOLDER "tests")
set_target_properties("gtest" PROPERTIES FOLDER "tests")
set_target_properties("gtest_main" PROPERTIES FOLDER "tests")
set_target_properties("gmock_main" PROPERTIES FOLDER "tests")

# install binaries
install(DIRECTORY DESTINATION ${TESTS_INSTALL_DIR})
install(DIRECTORY DESTINATION ${TESTS_BIN_DIR})
