cmake_minimum_required (VERSION 3.1)
project (wordplay)

find_package(PkgConfig)
pkg_check_modules(sqlite3 sqlite3 REQUIRED)

add_subdirectory(vendor/twitcurl/libtwitcurl)
include_directories(vendor/twitcurl/libtwitcurl)

add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)
include_directories(vendor/yaml-cpp/include)

add_subdirectory(vendor/verbly)
include_directories(vendor/verbly/lib)

add_executable(wordplay wordplay.cpp)
set_property(TARGET wordplay PROPERTY CXX_STANDARD 11)
set_property(TARGET wordplay PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(wordplay verbly ${sqlite3_LIBRARIES} yaml-cpp twitcurl curl)
