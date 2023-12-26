#include <iostream>
#include <format>
#include <unordered_map>
#include <list>
#include <unordered_set>
#include <set>
#include <vector>
#include <sstream>
#include <utility>

using Connection = std::pair<unsigned, unsigned>;

class Components {
friend std::istream& operator>>(std::istream& is, Components& components);
public:
  unsigned split() const {
    unsigned count = 1;
    const unsigned start = 0;
    for (unsigned component = 1; component < components.size(); component++) {
      count += grouped(component, start);
    }
    return count * (components.size() - count);
  }
private:
  std::vector<std::unordered_set<unsigned>> components{};
  
  bool grouped(const unsigned start, const unsigned end) const {
    std::set<Connection> forbidden{};
    unsigned count{};
    for (auto found = findPath(start, end, forbidden); found; found = findPath(start, end, forbidden)) {
      if (++count > 3) {
        return true;
      }
    }
    return false;
  }
  
  bool findPath(const unsigned start, const unsigned end, std::set<Connection>& forbidden) const {
    std::unordered_map<unsigned, unsigned> path{};
    std::list<unsigned> list{start};
    std::unordered_set<unsigned> visited{start};
    while (!list.empty()) {
      const auto component = list.front();
      list.pop_front();
      if (component == end) {
        for (auto c = component;; c = path.at(c)) {
          forbidden.insert({c, path[c]});
          forbidden.insert({path[c], c});
          if (c == start) {
            break;
          }
        }
        return true;
      }
      for (const auto& connection : components[component]) {
        if (!forbidden.contains({component, connection}) && !visited.contains(connection)) {
          path[connection] = component;
          list.push_back(connection);
          visited.insert(connection);
        }
      }
    }
    return false;
  }
};

std::istream& operator>>(std::istream& is, Components& components) {
  constexpr auto tokenize = [](std::string& line) -> std::vector<std::string> {
    std::string token{};
    std::istringstream stream(std::move(line));
    std::vector<std::string> result{};
    while (stream >> token) {
      result.push_back(std::move(token));
    }
    return result;
  };
  std::string line{};
  unsigned id{};
  std::unordered_map<std::string, unsigned> names{};
  while (std::getline(is, line)) {
    auto tokens = tokenize(line);
    tokens[0].pop_back();
    for (const auto& token : tokens) {
      if (names.insert({token, id}).second) {
        id++;
      }
    }
    while (components.components.size() < id) {
      components.components.push_back({});
    }
    for (unsigned i = 1; i < tokens.size(); i++) {
      components.components[names[tokens[0]]].insert(names[tokens[i]]);
      components.components[names[tokens[i]]].insert(names[tokens[0]]);
    }
  }
  return is;
}

int main() {
  Components components{};
  std::cin >> components;
  std::cout << std::format("Part 1 result = {}\n", components.split());
  return 0;
}
