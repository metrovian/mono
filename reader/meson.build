# mono reader
src_reader = files(
  'source/mono.c',
  'test/main.c')

executable(
  'mono',
  src_reader,
  dependencies: dependency('alsa', required: true),
  link_args: ['-lm'],
  include_directories: 'include')