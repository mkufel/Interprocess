cmake_minimum_required(VERSION 3.8)
project(interprocess)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        common.h
        farmer.c
        md5s.c
        md5s.h
        md5s_main.c
        settings.h
        uint128.h
        worker.c)

add_executable(interprocess ${SOURCE_FILES})