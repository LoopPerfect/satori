#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "uv.h"
#include <memory>
#include <functional>
#include <type_traits>
#include <platform/platform.hpp>
#include <queue>
#include <string.h>




int main() {
  using namespace std;

  auto loop = uv_default_loop();
  auto writer = std::make_shared<WriterPool<>>(1024);
  auto socket = std::make_shared<SocketPool>(1024);

  socket->create([=](Socket* s, Connection* con){

    //std::cout << "new connection" << std::endl;
		con->onData = [=](auto, auto b, auto e) {
      //std::cout << std::string(b,e) << std::endl;
      //auto req = parseReq(string(b,e));
      //std::cout << req["path"] << std::endl;
					//if(req["method"].size()>0)

					std::string res =
					  "HTTP/1.1 200 OK\r\n"
						"Connection: keep-alive\r\n"
						"Content-Type: text/plain\r\n"
						"\r\n"
						"Hello World!";


       //   std::cout << "blub" << std::endl;

          writer->create((uv_stream_t*)con, [=](auto w, int s){
            //std::cout << "closing" << std::endl;
            w->close();
            con->close();
            //std::cout << "closed" << std::endl;
          })->write(res.c_str(), res.size());
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
	})->listen("127.0.0.1", 8082);

	uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}
