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

## Finding the wxWidgets library.
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

include_directories(
        src
        ${wxWidgets_INCLUDE_DIRS}
)

set(LIB_SOURCES
        src/propertytree.h
        src/trace.hpp
        src/variant.cc
        src/variantpropertytree.cc
        )

add_library(object_property_tree SHARED ${LIB_SOURCES})
target_link_libraries(object_property_tree ${wxWidgets_LIBRARIES})

## set the shared library soname
set_target_properties(object_property_tree PROPERTIES
        VERSION ${OBJECT_PROPERTY_TREE_VERSION}
        SOVERSION ${VERSION_MAJOR})

## install the shared library
install(TARGETS object_property_tree
        LIBRARY DESTINATION lib)

