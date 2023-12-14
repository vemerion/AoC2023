#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>

struct Pos {
  int row{};
  int col{};
  auto operator<=>(const Pos&) const = default;
};

Pos operator+(const Pos& p1, const Pos& p2) {
  return {p1.row + p2.row, p1.col + p2.col};
}

constexpr Pos north = {-1, 0};
constexpr Pos east = {0, 1};
constexpr Pos south = {1, 0};
constexpr Pos west = {0, -1};
constexpr std::array cardinals{north, west, south, east};

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);
public:
  unsigned load() const {
    unsigned result{};
    for (unsigned row = 0; row < grid.size(); row++) {
      for (unsigned col = 0; col < grid[row].size(); col++) {
        if (grid[row][col] != 'O') {
          continue;
        }
        result += grid.size() - row;
      }
    }

    return result;
  }

  Grid tilt(const Pos& direction) const {
    auto copy = *this;
    for (unsigned row = direction == south ? copy.grid.size() - 1 : 0; row < copy.grid.size(); row += direction == south ? -1 : 1) {
      for (unsigned col = direction == east ? copy.grid[row].size() - 1 : 0; col < copy.grid[row].size(); col += direction == east ? -1: 1) {
        if (copy.grid[row][col] != 'O') {
          continue;
        }
        auto moved = Pos(row, col);
        for (; copy.inside(moved + direction) && copy.at(moved + direction) == '.'; moved = moved + direction)
          ;
        copy.grid[row][col] = '.';
        copy.grid[moved.row][moved.col] = 'O';
      }
    }

    return copy;
  }

  unsigned cycle(const unsigned cycles) {
    std::vector<Grid> grids{};
    Grid current = *this;
    while (std::ranges::find(grids, current) == grids.end()) {
      grids.push_back(current);
      for (const auto& direction : cardinals) {
        current = std::move(current.tilt(direction));
      }
    }
    std::vector<unsigned> sequence{};
    for (auto itr = std::ranges::find(grids, current); itr != grids.end(); ++itr) {
      sequence.push_back(itr->load());
    }
    const unsigned period = sequence.size();
    const unsigned initial = grids.size() - period;
    const unsigned index = cycles - (initial + period * ((cycles - initial) / period));
    return sequence[index];
  }

  auto operator<=>(const Grid&) const = default;
  
private:
  bool inside(const Pos& p) const {
    return p.row >= 0 && p.col >= 0 && std::cmp_less(p.row, grid.size()) && std::cmp_less(p.col, grid[p.row].size());
  }
  
  char at(const Pos& p) const {
    return grid[p.row][p.col];
  }

  std::vector<std::string> grid{};
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line{};
  while (is >> line) {
    grid.grid.push_back(std::move(line));
  }
  return is;
}

int main() {
  Grid grid{};
  std::cin >> grid;
  std::cout << std::format("Part 1 result = {}\n", grid.tilt(north).load());
  std::cout << std::format("Part 2 result = {}\n", grid.cycle(1000000000u));
  return 0;
}
