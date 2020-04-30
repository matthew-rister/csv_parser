cmake_minimum_required (VERSION 3.8)
project ("csv_parser")

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories("${CMAKE_SOURCE_DIR}/extern" "${CMAKE_SOURCE_DIR}/src")

add_executable (csv_parser  "main.cpp" "test/csv_parser_test.cpp")

set_property(TARGET csv_parser PROPERTY CXX_STANDARD 17)