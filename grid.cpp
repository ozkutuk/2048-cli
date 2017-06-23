#include <iostream>
#include "grid.hpp"
#include "node.hpp"
#include "direction.hpp"
#include <vector>
#include <cstddef> // std::size_t
#include <random>

class Grid::iterator {
public:
  explicit iterator(pointer _node_p) : node_p(_node_p) {}
  // why not const refs?
  bool operator==(iterator other) const {return node_p == other.node_p;}
  bool operator!=(iterator other) const {return !(*this == other);}
  reference operator*() const {return *node_p;}
  iterator & operator++() {
    if (node_p -> right == nullptr) {
      if (node_p == nullptr)  return *this;

      Node *tmp = node_p;
      while (tmp -> left != nullptr) {
        tmp = tmp -> left;
      }
      tmp = tmp -> down;
      node_p = tmp;
    }
    else {
      node_p = node_p -> right;
    }
    return *this;
  }
  iterator operator++(int) {
    iterator retval = *this;
    ++(*this);
    return retval;
  }

private:
  pointer node_p;
};

Grid::iterator Grid::begin() {
  return iterator(&grid2d[0][0]);
}

Grid::iterator Grid::end() {
  return iterator(nullptr);
}

Grid::Grid(unsigned n)
  : grid2d(std::vector<std::vector<Node> >(n))
{
  for (int i = 0; i < n; ++i) {
    std::vector<Node> row(n);
    for (int j = 0; j < n; ++j) {
      row[j] = Node();
    }
    grid2d[i] = row;
  }
}

Grid::~Grid() { }

void Grid::bindNodes() {
  // assume n >= 2
  using std::size_t;
  size_t gridSize = grid2d.size();

  // bind left-right
  // treat first and last nodes independently
  // first nodes in their respective rows
  for (size_t row_i = 0; row_i < gridSize; ++row_i) {
    grid2d[row_i][0].bindNode(Direction::right, &grid2d[row_i][1]);
  }
  // nodes in cols = [1, gridSize-1)
  for (size_t row_i = 0; row_i < gridSize; ++row_i) {
    for (size_t col_i = 1; col_i < gridSize - 1; ++col_i) {
      grid2d[row_i][col_i].bindNode(Direction::left, &grid2d[row_i][col_i-1]);
      grid2d[row_i][col_i].bindNode(Direction::right, &grid2d[row_i][col_i+1]);
    }
  }
  // last nodes in their respective rows
  for (size_t row_i = 0; row_i < gridSize; ++row_i) {
    grid2d[row_i][gridSize-1].bindNode(Direction::left, &grid2d[row_i][gridSize-2]);
  }

  //bind up-down
  // treat first and last nodes independently
  // first nodes in their respective columns
  for (size_t col_i = 0; col_i < gridSize; ++col_i) {
    grid2d[0][col_i].bindNode(Direction::down, &grid2d[1][col_i]);
  }
  // nodes in rows = [1, gridSize-1)
  // TODO: might swap two for-loops for better caching
  for (size_t col_i = 0; col_i < gridSize; ++col_i) {
    for (size_t row_i = 1; row_i < gridSize - 1; ++row_i) {
      grid2d[row_i][col_i].bindNode(Direction::up, &grid2d[row_i-1][col_i]);
      grid2d[row_i][col_i].bindNode(Direction::down, &grid2d[row_i+1][col_i]);
    }
  }
  // last nodes in their respective columns
  for (size_t col_i = 0; col_i < gridSize; ++col_i) {
    grid2d[gridSize-1][col_i].bindNode(Direction::up, &grid2d[gridSize-2][col_i]);
  }
}

std::vector<Node *> Grid::getEmptyNodes() {
  std::vector<Node *> emptyNodes;
  for (auto it = begin(); it != end(); ++it) {
    if ((*it).isEmpty())
      emptyNodes.push_back(&(*it));
  }
  return emptyNodes;
}

bool Grid::isFull() {
  for (auto it = begin(); it != end(); ++it) {
    if ((*it).isEmpty())
      return false;
  }
  return true;
}

int Grid::getRandomVal() const {
  static std::random_device rd;
  static std::mt19937 rng(rd());
  //static std::mt19937 rng;
  static std::uniform_int_distribution<int> dist(1,2);
  return dist(rng) * 2; // 2 or 4
}

void Grid::populate() {
  static std::random_device rd;
  static std::mt19937 rng(rd());
  std::vector<Node *> emptyNodes = getEmptyNodes();
  std::uniform_int_distribution<int> dist(0, emptyNodes.size()-1);
  int randIndex = dist(rng);
  //int randIndex = 6;
  //std::cout << randIndex << std::endl;
  //std::cout << "prev val: " << emptyNodes[randIndex]->val << std::endl;
  emptyNodes[randIndex]->val = getRandomVal();
  //std::cout << "end val: " << emptyNodes[randIndex]->val << std::endl;
}
