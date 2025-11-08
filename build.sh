rm -rf bin
mkdir bin
# Borrowed from stackoverflow
# https://stackoverflow.com/a/59621930
g++ -O3 -g $(find src -type f -iregex ".*\.cpp") -o bin/main
cp demos/* bin
