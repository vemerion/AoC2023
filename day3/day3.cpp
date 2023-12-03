#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <cctype>
#include <optional>
#include <utility>
#include <map>
#include <algorithm>
#include <ranges>

class Grid {
friend std::istream& operator>>(std::istream& is, Grid& grid);

public:
  unsigned result(bool part1) const {
    unsigned result{};
    for (unsigned row = 0; row < grid.size(); row++) {
      for (unsigned col = 0; col < grid[row].size(); col++) {
        if (const auto num = part1 ? partNumber(row, col) : gearRatio(row, col); num) {
          result += *num;
        }
      }
    }
    return result;
  }

private:
  char at(unsigned row, unsigned col) const {
    return grid[row][col];
  }

  bool isDigit(unsigned row, unsigned col) const {
    return std::isdigit(static_cast<unsigned char>(at(row, col)));
  }

  bool isInside(unsigned row, unsigned col) const {
    return row < grid.size() && col < grid[row].size();
  }

  bool isPartNumber(unsigned row, unsigned col) const {
    for (int r = -1; r <= 1; r++) {
      for (int c = -1; c <= 1; c++) {
        if (isInside(row + r, col + c) && !isDigit(row + r, col + c) && at(row + r, col + c) != '.') {
          return true;
        }
      }
    }
    return false;
  }

  std::optional<unsigned> partNumber(unsigned row, unsigned col) const {
    if (isInside(row, col - 1u) && isDigit(row, col - 1u))
      return {};

    unsigned result{};
    bool valid{};
    for (; isInside(row, col) && isDigit(row, col); col++) {
      result = result * 10 + (at(row, col) - '0');
      valid |= isPartNumber(row, col);
    }

    return valid ? std::optional{result} : std::nullopt;
  }

  std::optional<unsigned> gearRatio(unsigned row, unsigned col) const {
    if (at(row, col) != '*')
      return {};

    std::map<std::pair<unsigned, unsigned>, unsigned> numbers{};

    for (int r = -1; r <= 1; r++) {
      for (int c = -1; c <= 1; c++) {
        if (!isDigit(row + r, col + c)) {
          continue;
        }
        for (unsigned i = 0;; i++) {
          if (const auto num = partNumber(row + r, col + c - i); num) {
            numbers[{row + r, col + c - i}] = *num;
            break;
          }
        }
      }
    }
    return numbers.size() == 2 ? std::optional{std::ranges::fold_left(numbers | std::ranges::views::values, 1u, std::multiplies())} : std::nullopt;
  }

  std::vector<std::string> grid{};
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line{};
  while (std::getline(is, line)) {
    grid.grid.push_back(std::move(line));
  }

  return is;
}

int main() {
  Grid grid{};
  std::cin >> grid;

  std::cout << std::format("Part 1 result = {}\n", grid.result(true));
  std::cout << std::format("Part 2 result = {}\n", grid.result(false));
  return 0;
}
