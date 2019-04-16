@echo off
clang -g -std=c11 -Werror -Wall -Wextra -Wpedantic json.c test/json_test.c -I . -I test -D_CRT_SECURE_NO_WARNINGS -o json_test.exe
del *.ilk *.obj
