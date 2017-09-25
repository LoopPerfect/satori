#ifndef SATORI_HTTP_PARSER_HPP
#define SATORI_HTTP_PARSER_HPP

#include <map>
#include <string>

namespace satori {

  std::map<std::string, std::string> parseReq(std::string const& str) {

    std::map<std::string, std::string> obj;

    std::string key;
    std::string val;

    int state = 0;

    key = "method";

    for (auto const& c : str) {
      switch (state) {
        case 0:
          if (c != ' ') {
            val += c;
          } else {
            obj[key] = val;
            state = 1;
            val = "";
            key = "path";
          }
          continue;
        case 1:
          if (c != ' ') {
            val += c;
          } else {
            obj[key] = val;
            val = "";
            key = "protocol";
            state = 2;
          }
          continue;
        case 2:
          if (c != '\n' || c != '\r') {
            val += c;
          } else {
            obj[key] = val;
            key = "";
            val = "";
            state = 4;
          }
          continue;

        case 3:
          switch (c) {
            case ' ':
              continue;
            case '\r':
              continue;
            case '\n':
              obj[key] = val;
              state = 4;
              key = "";
              continue;
            default:
              val += c;
              continue;
          }

        case 4:
        case 5:
          switch (c) {
            case '\r':
              continue;
            case '\n':
              ++state;
              key = "";
              continue;
            case ':':
              state = 3;
              continue;
            case ' ':
              continue;
            default:
              key += c;
              val = "";
              continue;
          }

        default:
          return obj;
      }
    }
    return obj;
  }
}

#endif
