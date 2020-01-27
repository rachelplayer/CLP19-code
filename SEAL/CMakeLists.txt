cmake_minimum_required(VERSION 3.12)

project(NoiseTest)

find_package(SEAL 3.4 REQUIRED)

add_executable(noisetest examples-invariant.cpp)
target_link_libraries(noisetest SEAL::seal)

add_executable(noisetest_batched examples-random-batch.cpp)
target_link_libraries(noisetest_batched SEAL::seal)
