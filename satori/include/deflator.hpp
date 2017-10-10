#ifndef SATORI_DEFLATOR_HPP
#define SATORI_DEFLATOR_HPP

#include <memory>
#include <string>

namespace satori {

struct Deflator {

private:
  struct State;

  std::shared_ptr<State> state;

  void feed(std::string const &chunk, int const type);

public:
  Deflator();

  ~Deflator();

  std::string result() const;

  void feed(std::string const &chunk);

  void finish();
};

} // namespace satori

#endif
