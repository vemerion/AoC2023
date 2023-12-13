#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cassert>

using Lines = std::vector<std::vector<char>>;

struct ReflectionLine {
  unsigned row{};
  unsigned col{};

  unsigned combine() const {
    return row * 100 + col;
  }
};

ReflectionLine operator+(const ReflectionLine& left, const ReflectionLine& right) {
  return {left.row + right.row, left.col + right.col};
}

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);
public:
  ReflectionLine reflect(unsigned count) const {
    constexpr auto reflecting = [](const Lines& lines, unsigned index) -> unsigned {
      unsigned count{};
      for (unsigned i = 0; index - i < lines.size() && index + i + 1 < lines.size(); i++) {
        count += std::ranges::count_if(std::views::zip(lines[index - i], lines[index + i + 1]), [](const auto& pair) { return std::get<0>(pair) != std::get<1>(pair); });
      }
      return count;
    };

    for (unsigned i = 0; i < rows.size() - 1; i++) {
      if (reflecting(rows, i) == count) {
        return {i + 1, 0};
      }
    }

    for (unsigned i = 0; i < cols.size() - 1; i++) {
      if (reflecting(cols, i) == count) {
        return {0, i + 1};
      }
    }
    assert(false);
  }
private:
  Lines rows{};
  Lines cols{};
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line{};
  while (std::getline(is, line) && !line.empty()) {
    grid.rows.push_back({});
    for (const auto c : line) {
      grid.rows.back().push_back(c);
    }
  }
  for (unsigned i = 0; i < grid.rows[0].size(); i++) {
    grid.cols.push_back({});
    for (unsigned j = 0; j < grid.rows.size(); j++) {
      grid.cols.back().push_back(grid.rows[j][i]);
    }
  }
  return is;
}

unsigned result(const std::vector<Grid>& grids, unsigned count) {
  return std::ranges::fold_left(grids | std::views::transform([count](const auto& grid) { return grid.reflect(count); }), ReflectionLine{0, 0}, std::plus()).combine();
}

int main() {
  std::vector<Grid> grids{};
  Grid grid{};
  while (std::cin >> grid) {
    grids.push_back(std::move(grid));
  }
  grids.push_back(std::move(grid));

  std::cout << std::format("Part 1 result = {}\n", result(grids, 0));
  std::cout << std::format("Part 2 result = {}\n", result(grids, 1));
}
