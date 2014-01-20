#!/bin/sh
FILE=$1
TAG=$2
VALUE=$3

cat "$FILE" | sed "s/$TAG/$VALUE/g"  >"${FILE}".tmp && mv "${FILE}".tmp "${FILE}"
