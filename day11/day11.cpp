#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include <cstdint>

struct Pos {
  uint64_t row{};
  uint64_t col{};

  uint64_t distance(const Pos& other) const {
    return std::max(row, other.row) - std::min(row, other.row) + std::max(col, other.col) - std::min(col, other.col);
  }
};

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);
public:
  uint64_t lengths(uint64_t expansion) const {
    constexpr auto isRowEmpty = [](const auto& grid, unsigned row) {
      return std::ranges::all_of(grid[row], [](const auto c) { return c == '.'; });
    };
    constexpr auto isColEmpty = [](const auto& grid, unsigned col) -> bool {
      for (size_t row = 0; row < grid.size(); row++) {
        if (grid[row][col] != '.')
          return false;
      }
      return true;
    };

    std::vector<Pos> galaxies{};
    std::vector<uint64_t> emptyRows{};
    std::vector<uint64_t> emptyCols{};
    for (size_t row = 0; row < grid.size(); row++) {
      if (isRowEmpty(grid, row)) {
        emptyRows.push_back(row);
      }
    }
    for (size_t col = 0; col < grid.front().size(); col++) {
      if (isColEmpty(grid, col)) {
        emptyCols.push_back(col);
      }
    }
    for (size_t row = 0; row < grid.size(); row++) {
      for (size_t col = 0; col < grid[row].size(); col++) {
        if (grid[row][col] == '#') {
          galaxies.push_back({row + std::ranges::count_if(emptyRows, [row](const auto i) { return i < row; }) * (expansion - 1), col + std::ranges::count_if(emptyCols, [col](const auto i) { return i < col; }) * (expansion - 1)});
        }
      }
    }

    uint64_t length{};
    for (size_t i = 0; i < galaxies.size(); i++) {
      for (size_t j = i + 1; j < galaxies.size(); j++) {
        length += galaxies[i].distance(galaxies[j]);
      }
    }
    return length;
  }
private:
  std::vector<std::vector<char>> grid{};
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line{};
  while (std::getline(is, line)) {
    grid.grid.push_back({});
    for (const auto c : line) {
      grid.grid.back().push_back(c);
    }
  }
  return is;
}

int main() {
  Grid grid{};
  std::cin >> grid;
  std::cout << std::format("Part 1 result = {}\n", grid.lengths(2));
  std::cout << std::format("Part 2 result = {}\n", grid.lengths(1'000'000));
  return 0;
}
