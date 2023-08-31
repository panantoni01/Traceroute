#!/bin/bash

TOOL=clang-format-14

MODIFY=-n
if [ "$1" == "--fix" ]; then
  MODIFY=""
fi

$TOOL $MODIFY -style=file --Werror -i $(find . -name "*.[ch]")

if [ $? -ne 0 ]; then
  echo "Incorrect formatting!"
  exit 1
fi

echo "Format check OK"
