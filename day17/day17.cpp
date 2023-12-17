#include <iostream>
#include <format>
#include <vector>
#include <array>
#include <list>
#include <limits>
#include <utility>
#include <cassert>
#include <queue>

using Direction = std::pair<int, int>;

constexpr Direction north{-1, 0};
constexpr Direction east{0, 1};
constexpr Direction south{1, 0};
constexpr Direction west{0, -1};
constexpr std::array cardinals{north, east, south, west};

struct Node {
  unsigned cost{};
  bool visited{};
  bool destination{};
  unsigned value{std::numeric_limits<unsigned>::max()};
  std::vector<Node*> neighbors{};
};

using Graph = std::vector<std::vector<std::vector<Node>>>;

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);
public:
  unsigned path(const bool part1) {
    auto& graph = part1 ? regular : ultra;

    struct Element {
      Node* node{};
      unsigned value{};

      Element(Node* node) : node(node), value(node->value) {}
    };
    constexpr auto cmp = [](const Element& e1, const Element& e2) { return e1.value > e2.value; };
    std::priority_queue<Element, std::vector<Element>, decltype(cmp)> unvisited{};
    for (unsigned i = 0; i < cardinals.size(); i++) {
      at(graph, 0, 0, i, 0).value = 0;
      unvisited.push(&at(graph, 0, 0, i, 0));
    }

    while (true) {
      auto [node, value] = unvisited.top();
      unvisited.pop();
      if (node->visited) {
        continue;
      }
      for (auto* neighbor : node->neighbors) {
        if (neighbor->visited) {
          continue;
        }
        if (node->value + neighbor->cost < neighbor->value) {
          neighbor->value = node->value + neighbor->cost;
          unvisited.push(neighbor);
        }
      }
      node->visited = true;

      if (node->destination) {
        return node->value;
      }
    }

    assert(false);
  }

private:
  std::vector<std::vector<std::vector<Node>>> regular{};
  std::vector<std::vector<std::vector<Node>>> ultra{};

  bool inside(unsigned row, unsigned col) const {
    return row < regular.size() && col < regular[row].size();
  }

  Node& at(Graph& graph, unsigned row, unsigned col, unsigned direction, unsigned line) {
    return graph[row][col][direction * (graph[row][col].size() / cardinals.size()) + line];
  }

  void build() {
    static constexpr std::array nearby{0ull, 1ull, cardinals.size() - 1ull};
    const auto build = [this](const bool part1) {
      auto& graph = part1 ? regular : ultra;
      for (unsigned row = 0; row < graph.size(); row++) {
        for (unsigned col = 0; col < graph[row].size(); col++) {
          for (unsigned dir = 0; dir < cardinals.size(); dir++) {
            for (unsigned line = 0; line < graph[row][col].size() / cardinals.size(); line++) {
              if (row == graph.size() - 1 && col == graph[row].size() - 1 && (part1 || line > 3)) {
                at(graph, row, col, dir, line).destination = true;
              }
              for (unsigned i = 0; i < nearby.size(); i++) {
                unsigned d = (dir + nearby[i]) % cardinals.size();
                const auto& [r, c] = cardinals[d];
                if (inside(row + r, col + c) && ((part1 && (d != dir || line != 3)) || (!part1 && ((d == dir && line != 10) || (d != dir && line > 3))))) {
                  at(graph, row, col, dir, line).neighbors.push_back(&at(graph, row + r, col + c, d, dir == d ? line + 1 : 1));
                }
              }
            }
          }
        }
      }
    };

    build(true);
    build(false);
  }
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line{};
  while (is >> line) {
    grid.regular.push_back({});
    grid.ultra.push_back({});
    for (const auto c : line) {
      grid.regular.back().push_back({});
      grid.ultra.back().push_back({});
      for (unsigned i = 0; i < 4 * 4; i++) {
        grid.regular.back().back().push_back(Node(c - '0'));
      }
      for (unsigned i = 0; i < 11 * 4; i++) {
        grid.ultra.back().back().push_back(Node(c - '0'));
      }
    }
  }

  grid.build();

  return is;
}

int main() {
  Grid grid{};
  std::cin >> grid;
  std::cout << std::format("Part 1 result = {}\n", grid.path(true));
  std::cout << std::format("Part 2 result = {}\n", grid.path(false));
  return 0;
}
