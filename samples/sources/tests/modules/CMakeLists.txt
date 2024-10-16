SET(TEST_TARGET module_test) 
set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIRS 
    ${APP_DIR}/private
    ${APP_DIR}/include
    ${APP_DIR}/include/stelgic
    ${APP_DIR}/include/third_party
    ${APP_DIR}/include/third_party/stduuid
    ${APP_DIR}/include/third_party/stduuid/include
)

if(WIN32)
    set(LIBRARY_DIRS ${APP_DIR}/lib/win64 ${APP_DIR}/bin)
    SET(LIBS stelgic g3log arrow arrow_dataset jsoncpp)
elseif(UNIX)
    set(LIBRARY_DIRS ${APP_DIR}/lib/unix ${APP_DIR}/bin)
    SET(LIBS m stdc++ stelgic g3log curl cpr ssl crypto jsoncpp dl uuid)
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
  ${LIBS}
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

if(WIN32)
    install(TARGETS ${TARGET} ${TARGET}
        RUNTIME_DEPENDENCIES
        PRE_INCLUDE_REGEXES ${TARGET}
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
        DIRECTORIES ${LIBRARY_DIRS}
        LIBRARY DESTINATION ${TESTS_BIN_DIR}
        ARCHIVE DESTINATION ${TESTS_BIN_DIR}
        RUNTIME DESTINATION ${TESTS_BIN_DIR}
    )
else()
    install(TARGETS ${TARGET} ${TARGET}
        RUNTIME_DEPENDENCIES
        PRE_INCLUDE_REGEXES ${TARGET}
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
        POST_EXCLUDE_REGEXES "/usr/lib64/.*\\.so"
        DIRECTORIES ${LIBRARY_DIRS}
        LIBRARY DESTINATION ${TESTS_BIN_DIR}
        ARCHIVE DESTINATION ${TESTS_BIN_DIR}
        RUNTIME DESTINATION ${TESTS_BIN_DIR}
    )
endif()
