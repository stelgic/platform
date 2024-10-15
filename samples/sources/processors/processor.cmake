# macro to generate cmake for each subfolder

macro( add_processor_module name)
    SET(TARGET_PROCESSOR ${name}) 
    PROJECT(${TARGET_PROCESSOR}) 

    # build shared module
    add_definitions(-DBUILD_SHARED_LIBS=ON)
    add_definitions(-DFFTWPP_SINGLE_THREAD)
    set(LDFLAGS "-Wl,-rpath,'$$ORIGIN'")

    set(INCLUDE_DIRS 
        ${CMAKE_SOURCE_DIR}/binaries/include
        ${CMAKE_SOURCE_DIR}/binaries/include/stelgic
        ${CMAKE_SOURCE_DIR}/binaries/include/third_party
        ${CMAKE_SOURCE_DIR}/binaries/include/third_party/stduuid
        ${CMAKE_SOURCE_DIR}/binaries/include/third_party/stduuid/include
        ${FFTW_ROOT}/include
    )

    # project include directories
    if(UNIX)
        # add link libraries
        set(LIBRARY_DIRS 
            ${BINARY_DIR}/lib/unix
            ${BOOST_ROOT}/lib
            ${OPENSSL_ROOT}/lib
            ${JSON_ROOT}/lib64
            ${FFTW_ROOT}/lib
            ${G3LOG_ROOT}/lib64
        )

        SET(LIBS m stdc++ stelgic.a g3log arrow arrow_dataset jsoncpp dl uuid)

    elseif(WIN32)
        # add link libraries
        set(LIBRARY_DIRS ${BINARY_DIR}/lib/win64)
        #SET(LIBS g3log jsoncpp arrow arrow_dataset arrow_acero)
        SET(LIBS stelgic)

    endif()

    file(GLOB TARGET_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.cpp
    )

    # Add executable called "craftor" that is built from the source files
    add_library(${TARGET_PROCESSOR} SHARED )

    # add sources to the library target
    target_sources(
        ${TARGET_PROCESSOR}
        PRIVATE ${TARGET_SRC}
    )

    # set the include directories
    target_include_directories(
        ${TARGET_PROCESSOR}
        PUBLIC ${INCLUDE_DIRS}
    )

    target_link_directories(
        ${TARGET_PROCESSOR}
        PUBLIC ${LIBRARY_DIRS}
    )

    # Link the executable to the library.
    target_link_libraries (${TARGET_PROCESSOR} PRIVATE ${LIBS})
    set_target_properties(${TARGET_PROCESSOR} PROPERTIES FOLDER "processors")

    set(PROCESSOR_LIB_DIR ${BINARY_DIR}/modules/processors)
    install(DIRECTORY DESTINATION ${PROCESSOR_LIB_DIR})

    install(TARGETS ${TARGET_PROCESSOR} 
        LIBRARY DESTINATION ${PROCESSOR_LIB_DIR}
        ARCHIVE DESTINATION ${PROCESSOR_LIB_DIR}
        RUNTIME DESTINATION ${PROCESSOR_LIB_DIR}
    )

    set(EXECUTABLE_DIR ${BINARY_DIR}/bin)
    install(DIRECTORY DESTINATION ${EXECUTABLE_DIR})

    if(UNIX)
        set(OUT_LIBRARY ${BINARY_DIR}/lib/unix)
        
        install(TARGETS ${TARGET} ${TARGET}
            RUNTIME_DEPENDENCIES
            PRE_INCLUDE_REGEXES ${TARGET}
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*usr/lib64/.*\\.so"
            DIRECTORIES ${BINARY_DIRS}
            LIBRARY DESTINATION ${OUT_LIBRARY}
            ARCHIVE DESTINATION ${OUT_LIBRARY}
            RUNTIME DESTINATION ${EXECUTABLE_DIR}
        )
    endif()

endmacro()

