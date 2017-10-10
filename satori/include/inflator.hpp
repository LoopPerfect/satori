#ifndef SATORI_INFLATOR_HPP
#define SATORI_INFLATOR_HPP

#include <memory>
#include <string>

namespace satori {

struct Inflator {

private:
  struct State;

  std::shared_ptr<State> state;

public:
  Inflator();

  ~Inflator();

  std::string result() const;

  bool feed(std::string const &chunk);
};

} // namespace satori

#endif
