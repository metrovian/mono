# atiny85 project
project(
  'mono-atiny85',
  'c',
  version : '1.0.0',
  default_options: [
    'c_std=c99',
    'warning_level=2',
    'buildtype=release'])

src_atiny85 = files(
  'mono/mono.c',
  'main.c')

elf_atiny85 = executable(
  'mono-atiny85.elf',
  src_atiny85,
  include_directories: '.')

custom_target(
  'mono-atiny85.hex',
  input : elf_atiny85,
  output : 'mono-atiny85.hex',
  command : [find_program('avr-objcopy'), '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
  build_by_default : true)