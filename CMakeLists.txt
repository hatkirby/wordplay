cmake_minimum_required (VERSION 3.1)
project (wordplay)

find_package(PkgConfig)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)
pkg_check_modules(mastodonpp mastodonpp REQUIRED)

add_subdirectory(vendor/verbly)

include_directories(
  vendor/verbly/lib
  ${mastodonpp_INCLUDE_DIRS}
  ${yaml-cpp_INCLUDE_DIRS})

link_directories(
  ${mastodonpp_LIBRARY_DIRS}
  ${yaml-cpp_LIBRARY_DIRS})

add_executable(wordplay wordplay.cpp)
set_property(TARGET wordplay PROPERTY CXX_STANDARD 17)
set_property(TARGET wordplay PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(wordplay verbly ${mastodonpp_LIBRARIES} ${yaml-cpp_LIBRARIES})
