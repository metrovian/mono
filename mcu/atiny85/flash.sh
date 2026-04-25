#!/bin/bash
set -eu
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
build_dir="${script_dir}/builddir"
cross_file="${script_dir}/meson-avr.cross"
hex_path="${build_dir}/mono-atiny85.hex"
header_path="${script_dir}/example.h"

if [ "$#" -gt 1 ]; then
  echo "usage: $0 [input.abc]" >&2
  exit 64
fi

if [ "$#" -eq 1 ]; then
  abc_path=$1
  if [ ! -f "$abc_path" ]; then
    echo "abc file not found: $abc_path" >&2
    exit 66
  fi
  if ! command -v mono >/dev/null 2>&1; then
    echo "mono command not found; run meson install for writer first" >&2
    exit 69
  fi
  mono "$abc_path" "$header_path"
fi

if [ ! -f "${build_dir}/build.ninja" ]; then
  meson setup "$build_dir" --cross-file "$cross_file"
fi

meson compile -C "$build_dir"
if [ ! -f "$hex_path" ]; then
  echo "hex file not found: $hex_path" >&2
  exit 66
fi

avrdude -p t85 \
  -c linuxspi \
  -P /dev/spidev0.0:/dev/gpiochip0 \
  -U "flash:w:${hex_path}:i"
