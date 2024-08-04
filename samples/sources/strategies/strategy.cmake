# macro to generate cmake for each subfolder

macro( add_strategy_module name)
    SET(TARGET_STRATEGY ${name}) 
    PROJECT(${TARGET_STRATEGY}) 

    # build shared module
    add_definitions(-DBUILD_SHARED_LIBS=ON)
    set(LDFLAGS "-Wl,-rpath,'$$ORIGIN'")

    set(INCLUDE_DIRS 
        ${APP_DIR}/include
        ${APP_DIR}/include/qcraftor
        ${APP_DIR}/include/third_party
        ${APP_DIR}/include/third_party/stduuid
        ${APP_DIR}/include/third_party/stduuid/include
    )

    file(GLOB TARGET_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.cpp
    )

    # Add executable called "craftor" that is built from the source files
    add_library(${TARGET_STRATEGY} SHARED )

    # add sources to the library target
    target_sources(
        ${TARGET_STRATEGY}
        PRIVATE ${TARGET_SRC}
    )

    # set the include directories
    target_include_directories(
        ${TARGET_STRATEGY}
        PRIVATE ${TARGET_SRC}
        PUBLIC ${INCLUDE_DIRS}
    )

    target_link_directories(
        ${TARGET_STRATEGY}
        PUBLIC ${LIBRARY_DIRS}
    )

    # Link the executable to the library.
    target_link_libraries (${TARGET_STRATEGY} PRIVATE ${LIBS})
    set_target_properties(${TARGET_STRATEGY} PROPERTIES FOLDER "strategies")

    set(STRATEGY_LIB_DIR ${CMAKE_INSTALL_LIBDIR}/modules/strategies)
    install(DIRECTORY DESTINATION ${STRATEGY_LIB_DIR})

    install(TARGETS ${TARGET_STRATEGY} 
        LIBRARY DESTINATION ${STRATEGY_LIB_DIR}
        ARCHIVE DESTINATION ${STRATEGY_LIB_DIR}
        RUNTIME DESTINATION ${STRATEGY_LIB_DIR}
    )

endmacro()

