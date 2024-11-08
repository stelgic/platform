# macro to generate cmake for each subfolder

macro( add_metrics_module name)
    SET(TARGET_METRICS ${name}) 
    PROJECT(${TARGET_METRICS}) 

    # build shared module
    add_definitions(-DBUILD_SHARED_LIBS=ON)
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
        SET(LIBS stelgic g3log jsoncpp arrow arrow_dataset arrow_acero)
    elseif(UNIX)
        set(LIBRARY_DIRS ${APP_DIR}/lib/unix ${APP_DIR}/bin)
        SET(LIBS m stdc++ stelgic g3log pthread arrow arrow_dataset jsoncpp dl uuid)
    endif()

    file(GLOB TARGET_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/kernels/*.hpp
    )

    # Add executable called "craftor" that is built from the source files
    add_library(${TARGET_METRICS} SHARED )

    # add sources to the library target
    target_sources(
        ${TARGET_METRICS}
        PRIVATE ${TARGET_SRC}
    )

    # set the include directories
    target_include_directories(
        ${TARGET_METRICS}
        PUBLIC ${INCLUDE_DIRS}
    )

    target_link_directories(
        ${TARGET_METRICS}
        PUBLIC ${LIBRARY_DIRS}
    )

    # Link the executable to the library.
    target_link_libraries (${TARGET_METRICS} PRIVATE ${LIBS})
    set_target_properties(${TARGET_METRICS} PROPERTIES FOLDER "metrics")

    set(METRICS_LIB_DIR ${CMAKE_INSTALL_LIBDIR}/modules/metrics)
    install(DIRECTORY DESTINATION ${METRICS_LIB_DIR})

    install(TARGETS ${TARGET_METRICS} 
        LIBRARY DESTINATION ${METRICS_LIB_DIR}
        ARCHIVE DESTINATION ${METRICS_LIB_DIR}
        RUNTIME DESTINATION ${METRICS_LIB_DIR}
    )

endmacro()

