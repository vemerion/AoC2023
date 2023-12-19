#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <algorithm>
#include <ranges>
#include <utility>
#include <optional>
#include <list>

std::vector<std::string> tokenize(const std::string& s) {
  std::vector<std::string> tokens{};
  std::string token{};
  for (const auto c : s) {
    if (std::isalnum(static_cast<unsigned char>(c))) {
      token.push_back(c);
    } else {
      if (!token.empty()) {
        tokens.push_back(std::move(token));
      }
      token = "";
      tokens.push_back(std::string{c});
    }
  }
  return tokens;
}

struct Range {
  int64_t min{};
  int64_t max{};

  bool empty() const {
    return max < min;
  }

  int64_t count() const {
    return empty() ? 0 : max - min + 1;
  }

  std::pair<Range, Range> split(int64_t v) const {
    return {v < min ? Range{1, 0} : Range{min, std::min(v, max)}, v > max ? Range{1, 0} : Range{std::max(v, min), max}};
  }
};

struct Part {
  Range x{};
  Range m{};
  Range a{};
  Range s{};

  Range& operator[](const char c) {
    switch (c) {
      case 'x':
        return x;
      case 'm':
        return m;
      case 'a':
        return a;
      case 's':
        return s;
      default:
        assert(false);
    }
  }

  const Range& operator[](const char c) const {
    switch (c) {
      case 'x':
        return x;
      case 'm':
        return m;
      case 'a':
        return a;
      case 's':
        return s;
      default:
        assert(false);
    }
  }

  int64_t sum() const {
    return x.min + m.min + a.min + s.min;
  }

  int64_t combinations() const {
    return x.count() * m.count() * a.count() * s.count();
  }

  static Part parse(const std::string& line) {
    Part result{};
    const auto tokens = tokenize(line);
    for (size_t i = 1; i < tokens.size(); i += 4) {
      const int64_t value = std::stoi(tokens[i + 2]);
      result[tokens[i][0]] = {value, value};
    }
    return result;
  }
};

struct Rule {
  char cat{};
  char op{};
  int64_t val{};
  std::string dest{};

  std::pair<std::optional<Part>, std::optional<Part>> process(const Part& part) const {
    if (op == '\0') {
      return {part, {}};
    }
    auto [left, right] = part[cat].split(val);
    left.max -= op == '<' && left.max == val;
    right.min += op == '>' && right.min == val;
    auto p1 = part;
    auto p2 = part;
    const auto& pass = op == '<' ? left : right;
    const auto& fail = op == '<' ? right : left;
    p1[cat] = pass;
    p2[cat] = fail;
    return {pass.empty() ? std::nullopt : std::optional{p1}, fail.empty() ? std::nullopt : std::optional{p2}};
  }
};

class Workflow {
public:
  static Workflow parse(const std::string& line) {
    Workflow result{};
    const auto tokens = tokenize(line);
    result.name = std::move(tokens[0]);
    for (size_t i = 2; i < tokens.size();) {
      if (tokens[i + 1] == "," || tokens[i + 1] == "}") {
        result.rules.push_back({'\0', '\0', 0, tokens[i]});
        i += 2;
      } else {
        result.rules.push_back({tokens[i][0], tokens[i + 1][0], std::stoi(tokens[i + 2]), std::move(tokens[i + 4])});
        i += 6;
      }
    }
    return result;
  }

  auto process(Part part) const {
    std::vector<std::pair<std::string, Part>> result{};
    for (const auto& rule : rules) {
      const auto [pass, fail] = rule.process(part);
      if (pass) {
        result.push_back({rule.dest, *pass});
      }
      if (fail) {
        part = *fail;
      } else {
        break;
      }
    }
    return result;
  }

  const std::string& getName() const {
    return name;
  }
private:
  std::string name{};
  std::vector<Rule> rules{};
};



class System {
friend std::istream& operator>>(std::istream& is, System& system);
public:
  int64_t process(const bool part1) const {
    std::vector<Part> accepted{};
    std::list<std::pair<std::string, Part>> list{};
    if (part1) {
      for (const auto& part : parts) {
        list.push_back({"in", part});
      }
    } else {
      list.push_back({"in", {{1, 4000}, {1, 4000}, {1, 4000}, {1, 4000}}});
    }

    while (!list.empty()) {
      const auto [name, part] = std::move(list.front());
      list.pop_front();
      if (name == "A") {
        accepted.push_back(part);
      } else if (name == "R") {
        continue;
      } else {
        for (const auto& next : workflows.at(name).process(part)) {
          list.push_back(next);
        }
      }
    }
    return std::ranges::fold_left(accepted | std::views::transform([part1](const Part& part) { return part1 ? part.sum() : part.combinations(); }), 0, std::plus());
  }

private:
  std::map<std::string, Workflow> workflows{};
  std::vector<Part> parts{};
};

std::istream& operator>>(std::istream& is, System& system) {
  std::string line{};
  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }
    const auto workflow = Workflow::parse(line);
    system.workflows[workflow.getName()] = workflow;
  }
  
  while (std::getline(is, line)) {
    system.parts.push_back(Part::parse(line));
  }
  return is;
}

int main() {
  System system{};
  std::cin >> system;
  std::cout << std::format("Part 1 result = {}\n", system.process(true));
  std::cout << std::format("Part 2 result = {}\n", system.process(false));
  return 0;
}
