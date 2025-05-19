#!/bin/bash

EMCC=emcc
SRC_DIR=src
OUT_DIR=web

mkdir -p $OUT_DIR

$EMCC -O3 \
  $SRC_DIR/wasm.c \
  $SRC_DIR/lex.c \
  $SRC_DIR/exec.c \
  $SRC_DIR/prints.c \
  -I$SRC_DIR \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS="['_init_environment', '_cleanup_environment', '_lex_code', '_get_tokens_string', '_execute_all', '_execute_step', '_get_stack_string']" \
  -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -o $OUT_DIR/cazal.js 