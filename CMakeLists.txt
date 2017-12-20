cmake_minimum_required (VERSION 3.1)
project (wordplay)

find_package(PkgConfig)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)

add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/verbly)

include_directories(
  vendor/verbly/lib
  vendor/libtwittercpp/src
  ${yaml-cpp_INCLUDE_DIRS})

add_executable(wordplay wordplay.cpp)
set_property(TARGET wordplay PROPERTY CXX_STANDARD 11)
set_property(TARGET wordplay PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(wordplay verbly twitter++ ${yaml-cpp_LIBRARIES})
