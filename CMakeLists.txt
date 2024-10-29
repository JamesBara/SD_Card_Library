cmake_minimum_required(VERSION 3.29)
project(SD_card_Library DESCRIPTION "Library for W25Qxx NOR Flash.")
add_library(SD_card_Library INTERFACE)

target_include_directories(SD_card_Library INTERFACE
    inc
    src
)

target_sources(SD_card_Library INTERFACE
    src/sd_card.c
    inc/sd_card.h
)
