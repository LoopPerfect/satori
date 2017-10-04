#ifndef SATORI_DEFLATOR_HPP
#define SATORI_DEFLATOR_HPP

#include <string>
#include <memory>

namespace satori {

  struct Deflator {

  private:

    struct State;

    std::shared_ptr<State> state;

    void feed(std::string const& chunk, int const type);

  public:

    Deflator();

    ~Deflator();

    std::string result() const;

    void feed(std::string const& chunk);

    void finish();

  };

}

#endif
