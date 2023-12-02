#include <iostream>
#include <format>
#include <vector>
#include <array>
#include <cassert>
#include <ranges>
#include <algorithm>

constexpr std::array colors = { "red", "green", "blue" };

using Counts = std::array<unsigned, colors.size()>;

size_t toIndex(const std::string& s) {
  for (size_t i = 0; i < colors.size(); i++) {
    if (s.contains(colors[i])) {
      return i;
    }
  }
  assert(false);
}

struct Game {
  unsigned id{};
  std::vector<Counts> rounds{};

  bool isPossible(const Counts& counts) const {
    for (const auto& round : rounds) {
      for (size_t i = 0; i < counts.size(); i++) {
        if (round[i] > counts[i]) {
          return false;
        }
      }
    }
    return true;
  }

  unsigned power() const {
    Counts counts{};

    for (const auto& round : rounds) {
      for (size_t i = 0; i < counts.size(); i++) {
        counts[i] = std::max(counts[i], round[i]);
      }
    }

    return std::ranges::fold_left(counts, 1u, std::multiplies());
  }
};

Game parse() {
  Game game{};
  std::string s{};
  std::cin >> s >> game.id >> s;
  while (true) {
    game.rounds.push_back({});
    while (true) {
      unsigned count{};
      std::cin >> count >> s;
      game.rounds.back()[toIndex(s)] = count;
      if (s.back() == ';') {
        break;
      } else if (s.back() != ',') {
        std::cin.ignore(); // Consume newline
        return game;
      }
    }
  }
  assert(false);
}

unsigned part1(const std::vector<Game>& games) {
  static constexpr Counts counts{ 12, 13, 14 };
  return std::ranges::fold_left(games | std::views::filter([](const auto& game) { return game.isPossible(counts); }) | std::views::filter([](const auto& game) { return game.id; }) | std::views::transform([](const auto& game) { return game.id; }), 0u, std::plus());
}

int main() {
  std::vector<Game> games{};

  while (std::cin.peek() != EOF) {
    games.push_back(parse());
  }

  std::cout << std::format("Part 1 result = {}\n", part1(games));
  std::cout << std::format("Part 2 result = {}\n", std::ranges::fold_left(games | std::views::transform([](const auto& game) { return game.power(); }), 0u, std::plus()));
  return 0;
}
