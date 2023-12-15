#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <algorithm>
#include <ranges>
#include <array>
#include <list>
#include <cassert>

unsigned hash(const std::string& s) {
  return std::ranges::fold_left(s, 0u, [](const auto a, const auto b) { return (a + b) * 17 % 256; });
}

unsigned power(const std::vector<std::string>& sequence) {
  constexpr auto parse = [](const auto& step) -> std::tuple<std::string, char, unsigned> {
    if (const auto split = step.find('='); split != std::string::npos) {
      return {step.substr(0, split), step[split], std::stoi(step.substr(split + 1))};
    } else if (const auto split = step.find('-'); split != std::string::npos) {
      return {step.substr(0, split), step[split], 0};
    }
    assert(false);
  };

  struct Lens {
    const std::string label{};
    unsigned focal{};
  };

  std::array<std::list<Lens>, 256> boxes{};

  for (const auto& step : sequence) {
    const auto [label, op, focal] = parse(step);
    auto& box = boxes[hash(label)];
    switch (op) {
      case '-':
        box.remove_if([&label](const auto& lens) { return lens.label == label; });
        break;
      case '=': {
        if (auto found = std::ranges::find_if(box, [&label](const auto& lens) { return lens.label == label; }); found != box.end()) {
          found->focal = focal;
        } else {
          box.push_back({label, focal});
        }
        break;
      }
      default:
        assert(false);
    }
  }

  return std::ranges::fold_left(std::views::zip_transform([](const auto i, const auto& box) { return std::ranges::fold_left(std::views::zip_transform([i](const auto j, const auto& lens) { return i * j * lens.focal; }, std::views::iota(1u, box.size() + 1), box), 0u, std::plus()); }, std::views::iota(1u, boxes.size() + 1), boxes), 0u, std::plus());
}

int main() {
  std::vector<std::string> sequence{};
  std::string step{};
  while (std::getline(std::cin, step, ',')) {
    if (step.back() == '\n')
      step.pop_back();
    sequence.push_back(std::move(step));
  }

  std::cout << std::format("Part 1 result = {}\n", std::ranges::fold_left(sequence | std::views::transform([](const auto& s) { return hash(s); }), 0u, std::plus()));
  std::cout << std::format("Part 2 result = {}\n", power(sequence));
  return 0;
}
