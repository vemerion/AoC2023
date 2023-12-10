#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <cassert>
#include <algorithm>
#include <ranges>
#include <array>
#include <list>

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
constexpr std::array cardinals{north, east, south, west};
const std::map<char, std::vector<Pos>> directions{{'S', {north, east, south, west}}, {'|', {north, south}}, {'-', {east, west}}, {'L', {north, east}}, {'J', {north, west}}, {'7', {south, west}}, {'F', {south, east}}};

class Squeeze {
public:
  Squeeze(const Pos& p1, const Pos& p2) : p1(std::min(p1, p2)), p2(std::max(p1, p2)) {
  }
  auto operator<=>(const Squeeze&) const = default;

  const Pos& first() const {
    return p1;
  }

  const Pos& second() const {
    return p2;
  }

  bool isVertical() const {
    return p1.row == p2.row;
  }

  std::vector<Squeeze> forward() const {
    if (isVertical()) {
      return {Squeeze{p1 + north, p2 + north}, Squeeze{p1 + north + east, p2}, Squeeze{p1, p2 + north + west}, Squeeze{p1 + south, p2 + south}, Squeeze{p1 + south + east, p2}, Squeeze{p1, p2 + south + west}};
    } else {
      return {Squeeze{p1 + east, p2 + east}, Squeeze{p1 + east + south, p2}, Squeeze{p1, p2 + east + north}, Squeeze{p1 + west, p2 + west}, Squeeze{p1 + west + south, p2}, Squeeze{p1, p2 + west + north}};
    } 
  }

  std::vector<Pos> opening() const {
    if (isVertical()) {
      return {Pos{p1 + north}, Pos{p1 + south}, Pos{p2 + north}, Pos{p2 + south}};
    } else {
      return {Pos{p1 + east}, Pos{p1 + west}, Pos{p2 + east}, Pos{p2 + west}};
    }
  }
private:
  const Pos p1{};
  const Pos p2{};
};

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);
using Cache = std::map<Pos, bool>;
public:
  std::set<Pos> findPath() const {
    for (const auto& direction : directions.at('S')) {
      const auto current = start + direction;
      if (!inside(current) || at(current) == '.' || !isConnected(start, current)) {
        continue;
      }
      return findPath(current);
    }
    assert(false);
  }

  unsigned enclosed() const {
    const auto path = findPath();
    Cache cache{};
    for (size_t row = 0; row < grid.size(); row++) {
      for (size_t col = 0; col < grid[row].size(); col++) {
        enclosed(cache, path, Pos(row, col));
      }
    }
    return std::ranges::count_if(cache, [](const auto& pair) { return pair.second; });
  }

private:
  std::vector<std::vector<char>> grid{};
  Pos start{};

  bool inside(const Pos& pos) const {
    return pos.row >= 0 && pos.col >= 0 && std::cmp_less(pos.row, grid.size()) && std::cmp_less(pos.col, grid[pos.row].size());
  }

  char at(const Pos& pos) const {
    return grid[pos.row][pos.col];
  }

  bool isConnected(const Pos& p1, const Pos& p2) const {
    const auto leadsTo = [this](const Pos& from, const Pos& to) {
      return std::ranges::any_of(directions.at(at(from)) | std::views::transform([&from](const auto& d) { return from + d; }), [&to](const auto& p) { return p == to; });
    };
    return leadsTo(p1, p2) && leadsTo(p2, p1);
  }

  bool isSqueeze(const std::set<Pos>& path, const Pos& p1, const Pos& p2) const {
    return path.contains(p1) && path.contains(p2) && !isConnected(p1, p2);
  }

  std::set<Pos> squeezing(const std::set<Pos>& path, const Pos& current) const {
    std::set<Pos> result{};
    std::set<Squeeze> found{};
    std::list<Squeeze> list{};
    for (const auto& direction : cardinals) {
      const auto adjacent = current + direction;
      for (int i = -1; i <= 1; i += 2) {
        const auto other = adjacent + Pos{direction.row == 0 ? i : 0, direction.col == 0 ? i : 0};
        if (isSqueeze(path, adjacent, other)) {
          list.push_back({adjacent, other});
          found.insert({adjacent, other});
        }
      }
    }

    while (!list.empty()) {
      const auto next = list.front();
      list.pop_front();
      for (const auto& s : next.forward()) {
        if (!found.contains(s) && isSqueeze(path, s.first(), s.second())) {
          found.insert(s);
          list.push_back(s);
        }
      }
      for (const auto& p : next.opening()) {
        if (inside(p) && !path.contains(p)) {
          result.insert(p);
        }
      }
    }

    return result;
  }

  std::set<Pos> findPath(Pos current) const {
    std::set<Pos> result{start, current};
    auto prev = start;
    while (inside(current) && at(current) != '.') {
      for (const auto& direction : directions.at(at(current))) {
        const auto next = current + direction;
        if (next != prev) {
          prev = current;
          current = next;
          result.insert(current);
          break;
        }
      }
      if (current == start) {
        return result;
      }
    }
    return {};
  }

  void enclosed(Cache& cache, const std::set<Pos>& path, Pos pos) const {
    constexpr auto fill = [](Cache& cache, const std::set<Pos>& positions, bool value) {
      for (const auto& p : positions) {
        cache[p] = value;
      }
    };
    if (cache.contains(pos)) {
      return;
    }
    if (path.contains(pos)) {
      return;
    }

    std::set<Pos> found{pos};
    std::list<Pos> list{pos};
    while (!list.empty()) {
      const auto next = list.front();
      list.pop_front();
      if (next.row == 0 || next.col == 0 || std::cmp_equal(next.row, grid.size() - 1) || std::cmp_equal(next.col, grid[next.row].size() - 1)) {
        fill(cache, found, false);
        return;
      }

      for (const auto& p : squeezing(path, next)) {
        if (!found.contains(p)) {
          found.insert(p);
          list.push_back(p);
        }
      }
      for (int row = -1; row <= 1; row++) {
        for (int col = -1; col <= 1; col++) {
          if (const auto p = next + Pos{row, col}; next != p && inside(p) && !path.contains(p) && !found.contains(p)) {
            found.insert(p);
            list.push_back(p);
          }
        }
      }
    }

    fill(cache, found, true);
  }
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line{};
  while (std::getline(is, line)) {
    grid.grid.push_back({});
    for (const auto c : line) {
      grid.grid.back().push_back(c);
      if (c == 'S') {
        grid.start = Pos(grid.grid.size() - 1, grid.grid.back().size() - 1);
      }
    }
  }
  return is;
}

int main() {
  Grid grid{};
  std::cin >> grid;
  
  std::cout << std::format("Part 1 result = {}\n", grid.findPath().size() / 2);
  std::cout << std::format("Part 2 result = {}\n", grid.enclosed());
  return 0;
}
