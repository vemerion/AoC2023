#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <ranges>

using Cache = std::map<std::pair<uint64_t, uint64_t>, uint64_t>;

struct Line {
  std::string springs{};
  std::vector<unsigned> damaged{};

  uint64_t arrangements() const {
    uint64_t count{};
    Cache cache{};
    arrangements(cache, 0, 0, count);
    return count;
  }

  Line unfold() const {
    Line unfolded{};
    static constexpr unsigned count = 5;
    for (unsigned i = 0; i < count; i++) {
      unfolded.springs += springs + (i != count - 1 ? "?" : "");
      unfolded.damaged.insert(unfolded.damaged.end(), damaged.begin(), damaged.end());
    }
    return unfolded;
  }
private:
  void arrangements(Cache& cache, unsigned springsIndex, unsigned damagedIndex, uint64_t& count) const {
    const auto isValid = [this](unsigned index, unsigned count) -> bool {
      if (index + count > springs.size()) {
        return false;
      }
      for (unsigned i = 0; i < count; i++) {
        if (springs[index + i] == '.') {
          return false;
        }
      }
      return index + count == springs.size() || springs[index + count] != '#';
    };

    if (const auto found = cache.find({springsIndex, damagedIndex}); found != cache.end()) {
      count += found->second;
      return;
    }

    if (springsIndex >= springs.size()) {
      count += damagedIndex == damaged.size();
      return;
    }

    switch (springs[springsIndex]) {
      case '.': {
        uint64_t before = count;
        arrangements(cache, springsIndex + 1, damagedIndex, count);
        cache[{springsIndex, damagedIndex}] = count - before;
        break;
      }
      case '#': {
        if (damagedIndex == damaged.size()) {
          return;
        }
        if (isValid(springsIndex, damaged[damagedIndex])) {
          uint64_t before = count;
          arrangements(cache, springsIndex + damaged[damagedIndex] + 1, damagedIndex + 1, count);
          cache[{springsIndex, damagedIndex}] = count - before;
        }
        break;
      }
      case '?': {
        uint64_t before = count;
        arrangements(cache, springsIndex + 1, damagedIndex, count); // '.'
        if (damagedIndex < damaged.size() && isValid(springsIndex, damaged[damagedIndex])) {
          arrangements(cache, springsIndex + damaged[damagedIndex] + 1, damagedIndex + 1, count); // '#'
        }
        cache[{springsIndex, damagedIndex}] = count - before;
        break;
      }
    }
  }
};

int main() {
  constexpr auto toVector = [](const std::string& s) -> std::vector<unsigned> {
    std::vector<unsigned> result{};
    unsigned n{};
    for (const char* p = s.data(); p < s.data() + s.size(); p++) {
      p = std::from_chars(p, s.data() + s.size(), n).ptr;
      result.push_back(n);
    }
    return result;
  };
  std::vector<Line> lines{};
  std::string springs{};
  std::string damaged{};
  while (std::cin >> springs >> damaged) {
    lines.push_back({std::move(springs), toVector(damaged)});
  }
  std::cout << std::format("Part 1 result = {}\n", std::ranges::fold_left(lines | std::views::transform([](const auto& line) { return line.arrangements(); }), 0ull, std::plus()));
  std::cout << std::format("Part 2 result = {}\n", std::ranges::fold_left(lines | std::views::transform([](const auto& line) { return line.unfold().arrangements(); }), 0ull, std::plus()));
  return 0;
}
