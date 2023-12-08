#include <iostream>
#include <format>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <ranges>

struct Node {
  std::string left{};
  std::string right{};
};

using Factors = std::map<uint64_t, uint64_t>;

void factorize(Factors& factors, uint64_t n) {
  Factors fs{};
  for (uint64_t i = 2; n != 1; i++) {
    if (n % i != 0) {
      continue;
    }
    fs[i]++;
    n /= i;
    i = 1;
  }

  for (const auto& [factor, count] : fs) {
    factors[factor] = std::max(factors[factor], count);
  }
}

class Network {
friend std::istream& operator>>(std::istream& is, Network& network);
public:
  uint64_t steps(const bool part1) const {
    std::vector<std::string> current{};
    if (part1) {
      current.push_back("AAA");
    } else {
      for (const auto& [name, node] : nodes) {
        if (name.back() == 'A') {
          current.push_back(name);
        }
      }
    }

    std::map<unsigned, uint64_t> periods{};

    for (uint64_t step = 0; periods.size() < current.size(); step++) {
      const auto left = instructions[step % instructions.size()] == 'L';
      for (unsigned i = 0; i < current.size(); i++) {
        auto& c = current[i];
        c = left ? nodes.at(c).left : nodes.at(c).right;
        if ((part1 && c == "ZZZ") || (!part1 && c.back() == 'Z')) {
          if (!periods.contains(i)) {
            periods[i] = step + 1;
          }
        }
      }
    }

    Factors factors{};
    for (const auto& [index, period] : periods) {
      factorize(factors, period);
    }
    return std::ranges::fold_left(factors | std::views::transform([](const auto& factor) { return std::pow(factor.first, factor.second); }), 1u, std::multiplies());
  }

private:
  std::string instructions{};
  std::map<std::string, Node> nodes{};
};

std::istream& operator>>(std::istream& is, Network& network) {
  is >> network.instructions;
  std::string unused{};
  std::string name{};
  Node node{};
  while (is >> name >> unused >> node.left >> node.right) {
    node.left = node.left.substr(1, 3);
    node.right = node.right.substr(0, 3);
    network.nodes[name] = std::move(node);
  }
  return is;
}

int main() {
  Network network{};
  std::cin >> network;

  std::cout << std::format("Part 1 result = {}\n", network.steps(true));
  std::cout << std::format("Part 2 result = {}\n", network.steps(false));
  return 0;
}
