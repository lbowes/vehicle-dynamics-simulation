cmake_minimum_required(VERSION 3.17)
# todo

add_executable(my-tests
    src/test_dummy.cpp
)

target_link_libraries(my-tests PRIVATE vehicle-dynamics-sim)