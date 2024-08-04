SET(TEST_TARGET module_test) 
set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIRS 
    ${APP_DIR}/private
    ${APP_DIR}/include
    ${APP_DIR}/include/qcraftor
    ${APP_DIR}/include/third_party
    ${APP_DIR}/include/third_party/stduuid
    ${APP_DIR}/include/third_party/stduuid/include
)

if(WIN32)
    set(LIBRARY_DIRS ${APP_DIR}/lib/win64 ${APP_DIR}/bin)
elseif(UNIX)
    set(LIBRARY_DIRS ${APP_DIR}/lib/unix ${APP_DIR}/bin)
endif()

# set includes and libraries
include_directories(${INCLUDE_DIRS})
link_directories(${LIBRARY_DIRS})

enable_testing()

add_executable(
  ${TEST_TARGET}
  module_test.cc
)

target_link_libraries(
  ${TEST_TARGET}
  GTest::gtest_main
  qcraftor
  libcrypto
)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})
set_target_properties(${TEST_TARGET} PROPERTIES FOLDER "tests")

# install binaries
install(DIRECTORY DESTINATION ${TESTS_BIN_DIR})
install(TARGETS ${TEST_TARGET}
    LIBRARY DESTINATION ${TESTS_BIN_DIR}
    ARCHIVE DESTINATION ${TESTS_BIN_DIR}
    RUNTIME DESTINATION ${TESTS_BIN_DIR}
)
