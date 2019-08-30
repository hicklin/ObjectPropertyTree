#include "catch.hpp"
#include "node_path.h"

TEST_CASE("NodePath") {
  NodePath<std::string> path1;
  NodePath<std::string> path2(path1);

  std::vector<std::string> path1_vector{"This", "is", "path", "one"};
  std::vector<std::string> path2_vector{"This", "is", "path", "two"};
  std::vector<std::string> path_vector{"This", "is", "path", "one", "This", "is", "path", "two"};

  REQUIRE(path1.empty());
  REQUIRE(path2.empty());

  path1.ToList("This.is.path.one");
  REQUIRE(path1 == path1_vector);
  std::string path1_string;
  path1.ToString(path1_string);
  REQUIRE(path1_string == "This.is.path.one");
  REQUIRE(path2.empty());

  path2.ToList("This.is.path.two");
  REQUIRE(path2 == path2_vector);
  auto path = path1.append(path2);
  REQUIRE(path == path_vector);
  std::string path_string;
  path.ToString(path_string);
  REQUIRE(path_string == "This.is.path.one.This.is.path.two");
}

