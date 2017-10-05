#ifndef SATORI_DIRECTORY_ENTRY_HPP
#define SATORI_DIRECTORY_ENTRY_HPP

#include <string>
#include <uv.h>

namespace satori {

// Satori equivalent of uv_dirent_type_t
// An enum class is safer than a C enum
enum class DirectoryEntryType {
  Unknown,
  File,
  Directory,
  Link,
  Fifo,
  Socket,
  Char,
  Block
};

DirectoryEntryType fromUVDirent(uv_dirent_type_t const& type) {
  switch (type) {
  case UV_DIRENT_FILE:
    return DirectoryEntryType::File;
  case UV_DIRENT_DIR:
    return DirectoryEntryType::Directory;
  case UV_DIRENT_LINK:
    return DirectoryEntryType::Link;
  case UV_DIRENT_FIFO:
    return DirectoryEntryType::Fifo;
  case UV_DIRENT_SOCKET:
    return DirectoryEntryType::Socket;
  case UV_DIRENT_CHAR:
    return DirectoryEntryType::Char;
  case UV_DIRENT_BLOCK:
    return DirectoryEntryType::Block;
  case UV_DIRENT_UNKNOWN:
  default:
    return DirectoryEntryType::Unknown;
  }
}

std::string directoryEntryTypeToString(DirectoryEntryType const& type) {
  switch (type) {
  case DirectoryEntryType::File:
    return "File";
  case DirectoryEntryType::Directory:
    return "Directory";
  case DirectoryEntryType::Link:
    return "Link";
  case DirectoryEntryType::Fifo:
    return "Fifo";
  case DirectoryEntryType::Socket:
    return "Socket";
  case DirectoryEntryType::Char:
    return "Char";
  case DirectoryEntryType::Block:
    return "Block";
  case DirectoryEntryType::Unknown:
  default:
    return "Unknown";
  }
}

// Satori equivalent of uv_dirent_s
// std::string is safer than a char*
struct DirectoryEntry {
  DirectoryEntryType type;
  std::string name;
};

} // namespace satori

#endif
