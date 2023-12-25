#include <iostream>
#include <format>
#include <vector>
#include <cstdint>

using Int = int64_t;

struct Line {
  Int x{};
  Int y{};
  Int z{};
  Int vx{};
  Int vy{};
  Int vz{};

  bool intersects(const Line& other, Int min, Int max) const {
    const double denominator = static_cast<double>(vx * other.vy) / static_cast<double>(vy) - other.vx;
    if (denominator == 0) {
      return false;
    }
    const double numerator = other.x - x - static_cast<double>(vx * (other.y - y)) / vy;
    const double t2 = numerator / denominator;
    const double t1 = (other.x - x + other.vx * t2) / vx;
    if (t1 < 0 || t2 < 0) {
      return false;
    }
    return (x + t1 * vx) >= min && (x + t1 * vx) <= max && (y + t1 * vy) >= min && (y + t1 * vy) <= max;
  }
};

class Hailstorm {
friend std::istream& operator>>(std::istream& is, Hailstorm& hailstorm);
public:
  unsigned intersections(Int min, Int max) const {
    unsigned count{};
    for (unsigned i = 0; i < lines.size(); i++) {
      for (unsigned j = i + 1; j < lines.size(); j++) {
        if (lines[i].intersects(lines[j], min, max)) {
          count++;
        }
      }
    }
    return count;
  }

private:
  std::vector<Line> lines{};
};

std::istream& operator>>(std::istream& is, Hailstorm& hailstorm) {
  Line line{};
  char c{};
  while (is >> line.x >> c >> line.y >> c >> line.z >> c >> line.vx >> c >> line.vy >> c >> line.vz) {
    hailstorm.lines.push_back(line);
  }
  return is;
}

int main() {
  Hailstorm hailstorm{};
  std::cin >> hailstorm;
  std::cout << std::format("Part 1 result = {}\n", hailstorm.intersections(200000000000000ll, 400000000000000ll));
  return 0;
}
