#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <functional>
#include <algorithm>
#include <ranges>
#include <utility>

auto extrapolate(std::deque<int> numbers) {
  std::vector<std::deque<int>> sequences{std::move(numbers)};
  while (std::ranges::any_of(sequences.back(), [](const auto e) { return e != 0; })) {
    std::deque<int> sequence{};
    // No ranges::to yet...
    for (const auto e : sequences.back() | std::views::pairwise_transform([](const auto a, const auto b) { return b - a; }))
      sequence.push_back(e);
    sequences.push_back(std::move(sequence));
  }
  for (size_t i = sequences.size() - 2; i < sequences.size(); i--) {
    sequences[i].push_back(sequences[i].back() + sequences[i + 1].back());
    sequences[i].push_front(sequences[i].front() - sequences[i + 1].front());
  }
  return std::pair{sequences[0].front(), sequences[0].back()};
}

int main() {
  constexpr auto numbers = [](const std::string& line) -> std::deque<int> {
    std::istringstream stream(line);
    int n{};
    std::deque<int> result{};
    while (stream >> n) {
      result.push_back(n);
    }
    return result;
  };

  std::string line{};
  int part1{};
  int part2{};
  while (std::getline(std::cin, line)) {
    const auto [previous, next] = extrapolate(numbers(line));
    part1 += next;
    part2 += previous;
  }
  std::cout << std::format("Part 1 result = {}\n", part1);
  std::cout << std::format("Part 2 result = {}\n", part2);
  return 0;
}
