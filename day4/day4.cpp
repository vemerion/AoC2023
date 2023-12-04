#include <iostream>
#include <format>
#include <vector>
#include <set>
#include <string>
#include <cassert>
#include <algorithm>
#include <ranges>
#include <map>

class Card {
friend Card parse();

public:
  unsigned score() const {
    unsigned result{};
    for (const auto number : numbers) {
      if (winning.contains(number)) {
        result++;
      }
    }

    return result;
  }

private:
  std::set<unsigned> winning{};
  std::set<unsigned> numbers{};
};

using Cache = std::map<unsigned, unsigned>;

unsigned part2(const std::vector<Card>& cards, unsigned index, Cache& cache) {
  if (const auto find = cache.find(index); find != cache.end()) {
    return find->second;
  }
  const auto score = cards[index].score();
  if (score == 0) {
    cache[index] = score;
    return 0;
  }

  unsigned result = score;
  for (unsigned i = 0; i < score; i++) {
    result += part2(cards, index + i + 1, cache);
  }
  cache[index] = result;
  return result;
}

unsigned part2(const std::vector<Card>& cards) {
  Cache cache{};
  unsigned result = cards.size();
  for (unsigned i = 0; i < cards.size(); i++) {
    result += part2(cards, i, cache);
  }
  return result;
}

Card parse() {
  Card card{};
  std::string s{};
  std::cin >> s >> s;
  while (true) {
    std::cin >> s;
    if (s == "|") {
      break;
    }
    card.winning.insert(std::stoi(s));
  }
  while (std::cin.peek() != '\n') {
    std::cin >> s;
    unsigned n = std::stoi(s);
    assert(!card.numbers.contains(n));
    card.numbers.insert(n);
  }
  std::cin.ignore();
  return card;
}

int main() {
  std::vector<Card> cards{};
  while (std::cin.peek() != EOF) {
    cards.push_back(parse());
  }

  std::cout << std::format("Part 1 result = {}\n", std::ranges::fold_left(cards | std::views::transform([](auto& card) { if (const auto score = card.score(); score == 0) return 0; else return 1 << (score - 1); }), 0u, std::plus()));
  std::cout << std::format("Part 2 result = {}\n", part2(cards));
  return 0;
}
