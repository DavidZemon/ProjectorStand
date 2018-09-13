cmake_minimum_required(VERSION 3.11)
find_package(PropWare REQUIRED)

project(ProjectorStand)

set(MODEL cmm)

create_executable(ProjectorStand main.cpp)
