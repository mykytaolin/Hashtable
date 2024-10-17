cmake_minimum_required(VERSION 3.27)
project(HashTableinC C)

set(CMAKE_C_STANDARD 11)

add_executable(HashTableinC src/main.c
        src/hash_table.c
        src/hash_table.h
        src/prime.c
        src/prime.h)
