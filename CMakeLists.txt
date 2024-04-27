cmake_minimum_required(VERSION "3.15")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

project(HNPI_STU_AL)

include_directories(include/hv)
add_compile_definitions(HV_STATIC HV_STATICLIB)
add_compile_options(-source-charset:utf-8)

add_executable(${PROJECT_NAME}
    main.cpp
    def.h
    logger.hpp
    utils.hpp
    cli.hpp
    service_manager.hpp
    auth.hpp
)

target_link_directories(${PROJECT_NAME} PRIVATE lib)
target_link_libraries(${PROJECT_NAME} PRIVATE hv_static.lib)
