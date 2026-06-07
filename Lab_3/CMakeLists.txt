cmake_minimum_required(VERSION 3.16)
project(Lab3a_ParallelMergeSort CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/W4)

find_package(Threads REQUIRED)

add_library(sorter_lib
    sort/SequentialMergeSort.cpp
    sort/ParallelMergeSort.cpp
)
target_include_directories(sorter_lib PUBLIC sort)
target_link_libraries(sorter_lib PUBLIC Threads::Threads)

add_executable(run_test tests/test_sorters.cpp)
target_link_libraries(run_test PRIVATE sorter_lib)

add_executable(benchmark benchmarks/benchmark.cpp)
target_link_libraries(benchmark PRIVATE sorter_lib)

enable_testing()
add_test(NAME UnitTests COMMAND run_test)
