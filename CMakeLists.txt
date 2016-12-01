cmake_minimum_required (VERSION 3.1)
project (wordplay)

add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)
add_subdirectory(vendor/verbly)

include_directories(vendor/verbly/lib vendor/libtwittercpp/src vendor/yaml-cpp/include)

add_executable(wordplay wordplay.cpp)
set_property(TARGET wordplay PROPERTY CXX_STANDARD 11)
set_property(TARGET wordplay PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(wordplay verbly twitter++ yaml-cpp)
