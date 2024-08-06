cmake_minimum_required(VERSION 3.10)
project(DON VERSION 1.0)
add_library(don STATIC ${PROJECT_SOURCE_DIR}/src/Directives.cpp ${PROJECT_SOURCE_DIR}/src/parser.cpp)
target_include_directories(don PUBLIC ${PROJECT_SOURCE_DIR}/include)
set_target_properties(don PROPERTIES PREFIX "")

