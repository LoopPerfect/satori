#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <http_parser.h>

int on_message_begin(http_parser *) {
  printf("\n***MESSAGE BEGIN***\n\n");
  return 0;
}

int on_headers_complete(http_parser *) {
  printf("\n***HEADERS COMPLETE***\n\n");
  return 0;
}

int on_message_complete(http_parser *) {
  printf("\n***MESSAGE COMPLETE***\n\n");
  return 0;
}

int on_url(http_parser *, const char *at, size_t length) {
  printf("Url: %.*s\n", (int)length, at);
  return 0;
}

int on_header_field(http_parser *, const char *at, size_t length) {
  printf("Header field: %.*s\n", (int)length, at);
  return 0;
}

int on_header_value(http_parser *, const char *at, size_t length) {
  printf("Header value: %.*s\n", (int)length, at);
  return 0;
}

int on_body(http_parser *, const char *at, size_t length) {
  printf("Body: %.*s\n", (int)length, at);
  return 0;
}

void usage(const char *name) {
  fprintf(stderr,
          "Usage: %s $type $filename\n"
          "  type: -x, where x is one of {r,b,q}\n"
          "  parses file as a Response, reQuest, or Both\n",
          name);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

  enum http_parser_type file_type;

  if (argc != 3) {
    usage(argv[0]);
  }

  char *type = argv[1];
  if (type[0] != '-') {
    usage(argv[0]);
  }

  switch (type[1]) {
  /* in the case of "-", type[1] will be NUL */
  case 'r':
    file_type = HTTP_RESPONSE;
    break;
  case 'q':
    file_type = HTTP_REQUEST;
    break;
  case 'b':
    file_type = HTTP_BOTH;
    break;
  default:
    usage(argv[0]);
  }

  char *filename = argv[2];
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("fopen");
    fclose(file);
    return EXIT_FAILURE;
  }

  fseek(file, 0, SEEK_END);
  long file_length = ftell(file);
  if (file_length == -1) {
    perror("ftell");
    fclose(file);
    return EXIT_FAILURE;
  }
  fseek(file, 0, SEEK_SET);

  char *data = new char[file_length];
  if (fread(data, 1, file_length, file) != (size_t)file_length) {
    fprintf(stderr, "couldn't read entire file\n");
    free(data);
    fclose(file);
    return EXIT_FAILURE;
  }

  http_parser_settings settings;
  memset(&settings, 0, sizeof(settings));
  settings.on_message_begin = on_message_begin;
  settings.on_url = on_url;
  settings.on_header_field = on_header_field;
  settings.on_header_value = on_header_value;
  settings.on_headers_complete = on_headers_complete;
  settings.on_body = on_body;
  settings.on_message_complete = on_message_complete;

  http_parser parser;
  http_parser_init(&parser, file_type);
  size_t nparsed = http_parser_execute(&parser, &settings, data, file_length);
  free(data);

  if (nparsed != (size_t)file_length) {
    fprintf(stderr, "Error: %s (%s)\n",
            http_errno_description(HTTP_PARSER_ERRNO(&parser)),
            http_errno_name(HTTP_PARSER_ERRNO(&parser)));
    fclose(file);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
