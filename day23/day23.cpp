#include <iostream>
#include <format>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <cassert>
#include <algorithm>
#include <utility>

struct Pos {
  int row{};
  int col{};
  auto operator<=>(const Pos&) const = default;
};

Pos operator+(const Pos& p1, const Pos& p2) {
  return {p1.row + p2.row, p1.col + p2.col};
}

constexpr Pos north{-1, 0};
constexpr Pos east{0, 1};
constexpr Pos south{1, 0};
constexpr Pos west{0, -1};
const std::vector cardinals{north, east, south, west};

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);
public:
  unsigned path(const bool part1) const {
    const auto findNeighbors = [this, part1](const Pos& pos) -> std::vector<std::pair<Pos, unsigned>> {
      struct Element {
        Pos pos{};
        unsigned cost{};
      };
      std::vector<std::pair<Pos, unsigned>> result{};
      std::list<Element> list{{pos, 0}};
      std::set<Pos> visited{pos};
      while (!list.empty()) {
        const auto [p, cost] = list.front();
        list.pop_front();
        if (isNode(p) && p != pos) {
          result.push_back({p, cost});
          continue;
        }
        for (const auto& dir : directions(p, part1)) {
          if (const auto next = p + dir; at(next) != '#' && !visited.contains(next)) {
            list.push_back({next, cost + 1});
            visited.insert(next);
          }
        }
      }
      return result;
    };
    struct Node {
      Pos pos{};
      std::vector<std::pair<Node*, unsigned>> neighbors{};
    };
    std::map<Pos, Node> graph{};
    for (unsigned row = 0; row < grid.size(); row++) {
      for (unsigned col = 0; col < grid[row].size(); col++) {
        if (const Pos p = Pos(row, col); at(p) != '#' && isNode(p)) {
          graph[p] = {p, {}};
        }
      }
    }
    for (auto& [pos, node] : graph) {
      for (const auto& [neighbor, cost] : findNeighbors(pos)) {
        node.neighbors.push_back({&graph[neighbor], cost});
      }
    }
    struct Element {
      std::set<Pos> path{};
      Pos pos{};
      unsigned cost{};
    };
    std::list<Element> list{{{}, start, 0}};

    unsigned result{};
    while (!list.empty()) {
      const auto [path, pos, cost] = std::move(list.back());
      list.pop_back();
      
      if (pos == end) {
        if (cost > result) {
          result = cost;
        }
        continue;
      }

      for (const auto& [neighbor, c] : graph[pos].neighbors) {
        if (!path.contains(neighbor->pos)) {
          auto nextPath = path;
          nextPath.insert(neighbor->pos);
          list.push_back({std::move(nextPath), neighbor->pos, cost + c});
        }
      }
    }

    return result;
  }

private:
  std::vector<std::vector<char>> grid{};
  Pos start{};
  Pos end{};

  char at(const Pos& pos) const {
    return grid[pos.row][pos.col];
  }

  bool isNode(const Pos& pos) const {
    return pos == start || pos == end || std::ranges::count_if(cardinals, [&pos, this](const auto& d) { return at(pos + d) != '#'; }) > 2;
  }

  std::vector<Pos> directions(const Pos& pos, const bool part1) const {
    if (pos == start) {
      return {south};
    } else if (pos == end) {
      return {north};
    }
    if (!part1) {
      return cardinals;
    }
    switch (at(pos)) {
      case '.':
        return cardinals;
      case '^':
        return {north};
      case '>':
        return {east};
      case 'v':
        return {south};
      case '<':
        return {west};
      default:
        assert(false);
    }
  }
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line{};
  while (is >> line) {
    grid.grid.push_back({});
    for (const auto c : line) {
      grid.grid.back().push_back(c);
    }
  }

  for (unsigned col = 0; col < grid.grid[0].size(); col++) {
    if (grid.grid[0][col] == '.') {
      grid.start = Pos(0, col);
    }
    if (grid.grid[grid.grid.size() - 1][col] == '.') {
      grid.end = Pos(grid.grid.size() - 1, col);
    }
  }

  return is;
}

int main() {
  Grid grid{};
  std::cin >> grid;
  std::cout << std::format("Part 1 result = {}\n", grid.path(true));
  std::cout << std::format("Part 2 result = {}\n", grid.path(false));
  return 0;
}
