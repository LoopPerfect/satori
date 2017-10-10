#ifndef SATORI_SATORI_HPP
#define SATORI_SATORI_HPP

#include <memory>
#include <string>
#include <uv.h>

#include <satori/actor.hpp>
#include <satori/deflate.hpp>
#include <satori/deflator.hpp>
#include <satori/dns.hpp>
#include <satori/directoryEntry.hpp>
#include <satori/enableMultiProcess.hpp>
#include <satori/errors.hpp>
#include <satori/handles.hpp>
#include <satori/httpParser.hpp>
#include <satori/inflate.hpp>
#include <satori/inflator.hpp>
#include <satori/loop.hpp>
#include <satori/managed_ptr.hpp>
#include <satori/promise.hpp>
#include <satori/recycler.hpp>
#include <satori/requests.hpp>
#include <satori/router.hpp>
#include <satori/url-parser.hpp>
#include <satori/url.hpp>
#include <satori/urlDecode.hpp>
#include <satori/urlEncode.hpp>
#include <satori/stringview.hpp>
#include <satori/runMode.hpp>

namespace satori {

struct Satori {

private:
  std::shared_ptr<Loop> loop;

public:
  Satori() : loop(std::make_shared<Loop>()) {}

  ~Satori() {}

  template <typename T>
  Promise<T> promise() {
    return Promise<T>(std::weak_ptr<Loop>(loop));
  }

  managed_ptr<FSScanDir>
  scanDirectory(std::string const& path,
                std::function<void(int const)> const& onScan = [](auto...) {},
                std::function<bool(DirectoryEntry const&)> const& onNext =
                  [](auto...) { return false; }) {
    auto fs = loop->newFSScanDir(path, 0);
    fs->onScandir = onScan;
    fs->onScandirNext = onNext;
    return fs;
  }

  managed_ptr<FSRealPath>
  realPath(std::string const& path,
    std::function<void(int, StringView)> const& onRealPath
  ) {
    auto fs = loop->newFSRealPath(path);
    fs->onRealpath = onRealPath;
    return fs;
  }

  /**
   *  Opens the file at the given Unix path.
   */
  managed_ptr<FSOpen> openFile(std::string const& path, int flags, int mode, std::function<void(size_t)> const& onOpen) {
    auto fs = loop->newFSOpen(path, flags, mode);
    fs->onOpen = onOpen;
    return fs;
  }

  managed_ptr<FSRead> readFile(size_t const fileDescriptor, std::function<void(int const, StringView const)> const& onRead, std::function<void()> const& onReadEnd) {
    auto fs = loop->newFSRead(fileDescriptor);
    fs->onRead = onRead;
    fs->onReadEnd = onReadEnd;
    fs->read();
    return fs;
  }

  managed_ptr<FSClose> closeFile(size_t const fileDescriptor) {
    auto fs = loop->newFSClose(fileDescriptor);
    return fs;
  }

  managed_ptr<FSUnlink> unlink(std::string const& path, std::function<void(int)> const& onUnlink) {
    auto fs = loop->newFSUnlink(path);
    fs->onUnlink = onUnlink;
    return fs;
  }

  void run(RunMode const mode = RunMode::Default) { loop->run(mode); }
};

} // namespace satori

#endif
