#ifndef SATORI_HTTP_PARSER_2_HPP
#define SATORI_HTTP_PARSER_2_HPP

#include <map>
#include <string>

#include <http_parser.h>

namespace satori {

  void parseHttpRequest(std::string const& x) {

    http_parser parser;
    http_parser_settings settings;

    http_parser_init(&parser, HTTP_REQUEST);

    // size_t http_parser_execute(http_parser *parser,
    //                            const http_parser_settings *settings,
    //                            const char *data,
    //                            size_t len);

    size_t const len = strlen(x.c_str());

    size_t parsed = http_parser_execute(&parser, &settings, x.c_str(), len);

    // Success?
    if (parsed == len) {

    }
  }

}

#endif
