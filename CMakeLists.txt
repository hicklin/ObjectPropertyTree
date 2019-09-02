project(object_property_tree)
cmake_minimum_required(VERSION 3.12)

## build settings
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
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

## Finding the boost library.
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS system thread REQUIRED)

include_directories(
        include
        src
        tests
        ${Boost_INCLUDE_DIRS}
)

add_subdirectory(include)
add_subdirectory(src)

#if (BUILD_TESTS)
    message("building tests")
    add_subdirectory(tests)
#endif ()
