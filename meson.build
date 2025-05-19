# meson.build
project('cazal', 'c')

src_files = run_command('sh', '-c', 'ls src/*.c | grep -v wasm.c', check: true).stdout().strip().split('\n')

executable('cazal', 
  src_files,
  include_directories: include_directories('src')
)

# Add a custom target for WASM build if the emcc compiler is available
emcc = find_program('emcc', required: false)
if emcc.found()
  custom_target('wasm_js',
    output: ['cazal.js', 'cazal.wasm'],  # Simplified output names
    input: src_files + ['src/wasm.c'],
    command: [
      'emcc', '-O2', '@INPUT@', 
      '-I' + meson.project_source_root() + '/src',
      '-s', 'WASM=1',
      '-s', 'EXPORTED_FUNCTIONS=["_init_environment", "_cleanup_environment", "_lex_code", "_get_tokens_string", "_execute_all", "_execute_step", "_get_stack_string", "_get_error_state", "_reset_error_state", "_main"]',
      '-s', 'EXPORTED_RUNTIME_METHODS=["ccall", "cwrap"]',
      '-s', 'ALLOW_MEMORY_GROWTH=1',
      '-o', '@OUTPUT0@'
    ],
    build_by_default: false,
    install: false,
  )
  
  custom_target('wasm_standalone',
    output: ['cazal_standalone.wasm'],
    input: src_files + ['src/wasm.c'],
    command: [
      'emcc', '-O2', '@INPUT@', 
      '-I' + meson.project_source_root() + '/src',
      '-s', 'WASM=1',
      '-s', 'STANDALONE_WASM',
      '-o', '@OUTPUT0@'
    ],
    build_by_default: false,
    install: false,
  )
endif
