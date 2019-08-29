project(object_property_tree)
cmake_minimum_required(VERSION 3.12)

## build settings
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(OBJECT_PROPERTY_TREE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

# CPack package information
set(CPACK_PACKAGE_DESCRIPTION "A proberty tree capable of holding object pointers.")

## C++14 standard is required by JWT header library.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Setting flags and definitions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories(
        src
        tests
)

add_subdirectory(tests)
add_subdirectory(src)
