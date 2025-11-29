#!/bin/bash

count=0

handler(){
  count=$((count + 1))
  if [ "$count" -ge 3 ]; then
    dir="saves/saved_files_$(date +%Y%m%d_%H%M%S)"
    mkdir "$dir"
    cp "$file1" "$file2" "$dir"
    echo "Files saved to $dir"
    exit
  fi
}

read -p "Enter first file: " file1
read -p "Enter second file: " file2


if [[ ! -f "$file1" || ! -f "$file2" ]]; then
  if [[ ! -f "$file1" && ! -f "$file2" ]]; then
    echo "Both files not found"
    exit 1
  else
    if [[ ! -f "$file1" ]]; then
      echo "First file not found"
      exit 1
    else
      echo "Second file not found"
      exit 1
    fi
  fi
fi

if [[ ! -f "$file2" ]]; then
  exit 1
fi

(
  if cmp -s "$file1" "$file2"; then
    echo "Files are identical"
  else
    echo "Files are different"
  fi
) | (
  while read line; do
    echo "$line"

    echo "Place taken:"
    du "$file1"
    du "$file2"
  done
)

stty -echoctl
trap handler SIGINT
echo "Press Ctrl+C 3 times to save, press anything else to exit..."
read -n 1
stty echoctl

exit 0