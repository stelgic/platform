# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.12.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_VISIBLITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINE_HIDDEN YES)

#SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

SET(PROJECT_NAME "qcraftor") 
PROJECT(${PROJECT_NAME} LANGUAGES CXX) 

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -03 -flto -march=native -mute=native")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

cmake_policy(SET CMP0086 NEW)   # Install CODE|SCRIPT allow the use of generator expressions.

# project include directories
if(UNIX)
    message(STATUS "Building for Unix OS")

    # CHANGE APP_DIR TO APPLICATION INSTALLATION FOLDER
    set(APP_DIR "/usr/local/Stelgic Fintech/Stelgic")
    # add link libraries
    set(LIBRARY_DIRS ${APP_DIR}/lib/unix)
    SET(LIBS m qcraftor dl uuid)

elseif(WIN32)
    message(STATUS "Building for Windows OS")
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)

    # CHANGE APP_DIR TO APPLICATION INSTALLATION FOLDER
    set(APP_DIR "C:/Program Files/Stelgic Fintech/Stelgic")
    # add link libraries
    set(LIBRARY_DIRS ${APP_DIR}/lib/win64 ${APP_DIR}/bin )
    #SET(LIBS g3log jsoncpp arrow arrow_dataset arrow_acero)
    SET(LIBS qcraftor g3log boost_system-vc140-mt boost_program_options-vc140-mt
        boost_iostreams-vc140-mt libssl libcrypto websockets jsoncpp 
        cpr libcurl arrow arrow_dataset arrow_acero zip libzippp fftw3 )
endif()


# create build directory
SET(BINARY_DIR ${CMAKE_SOURCE_DIR}/binaries/bin CACHE STRING "")
SET(CMAKE_INSTALL_LIBDIR ${CMAKE_SOURCE_DIR}/binaries CACHE STRING "")
install(DIRECTORY DESTINATION ${BINARY_DIR})

# add modules implemenations
add_subdirectory (processors)
add_subdirectory (strategies)
add_subdirectory (metrics)
add_subdirectory (tests)

