#ifndef SATORI_HTTP_PARSER_HPP
#define SATORI_HTTP_PARSER_HPP

#include <map>
#include <string>
#include <functional>

#include <http_parser.h>

namespace satori {

  struct HttpParser : private http_parser {

    http_parser_settings settings;

    using HttpCb = std::function<int(http_parser const*)>;
    using HttpDataCb = std::function<int(http_parser const*, const char*, size_t)>;

    HttpCb onMessageBegin = [](http_parser const*) { return 0; };
    HttpDataCb onUrl = [](http_parser const*, const char*, size_t) { return 0; };
    HttpDataCb onStatus = [](http_parser const*, const char*, size_t) { return 0; };
    HttpDataCb onHeaderField = [](http_parser const*, const char*, size_t) { return 0; };
    HttpDataCb onHeaderValue = [](http_parser const*, const char*, size_t) { return 0; };
    HttpCb onHeadersComplete = [](http_parser const*) { return 0; };
    HttpDataCb onBody = [](http_parser const*, const char*, size_t) { return 0; };
    HttpCb onMessageComplete = [](http_parser const*) { return 0; };
    HttpCb onChunkHeader = [](http_parser const*) { return 0; };
    HttpCb onChunkComplete = [](http_parser const*) { return 0; };

    HttpParser(http_parser_type type) {

      http_parser_init(this, type);

      http_parser_settings_init(&settings);

      settings.on_message_begin = [](http_parser* parser) {
        HttpParser* self = (HttpParser*)parser;
        return self->onMessageBegin(parser);
      };

      settings.on_url = [](http_parser* parser, const char* at, size_t length) {
        HttpParser* self = (HttpParser*)parser;
        return self->onUrl(parser, at, length);
      };

      settings.on_status = [](http_parser* parser, const char* at, size_t length) {
        HttpParser* self = (HttpParser*)parser;
        return self->onStatus(parser, at, length);
      };

      settings.on_header_value = [](http_parser* parser, const char* at, size_t length) {
        HttpParser* self = (HttpParser*)parser;
        return self->onHeaderValue(parser, at, length);
      };

      settings.on_headers_complete = [](http_parser* parser) {
        HttpParser* self = (HttpParser*)parser;
        return self->onHeadersComplete(parser);
      };

      settings.on_body = [](http_parser* parser, const char* at, size_t length) {
        HttpParser* self = (HttpParser*)parser;
        return self->onBody(parser, at, length);
      };

      settings.on_message_complete = [](http_parser* parser) {
        HttpParser* self = (HttpParser*)parser;
        return self->onMessageComplete(parser);
      };

      settings.on_chunk_header = [](http_parser* parser) {
        HttpParser* self = (HttpParser*)parser;
        return self->onChunkHeader(parser);
      };

      settings.on_chunk_complete = [](http_parser* parser) {
        HttpParser* self = (HttpParser*)parser;
        return self->onChunkComplete(parser);
      };
    }

    size_t execute(char const* data, size_t length) {
      return http_parser_execute(this, &settings, data, length);
    }

    size_t execute(std::string const& data) {
      return execute(data.c_str(), strlen(data.c_str()));
    }
  };

}

#endif
