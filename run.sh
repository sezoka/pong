#/usr/bin/sh

gcc ./src/main.c -O3 -Wall -Wswitch-enum -Wextra \
-lraylib \
-o pong && ./pong
