#include <format>
#include <iostream>
#include <string>
#include <array>
#include <string_view>
#include <cassert>

using Num = std::pair<std::string, std::string>;

auto toDigit(const std::string& line, bool fromLeft, bool part1) {
  static constexpr std::array digits = { Num{ "one", "1" }, Num{ "two", "2" }, Num{ "three", "3" }, Num{ "four", "4" }, Num{ "five", "5" }, Num{ "six", "6" }, Num{"seven", "7" }, Num{ "eight", "8" }, Num{ "nine", "9" } };
  for (size_t i = 1; i <= line.size(); i++) {
    std::string_view substr{fromLeft ? line.begin() : line.begin() + line.size() - i, fromLeft ? line.begin() + i : line.end()};
    for (const auto& [word, digit] : digits) {
      if ((!part1 && substr.find(word) != std::string::npos) || substr.find(digit) != std::string::npos) {
        return digit;
      }
    }
  }
  assert(false);
}

int main() {
  unsigned part1{};
  unsigned part2{};
  std::string line{};

  while (std::cin >> line) {
    part1 += std::stoi(toDigit(line, true, true) + toDigit(line, false, true));
    part2 += std::stoi(toDigit(line, true, false) + toDigit(line, false, false));
  }

  std::cout << std::format("Part 1 result = {}\n", part1);
  std::cout << std::format("Part 2 result = {}\n", part2);

  return 0;
}
