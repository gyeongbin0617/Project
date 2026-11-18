#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
if [[ "${MSYSTEM:-}" == "UCRT64" || "${MSYSTEM:-}" == "MINGW64" ]]; then
  gcc -std=c11 -O2 -Wall -Wextra -Werror -Iinclude -Ivendor/include/SDL2 src/*.c -Lvendor/lib -lSDL2_image -lSDL2_ttf -lSDL2 -lm -static-libgcc -mwindows -o NumberExplorers_new.exe
  mv -f NumberExplorers_new.exe NumberExplorers.exe
  echo "Build OK: NumberExplorers.exe"
else
  gcc -std=c11 -O2 -Wall -Wextra -Werror -Iinclude src/*.c $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf) -lm -o NumberExplorers
  echo "Build OK: ./NumberExplorers"
fi
