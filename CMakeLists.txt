cmake_minimum_required(VERSION 3.29)
project(SD_Card_Library DESCRIPTION "Library for SD Cards using SPI.")
add_library(SD_Card_Library INTERFACE)

target_include_directories(SD_Card_Library INTERFACE
    inc
    src
)

target_sources(SD_Card_Library INTERFACE
    src/sd_card.c
    inc/sd_card.h
)
