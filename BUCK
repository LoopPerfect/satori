load('//:subdir_glob.bzl', 'subdir_glob')
load('//:buckaroo_macros.bzl', 'buckaroo_deps')

cxx_library(
  name = 'satori',
  header_namespace = 'satori',
  exported_headers = subdir_glob([
    ('satori/include', '**/*.hpp'),
  ]),
  headers = subdir_glob([
    ('satori/detail', '**/*.hpp'),
  ]),
  srcs = glob([
    'satori/src/**/*.cpp',
  ]),
  deps = buckaroo_deps(), 
  visibility = [
    'PUBLIC',
  ],
)

cxx_binary(
  name = 'main',
  srcs = [
    'satori/apps/main.cpp',
  ],
  deps = [
    ':satori',
  ],
)

cxx_binary(
  name = 'uv_readfile',
  srcs = [
    'satori/apps/uv_readfile.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'uv_readfile_reuse_req',
  srcs = [
    'satori/apps/uv_readfile_reuse_req.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'satori_writefile',
  srcs = [
    'satori/apps/satori_writefile.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'satori_getrequest',
  srcs = [
    'satori/apps/satori_getrequest.cpp',
  ],
  deps = [
    ':satori', 
  ],
)


cxx_binary(
  name = 'satori_utime',
  srcs = [
    'satori/apps/satori_utime.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'satori_stat',
  srcs = [
    'satori/apps/satori_stat.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'satori_process',
  srcs = [
    'satori/apps/satori_process.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'uv_process',
  srcs = [
    'satori/apps/uv_process.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'nodejs_parse_http',
  srcs = [
    'satori/apps/nodejs_parse_http.cpp',
  ],
  deps = [
    ':satori', 
  ],
)

cxx_binary(
  name = 'satori_parse_http',
  srcs = [
    'satori/apps/satori_parse_http.cpp',
  ],
  deps = [
    ':satori',
  ],
)

cxx_binary(
  name = 'satori_filestream',
  srcs = [
    'satori/apps/satori_filestream.cpp',
  ],
  deps = [
    ':satori',
  ],
)

cxx_binary(
  name = 'r3_router',
  srcs = [
    'satori/apps/r3_router.cpp',
  ],
  deps = [
    ':satori',
  ],
)

cxx_binary(
  name = 'satori_router',
  srcs = [
    'satori/apps/satori_router.cpp',
  ],
  deps = [
    ':satori',
  ],
)

cxx_binary(
  name = 'satori_express',
  srcs = [
    'satori/apps/satori_express.cpp',
  ],
  deps = [
    ':satori',
  ],
)

cxx_binary(
  name = 'uv_dns',
  srcs = [
    'satori/apps/uv_dns.cpp',
  ],
  deps = [
    ':satori', 
  ],
)
