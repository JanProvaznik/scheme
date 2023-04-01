cmake_minimum_required(VERSION 3.0.0)
project(Scheme VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

# add all header files in src directory
file(GLOB_RECURSE HEADER_FILES src/*.h)
add_executable(Scheme src/main.cpp  src/reader.h src/printer.h src/datatypes/types.h src/datatypes/types.cpp src/reader.cpp src/printer.cpp src/datatypes/environment.h src/datatypes/environment.cpp src/util.h src/datatypes/closure.h src/util.cpp src/evaluator.cpp src/evaluator.h)

add_executable(tests tests/tests.cpp  src/reader.h src/printer.h src/datatypes/types.h src/datatypes/types.cpp src/reader.cpp src/printer.cpp src/datatypes/environment.h src/datatypes/environment.cpp src/util.h src/datatypes/closure.h src/util.cpp src/evaluator.cpp src/evaluator.h)
#include_directories(${CMAKE_CURRENT_BINARY_DIR}/external_includes)
add_test(NAME tests COMMAND tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
