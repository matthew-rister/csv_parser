cmake_minimum_required (VERSION 3.8)
project ("csv_parser")

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable (csv_parser  "main.cpp" "src/csv_parser.cpp")