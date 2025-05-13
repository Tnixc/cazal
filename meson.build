# meson.build
project('ccore', 'c')

src_files = run_command('sh', '-c', 'ls src/*.c', check: true).stdout().strip().split('\n')

executable('ccore', 
  src_files,
  include_directories: include_directories('src')
)
