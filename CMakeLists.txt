cmake_minimum_required(VERSION 3.2)

set(NAME perceptron)
PROJECT (${NAME})
set(PROJECT ${NAME} )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set (CMAKE_CXX_STANDARD 17)
set( HEADERS ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR} "*.cpp")

include_directories( ${PROJECT_SOURCE_DIR} )

set( REQUIRED_LIBRARIES )

add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} ${REQUIRED_LIBRARIES} )

