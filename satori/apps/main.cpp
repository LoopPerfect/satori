#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <type_traits>
#include <queue>
#include <string.h>
#include <unistd.h>

#include <uv.h>

#include <satori/satori.hpp>

int main() {

  using namespace std;

  auto loop = Satori::Loop(uv_default_loop());

  //fork();
  auto writer = std::make_shared<Satori::WriterPool<>>(1024);
  auto socket = std::make_shared<Satori::SocketPool>(1024);

  socket->create([=](Satori::Socket* s, Satori::Connection* con){

    //std::cout << "new connection" << std::endl;
		con->onData = [=](auto, auto cstr) {
      //std::cout << std::string(cstr) << std::endl;
      auto req = Satori::parseReq(cstr);
      //std::cout << req["path"] << std::endl;
					//if(req["method"].size()>0)
      if (req.size()) {
        std::string res =
          "HTTP/1.1 200 OK\r\n"
          "Connection: keep-alive\r\n"
          "Content-Type: text/plain\r\n"
          "\r\n"
          "Hello World!";

        writer->create((uv_stream_t*)con, [=](auto w, int s) {
          w->close();
          con->close();
        })->write(res.c_str(), res.size());
      }
    };

		con->onDataEnd = [=](auto) {
			//std::cout << "endRead" << std::endl;
			con->close();
		};

		if (con->accept()==0) {
     // std::cout << "enable reading" << std::endl;
			con->read();
		} else {
			//std::cout << "wierd connection" << std::endl;
			con->close();
		}
	})->listen("127.0.0.1", 8082, loop.loop);

	loop.run(UV_RUN_DEFAULT);
  // loop.close();
}
