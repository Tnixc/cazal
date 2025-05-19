run:
    meson compile -C out && echo "" && ./out/cazal
    
build:
     meson compile -C out

wasm:
    ./build_wasm.sh

serve:
    cd web && python -m http.server 8000
