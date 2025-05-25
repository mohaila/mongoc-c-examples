#!/bin/bash

DIRECTORY=$1
if [ -z "$DIRECTORY" ]; then
  DIRECTORY=.
fi
find "$DIRECTORY" -type f -iname "*.h" -o -iname "*.hpp" -o -iname "*.c" -o -iname "*.cpp" | while read file; do
  clang-format -style=file -i "$file"
  echo "$file formatted."
done