#ifndef SATORI_FS_HPP
#define SATORI_FS_HPP

#include <string>
#include <memory>
#include <functional>

#include <uv.h>

#include <satori/request.hpp>

namespace Satori {

  namespace detail {

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

      int utime(const char* path, double atime, double mtime) {
        return uv_fs_utime(
          this->loop,
          (uv_fs_t*)this,
          path,
          atime,
          mtime,
          [](uv_fs_t* r) {
            // assert(r == this);
            int result = r->result;
            auto* request = (FS*)r;
            request->onUtime(result);
            uv_fs_req_cleanup(r);
          });
      }

      std::function<void(ssize_t)> onOpen = [](ssize_t) {};
      std::function<void(int, uv_buf_t)> onRead = [](int, uv_buf_t) {};
      std::function<void(int)> onWrite = [](int) {};
      std::function<void(int)> onUtime = [](int) {};

      uv_buf_t buffer;
      unsigned const bufferSize = 1024;
    };

  }
}

#endif
