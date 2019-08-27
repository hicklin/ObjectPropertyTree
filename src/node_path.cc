#include "node_path.h"

template<typename T>
void NodePath<T>::ToList(const T &string, const char *separator) {
  boost::char_separator<char> sep(separator);
  tokenizer tokens(string, sep);
  for (auto i = tokens.begin(); i != tokens.end(); i++) {
    this->push_back(*i);
  }
}

template<typename T>
void NodePath<T>::ToString(T &string) const {
  if (this->size() > 0) {
    const NodePath &node_path = *this;
    string = node_path[0];
    for (unsigned i = 1; i < this->size(); i++) {
      string += DEFAULT_SEPARATOR;
      string += node_path[i];
    }
  }
}

template<typename T>
const NodePath<T> &NodePath<T>::append(const NodePath<T> &path) {
  for (int i = 0; i < int(path.size()); i++) {
    this->push_back(path[i]);
  }
  return *this;
}
