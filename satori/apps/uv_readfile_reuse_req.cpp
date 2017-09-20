#include <uv.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string>

uv_loop_t* loop;

uv_fs_t all_req;

uv_buf_t buffer;
char data[1024];

void open_cb(uv_fs_t* req);
void read_cb(uv_fs_t* req);
void close_cb(uv_fs_t* req);

std::string error_to_string(int error) {
  return std::string(uv_err_name(error)) +
    " " +
    std::string(uv_strerror(error));
}

int main(int argc, const char ** argv) {

  loop = new uv_loop_t();

  uv_loop_init(loop);

  if (!argv[1]) {
    std::cout << "Please pass a filename as argument. " << std::endl;
    return 1;
  }

  ssize_t uv_fs_open_result = uv_fs_open(
    loop,
    &all_req,
    argv[1],
    O_RDONLY,
    S_IRUSR,
    open_cb);

  if (uv_fs_open_result < 0) {
    std::cerr << error_to_string(uv_fs_open_result) << std::endl;
    exit(1);
  }

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}

void open_cb(uv_fs_t* req) {

  assert(req == &all_req);

  ssize_t file = req->result;

  uv_fs_req_cleanup(req);

  buffer = uv_buf_init(data, sizeof(data));

  int uv_fs_read_result = uv_fs_read(
    loop,
    &all_req,
    file,
    &buffer,
    1, // buffer.len,
    0,
    read_cb);

  if (uv_fs_read_result < 0) {
    std::cerr << error_to_string(uv_fs_read_result) << std::endl;
    exit(1);
  }
}

void read_cb(uv_fs_t* req) {

  assert(req == &all_req);

  ssize_t result = req->result;

  uv_fs_req_cleanup(req);

  if (result < 0) {
    std::cerr << error_to_string(result) << std::endl;
    exit(1);
  } else if (result > 0) {
    std::cout << data << std::endl;
  } else if (result == 0) {
    uv_file file = result;
    std::cout << "file " << file << std::endl;
    int uv_fs_close_result = uv_fs_close(loop, &all_req, file, close_cb);

    if (uv_fs_close_result < 0) {
      std::cerr << error_to_string(uv_fs_close_result) << std::endl;
      exit(1);
    }
  }
}

void close_cb(uv_fs_t* req) {

  assert(req == &all_req);

  ssize_t result = req->result;

  uv_fs_req_cleanup(req);

  if (result < 0) {
    std::cerr << error_to_string(result) << std::endl;
    exit(1);
  }
}
