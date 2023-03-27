cmake_minimum_required(VERSION 3.0.0)
project(Scheme VERSION 0.1.0)

include(CTest)
enable_testing()


# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)

# include doctest
include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY "https://github.com/onqtam/doctest"
    GIT_TAG "v2.4.9"
)
FetchContent_MakeAvailable(doctest)
#add_subdirectory(src)
# add all header files in src directory
file(GLOB_RECURSE HEADER_FILES src/*.h)
add_executable(Scheme src/main.cpp  src/reader.h src/printer.h src/datatypes/types.h src/datatypes/types.cpp src/reader.cpp src/printer.cpp src/environment.h src/environment.cpp src/util.h src/datatypes/Closure.cpp src/datatypes/Closure.h src/util.cpp)

add_executable(tests tests/reader_test.cpp src/reader.cpp)
target_link_libraries(tests doctest)
#include_directories(${CMAKE_CURRENT_BINARY_DIR}/external_includes)
add_test(NAME tests COMMAND tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
