cmake_minimum_required(VERSION 2.8)

project(rproject)

add_executable(
    ${PROJECT_NAME}

    pimpl.hpp
    pimpl_impl.hpp

    variant.cpp
    variant.hpp

    main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
