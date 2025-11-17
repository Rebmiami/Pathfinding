rm -rf bin
mkdir bin
em++ $(find src -type f -iregex ".*\.cpp") -o bin/main.html --preload-file demos --preload-file assets -O3 -sUSE_SDL=2 -sUSE_SDL_TTF=2 -pthread -sPTHREAD_POOL_SIZE=16 -sNO_DISABLE_EXCEPTION_CATCHING
cp -r demos bin
cp -r assets bin
