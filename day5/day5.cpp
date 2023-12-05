#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <optional>
#include <algorithm>

struct Mapping {
  unsigned dest{};
  unsigned source{};
  unsigned count{};
};

struct Range {
  unsigned start{};
  unsigned count{};

  unsigned end() const {
    return start + count;
  }

  std::pair<std::vector<Range>, std::optional<Range>> map(const Mapping& mapping) const {
    std::vector<Range> leftover{};
    std::optional<Range> mapped{};

    unsigned overlapStart = std::max(start, mapping.source);
    unsigned overlapEnd = std::min(start + count, mapping.source + mapping.count);
    if (overlapStart < overlapEnd) {
      Range overlap{overlapStart, overlapEnd - overlapStart};
      mapped = {mapping.dest + (overlap.start - mapping.source), overlap.count};
      if (overlap.start > start) {
        leftover.push_back({start, overlap.start - start});
      }
      if (overlap.end() < end()) {
        leftover.push_back({overlap.end(), end() - overlap.end()});
      }
    } else {
      leftover.push_back(*this);
    }

    return {leftover, mapped};
  }
};

using Map = std::vector<Mapping>;

class Almanac {
friend std::istream& operator>>(std::istream& is, Almanac& almanac);

public:
  unsigned lowest(const bool part1) const {
    std::vector<Range> ranges{};
    if (part1) {
      for (const auto seed : seeds) {
        ranges.push_back({seed, 1});
      }
    } else {
      for (size_t i = 0; i < seeds.size(); i += 2) {
        ranges.push_back({seeds[i], seeds[i + 1]});
      }
    }

    for (const auto& map : maps) {
      std::vector<Range> processed{};
      for (const auto& mapping : map) {
        std::vector<Range> next{};
        for (const auto& range : ranges) {
          const auto [leftover, mapped] = range.map(mapping);
          next.insert(next.end(), leftover.begin(), leftover.end());
          if (mapped) {
            processed.push_back(*mapped);
          } 
        }
        ranges = std::move(next);
      }
      ranges.insert(ranges.end(), processed.begin(), processed.end());
    }

    return std::ranges::min_element(ranges, [](const auto& a, const auto& b) { return a.start < b.start; })->start;
  }

private:
  std::vector<unsigned> seeds{};
  std::vector<Map> maps{};
};

std::istream& operator>>(std::istream& is, Almanac& almanac) {
  std::string line{};
  std::getline(is, line);
  std::istringstream stream(line);
  std::string unused{};
  unsigned seed{};
  stream >> unused;
  while (stream >> seed) {
    almanac.seeds.push_back(seed);
  }
    
  while (std::getline(is, line)) {
    if (line.empty()) {
      continue;
    } else if (line.contains("map")) {
      almanac.maps.push_back({});
    } else {
      Mapping mapping{};
      stream = std::istringstream(line);
      stream >> mapping.dest >> mapping.source >> mapping.count;
      almanac.maps.back().push_back(mapping);
    }
  }
  
  return is;
}

int main() {
  Almanac almanac;
  std::cin >> almanac;
  std::cout << std::format("Part 1 result = {}\n", almanac.lowest(true));
  std::cout << std::format("Part 2 result = {}\n", almanac.lowest(false));
  return 0;
}
