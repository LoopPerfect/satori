#ifndef SATORI_FS_HPP
#define SATORI_FS_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <cassert>
#include <uv.h>

#include <satori/requests.hpp>

namespace satori {

template <class R>
void release(R);

template <class T>
struct FS : uv_fs_t {

  void cleanup() {
    uv_fs_req_cleanup((uv_fs_t*)this);
    release((T*)this);
  }
};

struct FSClose : FS<FSClose> {

  FSClose(uv_loop_t* loop, ssize_t file) { close(loop, file); }


  ~FSClose(){}

  int close(uv_loop_t* loop, ssize_t file) {
    return uv_fs_close(loop, (uv_fs_t*)this, file, [](uv_fs_t* r) {
      auto* request = (FSClose*)r;
      request->onClose();
      request->cleanup();
    });
  }

  std::function<void(void)> onClose = [] {};
};

struct FSOpen : FS<FSOpen> {

  FSOpen(uv_loop_t* loop, std::string const& path, int flags, int mode) {
    open(loop, path, flags, mode);
  }


  ~FSOpen(){}

  int open(uv_loop_t* loop, std::string const& path, int flags, int mode) {
    return uv_fs_open(
      loop, (uv_fs_t*)this, path.c_str(), flags, mode, [](uv_fs_t* r) {
        // assert(r == this);
        ssize_t file = r->result;
        auto* request = (FSOpen*)r;
        request->onOpen(file);
        request->cleanup();
      });
  }

  std::function<void(ssize_t)> onOpen = [](ssize_t) {};
};

struct FSRead : FS<FSRead> {

  FSRead(uv_loop_t* loop, ssize_t fileID, unsigned bufSize = 1024) {
    read(loop, fileID, bufSize);
  }

  ~FSRead(){}

  int read(uv_loop_t* loop, ssize_t file, unsigned bufferSize) {
    buffer = uv_buf_init(new char[bufferSize], bufferSize);

    return uv_fs_read(
      loop, (uv_fs_t*)this, file, &buffer, 1, 0, [](uv_fs_t* r) {
        // assert(r == this);
        int result = r->result;
        auto* request = (FSRead*)r;
        request->onRead(result, request->buffer);
        delete[] request->buffer.base;
        request->cleanup();
      });
  }

  uv_buf_t buffer;
  std::function<void(int, uv_buf_t)> onRead = [](int, uv_buf_t) {};
};

struct FSWrite : FS<FSWrite> {

  FSWrite(uv_loop_t* loop, ssize_t file, std::string const& msg) : msg{msg} {
    write(loop, file);
  }


  ~FSWrite(){}

  int write(uv_loop_t* loop, ssize_t file) {
    uv_buf_t buf = uv_buf_init(&msg[0], msg.size());
    return uv_fs_write(loop, (uv_fs_t*)this, file, &buf, 1, 0, [](uv_fs_t* r) {
      // assert(r == this);
      int result = r->result;
      auto* request = (FSWrite*)r;
      request->onWrite(result);
      request->cleanup();
    });
  }

  std::string msg;
  std::function<void(int)> onWrite = [](int) {};
};

struct FSStat : FS<FSStat> {

  FSStat(uv_loop_t* loop, std::string const& path) { stat(loop, path); }

  int stat(uv_loop_t* loop, std::string const& path) {
    return uv_fs_stat(loop, (uv_fs_t*)this, path.c_str(), [](uv_fs_t* r) {
      // assert(r == this);
      auto result = r->result;
      auto statbuf = r->statbuf;
      auto* request = (FSStat*)r;
      request->onStat(result, statbuf);
      request->cleanup();
    });
  }

  std::function<void(int, uv_stat_t)> onStat = [](int, uv_stat_t) {};
};

struct FSUTime : FS<FSUTime> {

  FSUTime(uv_loop_t* loop, std::string const& path, double atime,
          double mtime) {
    utime(loop, path, atime, mtime);
  }

  int utime(uv_loop_t* loop, std::string const& path, double atime,
            double mtime) {
    return uv_fs_utime(
      loop, (uv_fs_t*)this, path.c_str(), atime, mtime, [](uv_fs_t* r) {
        // assert(r == this);
        int result = r->result;
        auto* request = (FSUTime*)r;
        request->onUtime(result);
        request->cleanup();
      });
  }

  std::function<void(int)> onUtime = [](int) {};
};

struct FSScanDir : FS<FSScanDir> {

  FSScanDir(uv_loop_t* loop, std::string const& path, int flags) {
    scandir(loop, path, flags);
  }

  int scandir(uv_loop_t* loop, std::string const& path, int flags) {
    return uv_fs_scandir(
      loop, (uv_fs_t*)this, path.c_str(), flags, [](uv_fs_t* r) {
        auto result = r->result;
        auto* request = (FSScanDir*)r;
        request->onScandir(result);
        uv_dirent_t ent;
        while (true) {
          int i = uv_fs_scandir_next(r, &ent);
          auto keepGoing = request->onScandirNext(ent);
          if (i == UV_EOF || !keepGoing) {
            break;
          }
        }
        request->cleanup();
      });
  }

  std::function<void(int)> onScandir = [](int) {};
  std::function<bool(uv_dirent_t)> onScandirNext = [](uv_dirent_t) {
    return false;
  };
};

struct FSRealPath : FS<FSRealPath> {

  FSRealPath(uv_loop_t* loop, std::string const& path) { realpath(loop, path); }

  int realpath(uv_loop_t* loop, std::string const& path) {
    return uv_fs_realpath(loop, (uv_fs_t*)this, path.c_str(), [](uv_fs_t* r) {
      auto result = r->result;
      auto* request = (FSRealPath*)r;
      if (result < 0) {
        request->onError(result);
      } else {
        request->onRealpath(std::string((char*)r->ptr));
      }
      request->cleanup();
    });
  }

  std::function<void(std::string)> onRealpath = [](std::string) {};
  std::function<void(ssize_t)> onError = [](ssize_t) {};
};

} // namespace satori

#endif
