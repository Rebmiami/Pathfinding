rm -rf bin
mkdir bin
em++ -s USE_SDL=2 -s USE_SDL_TTF=2 $(find src -type f -iregex ".*\.cpp") -o bin/main.html --preload-file demos --preload-file assets
cp -r demos bin
cp -r assets bin
