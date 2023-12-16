#include <iostream>
#include <format>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <utility>

struct Pos {
  const int row{};
  const int col{};
  auto operator<=>(const Pos&) const = default;
};

Pos operator+(const Pos& p1, const Pos& p2) {
  return {p1.row + p2.row, p1.col + p2.col};
}

constexpr Pos north{-1, 0};
constexpr Pos east{0, 1};
constexpr Pos south{1, 0};
constexpr Pos west{0, -1};

struct Light {
  const Pos pos{};
  const Pos dir{};
  auto operator<=>(const Light&) const = default;

  Light move() const {
    return {pos + dir, dir};
  }

  Light redirect(const Pos& direction) const {
    return {pos + direction, direction};
  }

  Light rotate(const bool clockwise) const {
    const auto factor = clockwise ? -1 : 1;
    const Pos direction{-dir.col * factor, dir.row * factor};
    return {pos + direction, direction};
  }
};

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);
public:
  unsigned energize(const Light& start) const {
    std::set<Light> found{start};
    std::list<Light> list{start};

    while (!list.empty()) {
      const auto light = list.front();
      list.pop_front();
      for (const auto& l : move(light)) {
        if (!found.contains(l)) {
          found.insert(l);
          list.push_back(l);
        }
      }
    }

    std::set<Pos> energized{};
    for (const auto& l : found) {
      energized.insert(l.pos);
    }
    return energized.size();
  }

  unsigned energizeLargest() const {
    unsigned result{};

    for (unsigned row = 0; row < grid.size(); row++) {
      result = std::max(result, energize({Pos(row, 0), east}));
      result = std::max(result, energize({Pos(row, grid[row].size() - 1), west}));
    }

    for (unsigned col = 0; col < grid.front().size(); col++) {
      result = std::max(result, energize({Pos(0, col), south}));
      result = std::max(result, energize({Pos(grid.size() - 1, col), north}));
    }

    return result;
  }

private:
  std::vector<std::string> grid{};
  bool inside(const Light& l) const {
    return l.pos.row >= 0 && l.pos.col >= 0 && std::cmp_less(l.pos.row, grid.size()) && std::cmp_less(l.pos.col, grid[l.pos.row].size());
  }

  char at(const Light& l) const {
    return grid[l.pos.row][l.pos.col];
  }

  std::vector<Light> move(const Light& light) const {
    std::vector<Light> result{};
    const auto add = [this, &result](const auto& l) {
      if (inside(l)) {
        result.push_back(l);
      }
    };
    if (!inside(light)) {
      return result;
    }

    const auto c = at(light);
    if (light.dir.col != 0 && c == '|') {
      add(light.redirect(north));
      add(light.redirect(south));
    } else if (light.dir.row != 0 && c == '-') {
      add(light.redirect(east));
      add(light.redirect(west));
    } else if ((c == '\\' && (light.dir == east || light.dir == west)) || (c == '/' && (light.dir == north || light.dir == south))) {
      add(light.rotate(true));
    } else if ((c == '\\' && (light.dir == north || light.dir == south)) || (c == '/' && (light.dir == east || light.dir == west))) {
      add(light.rotate(false));
    } else {
      add(light.move());
    }
    return result;
  }
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line;
  while (is >> line) {
    grid.grid.push_back(std::move(line));
  }
  return is;
}

int main() {
  Grid grid{};
  std::cin >> grid;
  std::cout << std::format("Part 1 result = {}\n", grid.energize({{0, 0}, east}));
  std::cout << std::format("Part 2 result = {}\n", grid.energizeLargest());
  return 0;
}
