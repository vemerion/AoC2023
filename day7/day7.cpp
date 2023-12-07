#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <cassert>
#include <ranges>
#include <list>

enum class Type { HighCard, OnePair, TwoPair, ThreeOfAKind, FullHouse, FourOfAKind, FiveOfAKind };

struct Hand {
  std::string cards{};
  unsigned bid{};

  bool lessThan(const Hand& other, const bool part1) const {
    if (const auto cmp = calcType(part1) <=> other.calcType(part1); cmp < 0) {
      return true;
    } else if (cmp == 0) {
      for (size_t i = 0; i < cards.size(); i++) {
        if (const auto cmp = strength(i, part1) <=> other.strength(i, part1); cmp < 0) {
          return true;
        } else if (cmp > 0) {
          return false;
        }
      }
      assert(false);
    } else {
      return false;
    }
  }

  unsigned strength(const unsigned i, const bool part1) const {
    char c = cards[i];
    if (!part1 && c == 'J') {
      return 0;
    }
    static constexpr std::string order{"23456789TJQKA"};
    return std::distance(order.begin(), std::ranges::find(order, c)) + 1;
  }

  Type calcType(bool part1) const {
    constexpr auto calc = [](const auto& counts) -> Type {
      std::map<unsigned, unsigned> copies{};
      for (const auto& [card, count] : counts) {
        copies[count]++;
      }

      if (copies.contains(5)) {
        return Type::FiveOfAKind;
      } else if (copies[1] == 5) {
        return Type::HighCard;
      } else if (copies.contains(4)) {
        return Type::FourOfAKind;
      } else if (copies.contains(3) && copies.contains(2)) {
        return Type::FullHouse;
      } else if (copies.contains(3)) {
        return Type::ThreeOfAKind;
      } else if (copies[2] == 2) {
        return Type::TwoPair;
      } else {
        return Type::OnePair;
      }
    };

    std::map<char, unsigned> counts{};
    for (const auto c : cards) {
      counts[c]++;
    }
    if (part1) {
      return calc(counts);
    } else {
      auto type{Type::HighCard};
      std::list<std::map<char, unsigned>> list{counts};
      while (!list.empty()) {
        auto next = list.front();
        list.pop_front();
        type = std::max(type, calc(next));
        if (const auto jokers = next['J']; jokers != 0) {
          next['J'] = 0;
          for (const auto& [card, count] : next) {
            if (count == 0)
              continue;
            auto copy = next;
            copy[card]++;
            copy['J'] = jokers - 1;
            list.push_back(std::move(copy));
          }
        }
      }
      return type;
    }
  }
};

unsigned winnings(std::vector<Hand>& hands, bool part1) {
  std::ranges::sort(hands, [part1](const auto& a, const auto& b) { return a.lessThan(b, part1); });
  return std::ranges::fold_left(std::views::zip(std::views::iota(1u, hands.size() + 1), hands) | std::views::transform([](const auto& pair) { return std::get<0>(pair) * std::get<1>(pair).bid; }), 0u, std::plus());
}

int main() {
  std::vector<Hand> hands{};
  std::string cards{};
  unsigned bid{};
  while (std::cin >> cards >> bid) {
    hands.push_back({cards, bid});
  }
  std::cout << std::format("Part 1 result = {}\n", winnings(hands, true));
  std::cout << std::format("Part 2 result = {}\n", winnings(hands, false));
  return 0;
}
