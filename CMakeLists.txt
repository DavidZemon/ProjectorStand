cmake_minimum_required(VERSION 3.11)
find_package(PropWare REQUIRED)

project(ProjectorStand)

set(MODEL lmm)

create_executable(ProjectorStand src/main/main.cpp)

create_test(CurrentWatchDogTest src/test/CurrentWatchDogTest.cpp)
target_include_directories(CurrentWatchDogTest PRIVATE src/main)

create_test(ButtonReaderTest src/test/ButtonReaderTest.cpp)
target_include_directories(ButtonReaderTest PRIVATE src/main)

create_test(MotorDriverTest src/test/MotorDriverTest.cpp)
target_include_directories(MotorDriverTest PRIVATE src/main)
