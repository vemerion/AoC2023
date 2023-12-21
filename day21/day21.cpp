#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <unordered_set>
#include <utility>
#include <array>
#include <cstdint>
#include <cassert>

struct Pos {
  int row{};
  int col{};
  auto operator<=>(const Pos&) const = default;

  Pos opposite() const {
    return {-row, -col};
  }
};

Pos operator+(const Pos& p1, const Pos& p2) {
  return {p1.row + p2.row, p1.col + p2.col};
}

constexpr Pos north{-1, 0};
constexpr Pos east{0, 1};
constexpr Pos south{1, 0};
constexpr Pos west{0, -1};
constexpr std::array cardinals{north, east, south, west};

class Garden {
friend std::istream& operator>>(std::istream& is, Garden& garden);
public:
  uint64_t walk(uint64_t n) const {
    const auto findPattern = [](const std::vector<uint64_t>& values) -> std::vector<std::pair<uint64_t, uint64_t>> {
      std::vector<std::pair<uint64_t, uint64_t>> result{};
      const auto valid = [&values](const uint64_t period) -> bool {
        const auto diff = values[values.size() - 1] - values[values.size() - period - 1];
        for (unsigned i = 1; i < 6; i++) {
          if (values.back() - diff * i != values[values.size() - 1 - i * period]) {
            return false;
          }
        }
        return true;
      };
      for (uint64_t i = 1;; i++) {
        if (valid(i)) {
          for (uint64_t j = 0; j < i; j++) {
            uint64_t current = values.size() - i + j;
            uint64_t prev = values.size() - i * 2 + j;
            result.push_back({values[current], values[current] - values[prev]});
          }
          return result;
        }
      }
      assert(false);
    };
    uint64_t count{};
    const bool odd = n % 2 != 0;

    constexpr auto hash = [](const Pos& p) { return p.row + p.col * 97; };
    std::unordered_set<Pos, decltype(hash)> current{start};
    std::unordered_set<Pos, decltype(hash)> next{};
    std::unordered_set<Pos, decltype(hash)> prev{};
    std::vector<uint64_t> increments{};

    // Assumes pattern found after 1000 steps
    uint64_t i{};
    for (; i <= std::min(n + 1, static_cast<uint64_t>(1000)); i++) {
      increments.push_back(current.size());
      for (const auto& pos : current) {
        count += i % 2 == odd;
        for (const auto& dir : cardinals) {
          if (const auto p = pos + dir; !prev.contains(p) && at(p) != '#') {
            next.insert(p);
          }
        }
      }
      prev = std::move(current);
      current = std::move(next);
    }

    if (i >= n) {
      return count;
    }

    auto pattern = findPattern(increments);
    for (uint64_t j = 0; i <= n; j++, i++) {
      auto& [inc, diff] = pattern[j % pattern.size()];
      inc += diff;
      if (i % 2 == odd) {
        count += inc;
      }
    }
    
    return count;
  }

private:
  std::vector<std::vector<char>> grid{};
  Pos start{};

  char at(const Pos& pos) const {
    int size = grid.size();
    const auto row = (pos.row % size + size) % size;
    size = grid[row].size();
    const auto col = (pos.col % size + size) % size;
    return grid[row][col];
  }
};

std::istream& operator>>(std::istream& is, Garden& garden) {
  std::string line{};
  while (is >> line) {
    garden.grid.push_back({});
    for (const auto c : line) {
      garden.grid.back().push_back(c);
      if (c == 'S') {
        garden.start = Pos(garden.grid.size() - 1, garden.grid.back().size() -1);
      }
    }
  }
  return is;
}

int main() {
  Garden garden{};
  std::cin >> garden;
  std::cout << std::format("Part 1 result = {}\n", garden.walk(64));
  std::cout << std::format("Part 2 result = {}\n", garden.walk(26501365));
  return 0;
}
