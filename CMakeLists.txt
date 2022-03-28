cmake_minimum_required(VERSION 3.10)

# set the project name
project(Bin2c)

# add the executable
include_directories(include)
add_executable(Bin2c 
					bin2c.c
)