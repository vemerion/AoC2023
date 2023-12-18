#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <map>
#include <array>
#include <cstdint>
#include <cassert>

struct Pos {
  int64_t row{};
  int64_t col{};
  auto operator<=>(const Pos&) const = default;
  
  Pos& operator+=(const Pos& other) {
    row += other.row;
    col += other.col;
    return *this;
  }

  Pos operator*(const int64_t n) const {
    return {row * n, col * n};
  }
};

Pos operator+(Pos p1, const Pos& p2) {
  p1 += p2;
  return p1;
}

constexpr Pos up{-1, 0};
constexpr Pos right{0, 1};
constexpr Pos down{1, 0};
constexpr Pos left{0, -1};
constexpr std::array cardinals{up, right, down, left};
const std::map<char, Pos> directions{{'U', up}, {'R', right}, {'D', down}, {'L', left}, {'0', right}, {'1', down}, {'2', left}, {'3', up}};

struct Step {
  char dir{};
  int64_t count{};
  std::string color{};

  char realDir() const {
    return color[7];
  }

  int64_t realCount() const {
    return std::stoll(color.substr(2, 5), nullptr, 16);
  }
};

struct Line {
  Pos start{};
  int64_t length{};
  bool down{};

  bool overlap(int64_t row) const {
    if (down) {
      return row >= start.row && row < start.row + length;
    } else {
      return row > start.row - length && row <= start.row;
    }
  }
};

class Plan {
  friend std::istream& operator>>(std::istream& is, Plan& plan);
public:
  uint64_t size(const bool part1) const {
    // Assumes interior is to the left of the lines going down and that the lagoon does not start and end with a line going down
    static constexpr auto interior = [](const std::vector<Line>& lines) -> uint64_t {
      uint64_t result{};
      for (size_t i = 0; i < lines.size(); i++) {
        const auto& line = lines[i];
        if (!line.down) {
          continue;
        }

        for (int64_t n = lines[i - 1].start.col < line.start.col ? 1 : 0; n < line.length - (lines[i + 1].start.col < line.start.col ? 1 : 0); n++) {
          const Line* found = nullptr;
          for (const auto& other : lines) {
            if (other.overlap(line.start.row + n) && other.start.col < line.start.col && (found == nullptr || other.start.col > found->start.col)) {
              found = &other;
            }
          }
          assert(found != nullptr);
          assert(!found->down);
          result += std::abs(line.start.col - found->start.col) - 1;
        }
      }

      return result;
    };
    Pos current{0, 0};
    std::vector<Line> lines{};
    uint64_t border{};

    for (const auto& step : steps) {
      const auto dir = directions.at(part1 ? step.dir : step.realDir());
      const auto count = part1 ? step.count : step.realCount();
      border += count;
      if (dir == up || dir == down) {
        lines.push_back({current, count + 1, dir == down});
      }
      current += dir * count;
    }

    return interior(lines) + border;
  }

private:
  std::vector<Step> steps{};
};

std::istream& operator>>(std::istream& is, Plan& plan) {
  Step step{};
  while (is >> step.dir >> step.count >> step.color) {
    plan.steps.push_back(step);
  }
  return is;
}

int main() {
  Plan plan{};
  std::cin >> plan;
  std::cout << std::format("Part 1 result = {}\n", plan.size(true));
  std::cout << std::format("Part 2 result = {}\n", plan.size(false));
  return 0;
}
