#ifndef SATORI_REQUESTS_HPP
#define SATORI_REQUESTS_HPP

#include <memory>
#include <functional>
#include <iostream>
#include <string>

#include <uv.h>


namespace Satori {

  struct Loop;

  static uv_buf_t createBuffer(char const* str, size_t const len) {
    uv_buf_t buf;
    buf.base = new char[len];
    buf.len = len;
    memcpy(buf.base, str, len);
    return buf;
  }

  namespace detail {

    template<class T = uv_req_t>
    struct Request : T {
      uv_loop_t* loop;

      Request(uv_loop_t* loop)
        : loop(loop)
      {}

      ~Request() {}

      void cancel() {
        uv_cancel((uv_req_t*)this);
      }

      void close() {
        onClose();
        release((Loop*)loop, this);
      }

      std::function<void()> onClose = []() {};
    };

    template<class T = uv_write_t>
    struct Write : Request<T> {
      Write(uv_loop_t* loop)
        : Request<T>(loop)
        , buf{0, 0}
      {}

      ~Write() {
        if (buf.len) {
          delete[] buf.base;
        }
      }

      constexpr void write(void* stream, std::string const& msg) {
        write(stream, msg.c_str(), msg.size());
      }

      template<unsigned n>
      constexpr void write(void* stream, char msg[n]) {
        write(stream, msg, n);
      }

      void write(void* stream, const char* msg, size_t len) {
        if (buf.len) {
          delete[] buf.base;
        }
        buf.base = new char[len];
        buf.len = len;
        memcpy(buf.base, msg, len);
        uv_write((uv_write_t*)this, (uv_stream_t*)stream, &buf, 1, [](uv_write_t* h, int status) {
          auto* write = (Write*)h;
          write->onWriteEnd(status);
        });
      }

      std::function<void(int status)> onWriteEnd = [](int) {};
      uv_buf_t buf;
    };

    template<class T = uv_work_t>
    struct Work : Request<T> {

      Work(uv_loop_t* loop)
        : Request<T>(loop) {
        uv_queue_work(
          (uv_loop_t*)loop,
          (uv_work_t*)this,
          [](void* h) {
            auto handle = (Work*)h;
            handle->job();
          }, [](void* h, int status) {
            auto handle = (Work*)h;
            handle->then(status);
          }
        );
      }

      ~Work() {};

      std::function<void()> job = []{};
      std::function<void(int)> then = [](int) {};
    };

    template<class T = uv_fs_t>
    struct FS : Request<T> {

      FS(uv_loop_t* loop)
        : Request<T>(loop), buffer{0, 0} {}

      ~FS() {
        if (buffer.len > 0) {
          delete[] buffer.base;
          buffer.len = 0;
        }
      };

      int open(std::string path, int flags, int mode) {
        return uv_fs_open(
          this->loop,
          (uv_fs_t*)this,
          path.c_str(),
          flags,
          mode,
          [](uv_fs_t* r) {
            // assert(r == this);
            ssize_t file = r->result;
            uv_fs_req_cleanup(r);
            auto* request = (FS*)r;
            request->onOpen(file);
          });
      }

      int read(ssize_t file) {

        assert(buffer.len == 0);

        buffer.base = new char[bufferSize];
        buffer.len = bufferSize;

        return uv_fs_read(
            this->loop,
            (uv_fs_t*)this,
            file,
            &buffer,
            1,
            0,
            [](uv_fs_t* r) {
              // assert(r == this);
              int result = r->result;
              uv_fs_req_cleanup(r);
              auto* request = (FS*)r;
              request->onRead(result, request->buffer);
            }
          );
      }

      int write(ssize_t file, const char* msg, size_t len) {
        if (buffer.len) {
          delete[] buffer.base;
        }
        buffer.base = new char[len];
        buffer.len = len;
        memcpy(buffer.base, msg, len);
        return uv_fs_write(
          this->loop,
          (uv_fs_t*)this,
          file,
          &buffer,
          1,
          0,
          [](uv_fs_t* r) {
            // assert(r == this);
            int result = r->result;
            uv_fs_req_cleanup(r);
            auto* request = (FS*)r;
            request->onWrite(result);
          });
      }

      int close(ssize_t file) {
        return uv_fs_close(
            this->loop,
            (uv_fs_t*)this,
            file,
            [](uv_fs_t* r) {
            auto* request = (Request<T>*)r;
            request->close();
            });
      }

      std::function<void(ssize_t)> onOpen = [](ssize_t) {};
      std::function<void(int, uv_buf_t)> onRead = [](int, uv_buf_t) {};
      std::function<void(int)> onWrite = [](int) {};

      uv_buf_t buffer;
      unsigned const bufferSize = 1024;
    };

    template<class T = uv_connect_t>
      struct Connect : Request<T> {
        Connect(uv_loop_t* loop)
          : Request<T>(loop)
        {}

        uv_connect_t connect(uv_tcp_t* tcp) {
          return uv_connect_t((uv_connect_t*)this, (uv_tcp_t*)tcp, [](uv_connect_t* h, int status) {
            ((Connect*) h)->onConnect(status);
          });
        }

        int connect(uv_pipe_t* pipe, char const* name) {
          return uv_pipe_connect((uv_connect_t*)this, (uv_pipe_t*)pipe, name, [](uv_connect_t* h, int status) {
            ((Connect*) h)->onConnect(status);
          });
        }

        std::function<void(int status)> onConnect = [](int){};
      };

  }


  using Request = detail::Request<>;
  using Connect = detail::Connect<>;
  using Write = detail::Write<>;
  using Work = detail::Work<>;
  using FS = detail::FS<>;


}

#endif
