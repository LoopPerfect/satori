# Generated by Buckaroo - https://buckaroo.pm
include_defs('//BUCKAROO_DEPS')

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
  deps = BUCKAROO_DEPS,
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
  visibility = [
    'PUBLIC',
  ],
)

cxx_binary(
  name = 'satori_readfile',
  srcs = [
    'satori/apps/satori_readfile.cpp',
  ],
  deps = [
    ':satori',
  ],
  visibility = [
    'PUBLIC',
  ],
)

cxx_binary(
  name = 'uv_readfile',
  srcs = [
    'satori/apps/uv_readfile.cpp',
  ],
  deps = BUCKAROO_DEPS,
  visibility = [
    'PUBLIC',
  ],
)

cxx_binary(
  name = 'uv_readfile_reuse_req',
  srcs = [
    'satori/apps/uv_readfile_reuse_req.cpp',
  ],
  deps = BUCKAROO_DEPS,
  visibility = [
    'PUBLIC',
  ],
)

cxx_binary(
  name = 'satori_writefile',
  srcs = [
    'satori/apps/satori_writefile.cpp',
  ],
  deps = BUCKAROO_DEPS + [
    ':satori',
  ],
  visibility = [
    'PUBLIC',
  ],
)

cxx_binary(
  name = 'satori_utime',
  srcs = [
    'satori/apps/satori_utime.cpp',
  ],
  deps = BUCKAROO_DEPS + [
    ':satori',
  ],
  visibility = [
    'PUBLIC',
  ],
)
