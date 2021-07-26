#[=======================================================================[
SPDX-License-Identifier: MIT
SPDX-FileCopyrightText: 2021 Avinal Kumar <avinal.xlvii@gmail.com>
#]=======================================================================]

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(c_socket 
    LANGUAGES C
    DESCRIPTION "Simple client-server program in C to send files from client to server."
    HOMEPAGE_URL "https://github.com/avinal/seventh-semester"
    VERSION 1.0.0)

if(WIN32 OR APPLE)
    message(ERROR "Please use a linux computer to compile this project!!")
endif()
add_executable(client client.c)
add_executable(server server.c)
