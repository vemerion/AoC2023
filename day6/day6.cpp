#include <iostream>
#include <format>
#include <vector>
#include <sstream>
#include <cstdint>
#include <cmath>
#include <ranges>
#include <algorithm>
#include <string>

uint64_t concat(const std::vector<uint64_t>& values) {
  return std::stoull(std::ranges::fold_left(values | std::views::transform([](const auto n) { return std::to_string(n); }), "", std::plus()));
}

uint64_t counts(uint64_t time, uint64_t distance) {
  double sqrt = std::sqrt(std::pow(time / 2.0, 2.0) - distance); 
  double lower = time / 2.0 - sqrt;
  double upper = time / 2.0 + sqrt;
  return (std::ceil(upper) - 1) - (std::floor(lower) + 1) + 1;
}

int main() {
  constexpr auto numbers = []() -> std::vector<uint64_t> {
    std::string line{};
    std::getline(std::cin, line);
    std::istringstream stream(line);
    std::string unused{};
    uint64_t n{};
    std::vector<uint64_t> result{};
    stream >> unused;
    while (stream >> n) {
      result.push_back(n);
    }
    return result;
  };
  const auto times = numbers();
  const auto distances = numbers();
  std::cout << std::format("Part 1 result = {}\n", std::ranges::fold_left(std::views::zip(times, distances) | std::views::transform([](const auto& pair) { return counts(std::get<0>(pair), std::get<1>(pair)); }), 1ull, std::multiplies()));
  std::cout << std::format("Part 2 result = {}\n", counts(concat(times), concat(distances)));
  return 0;
}
