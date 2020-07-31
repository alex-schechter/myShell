#!/bin/sh
while read line; do
    VAR1="Hello,"
    VAR3="$VAR1$line"
    echo "$VAR3"
done