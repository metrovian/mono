# mono reader
src_reader = files(
  'source/mono.c',
  'test/alsa/beep.c',
  'test/main.c')

test_reader = executable(
  'mono',
  src_reader,
  dependencies: dependency('alsa', required: true),
  link_args: ['-lm'],
  include_directories: 'include')

test('test_reader', test_reader)