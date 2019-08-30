#ifndef OBJECT_PROPERTY_TREE_NODE_PATH_H
#define OBJECT_PROPERTY_TREE_NODE_PATH_H

#include <vector>
#include <boost/tokenizer.hpp>

constexpr const char *DEFAULT_SEPARATOR = ".";

// A tree is an addressable set of nodes. objects of type T must have an assignment operator.
typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

/**
 * @brief This class handles a path to a node in the property tree. The path is a vector list of any type.
 * @tparam T The type of the path items.
 */
template<typename T>
class NodePath : public std::vector<T> {
 public:

  /**
   * @brief Create an object to handle path to a node.
   */
  NodePath() = default;

  /**
   * @brief Create an object to handle path to a node.
   * @param node_path The node path to crate from.
   */
  NodePath(const NodePath<T> &node_path) : std::vector<T>(node_path) {}

  /**
   * @brief Converts a string to a list.
   * @param string String to split.
   * @param separator The separator between fields.
   */
  void ToList(const T &string, const char *separator = DEFAULT_SEPARATOR) {
    boost::char_separator<char> sep(separator);
    tokenizer tokens(string, sep);
    for (auto i = tokens.begin(); i != tokens.end(); i++) {
      this->push_back(*i);
    }
  }

  /**
   * @brief Converts NodePath list to string with the default separator.
   * @param string Receives concatenated path elements.
   */
  void ToString(T &string) const {
    if (this->size() > 0) {
      const NodePath &node_path = *this;
      string = node_path[0];
      for (unsigned i = 1; i < this->size(); i++) {
        string += DEFAULT_SEPARATOR;
        string += node_path[i];
      }
    }
  }

  /**
   * @brief Append a path to this path.
   * @param path The path to append.
   * @return The appended path.
   */
  const NodePath<T> &append(const NodePath<T> &path) {
    for (int i = 0; i < int(path.size()); i++) {
      this->push_back(path[i]);
    }
    return *this;
  }
};
#endif //OBJECT_PROPERTY_TREE_NODE_PATH_H
