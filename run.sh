#/usr/bin/sh

gcc ./src/main.c -Wall -Wswitch-enum -Wextra \
-lraylib \
-o pong && ./pong
