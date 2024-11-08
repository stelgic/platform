# macro to generate cmake for each subfolder

macro( add_processor_module name)
    SET(TARGET_PROCESSOR ${name}) 
    PROJECT(${TARGET_PROCESSOR}) 

    # build shared module
    add_definitions(-DBUILD_SHARED_LIBS=ON)
    add_definitions(-DFFTWPP_SINGLE_THREAD)
    set(LDFLAGS "-Wl,-rpath,'$$ORIGIN'")

    set(INCLUDE_DIRS 
        ${APP_DIR}/include
        ${APP_DIR}/include/stelgic
        ${APP_DIR}/include/third_party
        ${APP_DIR}/include/third_party/stduuid
        ${APP_DIR}/include/third_party/stduuid/include
    )

    if(WIN32)
        set(LIBRARY_DIRS ${APP_DIR}/lib/win64 ${APP_DIR}/bin)
        SET(LIBS stelgic.lib g3log jsoncpp arrow arrow_dataset arrow_acero)
    elseif(UNIX)
        set(LIBRARY_DIRS ${APP_DIR}/lib/unix ${APP_DIR}/bin)
        SET(LIBS m stdc++ stelgic g3log arrow arrow_dataset arrow_acero jsoncpp dl uuid)
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
    
    set(PROCESSOR_LIB_DIR ${CMAKE_INSTALL_LIBDIR}/modules/processors)
    install(DIRECTORY DESTINATION ${PROCESSOR_LIB_DIR})

    install(TARGETS ${TARGET_PROCESSOR} 
        LIBRARY DESTINATION ${PROCESSOR_LIB_DIR}
        ARCHIVE DESTINATION ${PROCESSOR_LIB_DIR}
        RUNTIME DESTINATION ${PROCESSOR_LIB_DIR}
    )

endmacro()

