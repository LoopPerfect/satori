/*
typedef void (*uv_alloc_cb)(uv_handle_t* handle,
                            size_t suggested_size,
                            uv_buf_t* buf);
typedef void (*uv_read_cb)(uv_stream_t* stream,
                           ssize_t nread,
                           const uv_buf_t* buf);
typedef void (*uv_write_cb)(uv_write_t* req, int status);
typedef void (*uv_connect_cb)(uv_connect_t* req, int status);
typedef void (*uv_shutdown_cb)(uv_shutdown_t* req, int status);
typedef void (*uv_connection_cb)(uv_stream_t* server, int status);
typedef void (*uv_close_cb)(uv_handle_t* handle);
typedef void (*uv_poll_cb)(uv_poll_t* handle, int status, int events);
typedef void (*uv_timer_cb)(uv_timer_t* handle);
typedef void (*uv_async_cb)(uv_async_t* handle);
typedef void (*uv_prepare_cb)(uv_prepare_t* handle);
typedef void (*uv_check_cb)(uv_check_t* handle);
typedef void (*uv_idle_cb)(uv_idle_t* handle);
typedef void (*uv_exit_cb)(uv_process_t*, int64_t exit_status, int term_signal);
typedef void (*uv_walk_cb)(uv_handle_t* handle, void* arg);
typedef void (*uv_fs_cb)(uv_fs_t* req);
typedef void (*uv_work_cb)(uv_work_t* req);
typedef void (*uv_after_work_cb)(uv_work_t* req, int status);
typedef void (*uv_getaddrinfo_cb)(uv_getaddrinfo_t* req,
                                  int status,
                                  struct addrinfo* res);
typedef void (*uv_getnameinfo_cb)(uv_getnameinfo_t* req,
                                  int status,
                                  const char* hostname,
                                  const char* service);
*/

/* HANDLE

int uv_is_active(const uv_handle_t* handle);
int uv_is_closing(const uv_handle_t* handle);
void uv_close(uv_handle_t* handle, uv_close_cb close_cb);

void uv_ref(uv_handle_t* handle);
void uv_unref(uv_handle_t* handle);
int uv_has_ref(const uv_handle_t* handle);
size_t uv_handle_size(uv_handle_type type);
int uv_send_buffer_size(uv_handle_t* handle, int* value);
int uv_recv_buffer_size(uv_handle_t* handle, int* value);
int uv_fileno(const uv_handle_t* handle, uv_os_fd_t* fd);

*/

/* STREAM

int uv_shutdown(uv_shutdown_t* req, uv_stream_t* handle, uv_shutdown_cb cb);
int uv_listen(uv_stream_t* stream, int backlog, uv_connection_cb cb);
int uv_accept(uv_stream_t* server, uv_stream_t* client);

int uv_read_start(uv_stream_t* stream, uv_alloc_cb alloc_cb, uv_read_cb
read_cb); int uv_read_stop(uv_stream_t*);

int uv_write(uv_write_t* req, uv_stream_t* handle, const uv_buf_t bufs[],
unsigned int nbufs, uv_write_cb cb);

int uv_write2(uv_write_t* req, uv_stream_t* handle, const uv_buf_t bufs[],
unsigned int nbufs, uv_stream_t* send_handle, uv_write_cb cb);

int uv_try_write(uv_stream_t* handle, const uv_buf_t bufs[], unsigned int
nbufs);

int uv_is_readable(const uv_stream_t* handle);
int uv_is_writable(const uv_stream_t* handle);

int uv_stream_set_blocking(uv_stream_t* handle, int blocking);

*/

/*
template<bool inplace, class T, class F>
callbackFor(T* uv, F f) {
  sizeof(F)
};

template<class OnClose, class OnRead, class OnListen>
struct TcpCallbacks {
  OnClose onClose;
  OnRead onRead;
  OnListen onListen;
};

template<class UVAlloc>
struct Tcp : uv_tcp_t {
  Tcp(uv_loop_t* loop) {
    uv_tcp_init(loop, (uv_tcp_t*)this);
  o}

  template<class CB>
  int start(CB&&cb) {
    uv_read_start(
      (uv_stream_t*)this,
      uv_alloc_cb, [](
        uv_stream_t* stream,
        ssize_t nread,
        const uv_buf_t* buf) {

    })
  }

  static uv_alloc_cb;
};

*/
