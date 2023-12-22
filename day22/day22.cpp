#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <map>
#include <algorithm>

struct Brick {
  unsigned minX{};
  unsigned minY{};
  unsigned minZ{};
  unsigned maxX{};
  unsigned maxY{};
  unsigned maxZ{};

  void fall() {
    minZ--;
    maxZ--;
  }

  bool supports(const Brick& other) const {
    return other.minZ == maxZ + 1 && !(other.minX > maxX || other.maxX < minX || other.minY > maxY || other.maxY < minY);
  }
};

class Sand {
friend std::istream& operator>>(std::istream& is, Sand& sand);
public:
  unsigned settle() {
    std::list<Brick*> list{};
    unsigned count{};
    for (unsigned z = 1; list.size() < bricks.size(); z++) {
      for (auto& brick : bricks) {
        if (brick.maxZ == z) {
          list.push_back(&brick);
        }
      }
    }

    while (!list.empty()) {
      auto* brick = list.front();
      list.pop_front();
      bool falling{};
      while (brick->minZ > 1 && std::ranges::none_of(bricks, [brick](const auto& b) { return b.supports(*brick); })) {
        falling = true;
        brick->fall();
      }
      count += falling;
    }
    return count;
  }

  unsigned part1() const {
    std::set<const Brick*> candidates{};
    std::map<const Brick*, std::set<const Brick*>> support{};

    for (const auto& b1 : bricks) {
      candidates.insert(&b1);
      for (const auto& b2 : bricks) {
        if (b2.supports(b1)) {
          support[&b1].insert(&b2);
        }
      }
    }

    for (const auto& [supported, supports] : support) {
      if (supports.size() == 1) {
        candidates.erase(*supports.begin());
      }
    }
    return candidates.size();
  }

  unsigned part2() const {
    unsigned count{};
    for (size_t i = 0; i < bricks.size(); i++) {
      Sand copy = *this;
      copy.bricks.erase(copy.bricks.begin() + i);
      count += copy.settle();
    }
    return count;
  }

private:
  std::vector<Brick> bricks{};
};

std::istream& operator>>(std::istream& is, Sand& sand) {
  Brick brick{};
  char c{};
  while (is >> brick.minX >> c >> brick.minY >> c >> brick.minZ >> c >> brick.maxX >> c >> brick.maxY >> c >> brick.maxZ) {
    sand.bricks.push_back(brick);
  }
  return is;
}

int main() {
  Sand sand{};
  std::cin >> sand;
  sand.settle();
  std::cout << std::format("Part 1 result = {}\n", sand.part1());
  std::cout << std::format("Part 2 result = {}\n", sand.part2());
  return 0;
}
