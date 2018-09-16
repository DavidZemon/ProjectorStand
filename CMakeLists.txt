cmake_minimum_required(VERSION 3.11)
find_package(PropWare REQUIRED)

project(ProjectorStand)

set(MODEL cmm)

create_executable(ProjectorStand src/main/main.cpp)

create_test(CurrentWatchDogTest src/test/CurrentWatchDogTest.cpp)
target_include_directories(CurrentWatchDogTest PRIVATE src/main)
