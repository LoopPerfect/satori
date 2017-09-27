#ifndef SATORI_URL_PARSER_HPP
#define SATORI_URL_PARSER_HPP

#include <http_parser.h>
#include <neither/either.hpp>
#include <string>
#include <vector>

#include <satori/url.hpp>

namespace satori {

neither::Either<int, Url> parseUrl(std::string const& maybeUrl) {

  http_parser_url parser;

  http_parser_url_init(&parser);

  int const len = strlen(maybeUrl.c_str());
  int const connect = 0;

  int const result =
    http_parser_parse_url(maybeUrl.c_str(), len, connect, &parser);

  // Error?
  if (result != 0) {
    return neither::Either<int, Url>::leftOf(result);
  }

  auto const port = parser.port;
  auto parts = std::vector<std::string>();

  for (unsigned int i = 0; i < UF_MAX; i++) {
    if ((parser.field_set & (1 << i)) == 0) {
      continue;
    }

    auto part = std::string(maybeUrl.c_str() + parser.field_data[i].off,
                            parser.field_data[i].len);

    parts.push_back(part);
  }

  return neither::Either<int, Url>::rightOf({port, parts});
}

} // namespace satori

#endif
