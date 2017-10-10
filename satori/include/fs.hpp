#ifndef SATORI_FS_HPP
#define SATORI_FS_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <cassert>
#include <uv.h>

#include <satori/directoryEntry.hpp>
#include <satori/requests.hpp>
#include <satori/stringview.hpp>

namespace satori {

template <class R> void release(R);

template <class T> struct FS : uv_fs_t {

  FS(uv_loop_t *loop) { this->loop = loop; }

  void cleanup() {
    uv_fs_req_cleanup((uv_fs_t *)this);
    release((T *)this);
  }
};

struct FSClose : FS<FSClose> {

  FSClose(uv_loop_t *loop, ssize_t file) : FS<FSClose>(loop) {
    close(loop, file);
  }

  ~FSClose() {}

  int close(uv_loop_t *loop, ssize_t file) {
    return uv_fs_close(loop, (uv_fs_t *)this, file, [](uv_fs_t *r) {
      auto *request = (FSClose *)r;
      request->onClose();
      request->cleanup();
    });
  }

  std::function<void(void)> onClose = [] {};
};

struct FSOpen : FS<FSOpen> {

  FSOpen(uv_loop_t *loop, std::string const &path, int flags, int mode)
      : FS<FSOpen>(loop) {
    open(loop, path, flags, mode);
  }

  ~FSOpen() {}

  int open(uv_loop_t *loop, std::string const &path, int flags, int mode) {
    return uv_fs_open(loop, (uv_fs_t *)this, path.c_str(), flags, mode,
                      [](uv_fs_t *r) {
                        // assert(r == this);
                        ssize_t file = r->result;
                        auto *request = (FSOpen *)r;
                        request->onOpen(file);
                        request->cleanup();
                      });
  }

  std::function<void(ssize_t)> onOpen = [](ssize_t) {};
};

struct FSRead : FS<FSRead> {

  FSRead(uv_loop_t *loop, ssize_t fileID, unsigned bufferSize = 4096)
      : FS<FSRead>(loop), file(fileID) {
    buffer = uv_buf_init(new char[bufferSize], bufferSize); // TODO: Memory pool
  }

  ~FSRead() { delete[] buffer.base; }

  int read() {
    // TODO: Support n buffers
    reading = true;
    return uv_fs_read(
        loop, (uv_fs_t *)this, file, &buffer, 1, offset, [](uv_fs_t *r) {
          // assert(r == this);
          int result = r->result;
          auto *request = (FSRead *)r;

          request->onRead((result < 0) ? result : 0, request->buffer);

          if (result == 0) {
            request->onReadEnd();
          }

          if (result <= 0) {
            request->stop();
            request->cleanup();
            return;
          }

          if (result > 0) {
            request->seek(request->offset + result);
          }

          if (request->reading) {
            request->read();
          }
        });
  }

  void stop() { reading = false; }

  void seek(unsigned const newOffset) { offset = newOffset; }

  ssize_t file;
  unsigned offset = 0;
  bool reading = true;
  uv_buf_t buffer;
  std::function<void(int, StringView)> onRead = [](auto...) {};
  std::function<void()> onReadEnd = [](auto...) {};
};

struct FSWrite : FS<FSWrite> {

  FSWrite(uv_loop_t *loop, ssize_t file, std::string const &msg)
      : FS<FSWrite>(loop), msg{msg} {
    write(loop, file);
  }

  ~FSWrite() {}

  int write(uv_loop_t *loop, ssize_t file) {
    uv_buf_t buf = uv_buf_init(&msg[0], msg.size());
    return uv_fs_write(loop, (uv_fs_t *)this, file, &buf, 1, 0, [](uv_fs_t *r) {
      // assert(r == this);
      int result = r->result;
      auto *request = (FSWrite *)r;
      request->onWrite(result);
      request->cleanup();
    });
  }

  std::string msg;
  std::function<void(int)> onWrite = [](int) {};
};

struct FSStat : FS<FSStat> {

  FSStat(uv_loop_t *loop, std::string const &path) : FS<FSStat>(loop) {
    stat(loop, path);
  }

  int stat(uv_loop_t *loop, std::string const &path) {
    return uv_fs_stat(loop, (uv_fs_t *)this, path.c_str(), [](uv_fs_t *r) {
      // assert(r == this);
      auto result = r->result;
      auto statbuf = r->statbuf;
      auto *request = (FSStat *)r;
      request->onStat(result, statbuf);
      request->cleanup();
    });
  }

  std::function<void(int, uv_stat_t)> onStat = [](int, uv_stat_t) {};
};

struct FSUTime : FS<FSUTime> {

  FSUTime(uv_loop_t *loop, std::string const &path, double atime, double mtime)
      : FS<FSUTime>(loop) {
    utime(loop, path, atime, mtime);
  }

  int utime(uv_loop_t *loop, std::string const &path, double atime,
            double mtime) {
    return uv_fs_utime(loop, (uv_fs_t *)this, path.c_str(), atime, mtime,
                       [](uv_fs_t *r) {
                         // assert(r == this);
                         int result = r->result;
                         auto *request = (FSUTime *)r;
                         request->onUtime(result);
                         request->cleanup();
                       });
  }

  std::function<void(int)> onUtime = [](int) {};
};

struct FSScanDir : FS<FSScanDir> {

  FSScanDir(uv_loop_t *loop, std::string const &path, int flags)
      : FS<FSScanDir>(loop) {
    scandir(loop, path, flags);
  }

  int scandir(uv_loop_t *loop, std::string const &path, int flags) {
    return uv_fs_scandir(
        loop, (uv_fs_t *)this, path.c_str(), flags, [](uv_fs_t *r) {
          auto result = r->result;
          auto *request = (FSScanDir *)r;
          request->onScandir(result);
          uv_dirent_t ent;
          while (true) {
            int i = uv_fs_scandir_next(r, &ent);
            auto keepGoing = request->onScandirNext(
                {fromUVDirent(ent.type), std::string(ent.name)});
            if (i == UV_EOF || !keepGoing) {
              break;
            }
          }
          request->cleanup();
        });
  }

  std::function<void(int)> onScandir = [](int) {};
  std::function<bool(DirectoryEntry)> onScandirNext = [](DirectoryEntry) {
    return false;
  };
};

struct FSRealPath : FS<FSRealPath> {

  FSRealPath(uv_loop_t *loop, std::string const &path) : FS<FSRealPath>(loop) {
    realpath(loop, path);
  }

  int realpath(uv_loop_t *loop, std::string const &path) {
    return uv_fs_realpath(loop, (uv_fs_t *)this, path.c_str(), [](uv_fs_t *r) {
      auto result = r->result;
      auto *request = (FSRealPath *)r;
      request->onRealpath((result < 0) ? result : 0,
                          {(char *)r->ptr, (char *)r->ptr + result});
      request->cleanup();
    });
  }

  std::function<void(int, StringView)> onRealpath = [](auto...) {};
};

struct FSUnlink : FS<FSUnlink> {

  FSUnlink(uv_loop_t *loop, std::string const &path) : FS<FSUnlink>(loop) {
    unlink(loop, path);
  }

  int unlink(uv_loop_t *loop, std::string const &path) {
    return uv_fs_unlink(loop, (uv_fs_t *)this, path.c_str(), [](uv_fs_t *r) {
      auto result = r->result;
      auto *request = (FSUnlink *)r;
      request->onUnlink(result);
      request->cleanup();
    });
  }

  std::function<void(int)> onUnlink = [](auto...) {};
};

struct FSPoll : uv_fs_poll_t {

  FSPoll(uv_loop_t *loop) { uv_fs_poll_init(loop, (uv_fs_poll_t *)this); }

  // (uv_fs_poll_t* handle,
  //                               int status,
  //                               const uv_stat_t* prev,
  //                               const uv_stat_t* curr)

  // UV_EXTERN int uv_fs_poll_start(uv_fs_poll_t* handle,
  //                                uv_fs_poll_cb poll_cb,
  //                                const char* path,
  //                                unsigned int interval);

  ~FSPoll() { stop(); }

  int start(std::string const &path, unsigned const interval) {
    return uv_fs_poll_start((uv_fs_poll_t *)this,
                            [](uv_fs_poll_t *handle, int status,
                               const uv_stat_t *prev, const uv_stat_t *curr) {
                              auto *fsPoll = (FSPoll *)handle;
                              fsPoll->onPoll(status, *prev, *curr);
                            },
                            path.c_str(), interval);
  }

  int stop() { return uv_fs_poll_stop((uv_fs_poll_t *)this); }

  std::function<void(int, uv_stat_t const &, uv_stat_t const &)> onPoll =
      [](auto...) {};
};

} // namespace satori

#endif
