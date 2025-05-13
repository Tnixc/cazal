run:
    meson compile -C out && echo "" && ./out/ccore
    
build:
     meson compile -C out
