#include <iostream>
#include <satori/httpRouter.hpp>

#include <memory>

using namespace std;
using namespace satori;



int main() {

  auto loop = std::make_shared<Loop>();
  auto server = loop->newTcp();

  server->listen("127.0.0.1", 8080);

  AppRouter<> app;

  app.get("/", [] (auto req, auto res) { 
    res.status(200)
      .write("hello world!");
  });

  app.get("/{id}", [](auto req, auto res) {
    
    std::string response = "headers:\n";
    for(auto const& h: req.headers) {
      response += h.first + ": " + h.second + "\n";
    }

    response += "params: ";
    for (auto const& p: req.params) {
      response += p;
    }

    res.status(200)
      .write(response);
  });

  app.apply(loop.get(), server);
  loop->run();
}
