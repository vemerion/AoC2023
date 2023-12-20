#include <iostream>
#include <format>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <ranges>
#include <cassert>
#include <sstream>
#include <numeric>

struct Signal {
  const std::string source{};
  const std::string dest{};
  bool pulse{};
};

class Module {
public:
  Module(std::vector<std::string> dests) : dests(std::move(dests)) {}
  virtual void receive(const Signal& signal, const std::function<void(Signal)>& add) = 0;
  virtual void reset() = 0;
  virtual ~Module() = default;

  const std::vector<std::string>& getDests() const {
    return dests;
  }

  virtual void addSource([[maybe_unused]] const std::string& source) {}
protected:
  const std::vector<std::string> dests{};
};

class FlipFlop : public Module {
public:
  FlipFlop(std::vector<std::string> dests) : Module(std::move(dests)) {}
  virtual ~FlipFlop() = default;

  void receive(const Signal& signal, const std::function<void(Signal)>& add) override {
    if (signal.pulse) {
      return;
    }
    state = !state;
    for (const auto& d : dests) {
      add({signal.dest, d, state});
    }
  }

  void reset() override { 
    state = false;
  }
private:
  bool state{};
};

class Conjunction : public Module {
public:
  Conjunction(std::vector<std::string> dests) : Module(std::move(dests)) {}
  virtual ~Conjunction() = default;

  void receive(const Signal& signal, const std::function<void(Signal)>& add) override {
    memory[signal.source] = signal.pulse;
    const bool pulse = !std::ranges::all_of(memory | std::views::values, [](const auto p) { return p; });
    for (const auto& d : dests) {
      add({signal.dest, d, pulse});
    }
  }

  void addSource(const std::string& source) override {
    memory[source] = false;
  }

  void reset() override { 
    for (auto& [key, val] : memory) {
      val = false;
    }
  }
private:
  std::map<std::string, bool> memory{};
};

class Broadcast : public Module {
public:
  Broadcast(std::vector<std::string> dests) : Module(std::move(dests)) {}
  virtual ~Broadcast() = default;

  void receive(const Signal& signal, const std::function<void(Signal)>& add) override {
    for (const auto& d : dests) {
      add({signal.dest, d, signal.pulse});
    }
  }

  void reset() override { 
  }
};

class Configuration {
friend std::istream& operator>>(std::istream& is, Configuration& configuration);
public:
  uint64_t press(const bool part1) {
    static constexpr auto hasDest = [](const Module& m, const std::string& dest) { return std::ranges::find(m.getDests(), dest) != m.getDests().end(); };
    for (auto& [name, module] : modules) {
      module->reset();
    }
    uint64_t lowCount{};
    uint64_t highCount{};
    std::map<std::string, uint64_t> periods{};
    std::string penultimate = std::ranges::find_if(modules, [](const auto& pair) { return hasDest(*pair.second, "rx"); })->first;
    unsigned periodCount = std::ranges::count_if(modules, [&penultimate](const auto& pair) { return hasDest(*pair.second, penultimate); });
    for (unsigned i = 1; part1 ? i <= 1000 : periods.size() < periodCount; i++) {
      std::list<Signal> list{{"button", "broadcaster", false}};
      const auto add = [&list](Signal signal) { list.push_back(std::move(signal)); }; 
      while (!list.empty()) {
        const auto signal = std::move(list.front());
        list.pop_front();
        if (signal.dest == penultimate && signal.pulse) {
          periods[signal.source] = i;
        }
        if (const auto found = modules.find(signal.dest); found != modules.end()) {
          found->second->receive(signal, add);
        }
        lowCount += !signal.pulse;
        highCount += signal.pulse;
      }
    }
    return part1 ? lowCount * highCount : std::ranges::fold_left(periods | std::views::values, 1ull, [](const auto& a, const auto& b) { return std::lcm(a, b); });
  }

private:
  std::map<std::string, std::unique_ptr<Module>> modules{};
};

std::istream& operator>>(std::istream& is, Configuration& configuration) {
  constexpr auto split = [](std::string line) -> std::vector<std::string> {
    std::istringstream stream(std::move(line));
    std::string token{};
    std::vector<std::string> result{};
    while (stream >> token) {
      result.push_back(std::move(token));
    }
    return result;
  };

  std::string line{};
  while (std::getline(is, line)) {
    std::ranges::replace(line, ',', ' ');
    const auto tokens = split(std::move(line));
    std::vector<std::string> dests{};
    dests.insert(dests.end(), tokens.begin() + 2, tokens.end());
    switch (tokens[0][0]) {
      case '%':
        configuration.modules[tokens[0].substr(1)] = std::make_unique<FlipFlop>(std::move(dests));
        break;
      case '&':
        configuration.modules[tokens[0].substr(1)] = std::make_unique<Conjunction>(std::move(dests));
        break;
      case 'b':
        configuration.modules[tokens[0]] = std::make_unique<Broadcast>(std::move(dests));
        break;
      default:
        assert(false);
    }
  }

  for (const auto& [key, val] : configuration.modules) {
    for (const auto& d : val->getDests()) {
      if (const auto found = configuration.modules.find(d); found != configuration.modules.end()) {
        found->second->addSource(key);
      }
    }
  }
  return is;
}

int main() {
  Configuration configuration{};
  std::cin >> configuration;
  std::cout << std::format("Part 1 result = {}\n", configuration.press(true));
  std::cout << std::format("Part 2 result = {}\n", configuration.press(false));
  return 0;
}
