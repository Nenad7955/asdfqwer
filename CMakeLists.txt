cmake_minimum_required(VERSION 2.6)

# project(fatdriver)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(main main.cpp CustomFileSystem.cpp)

target_link_libraries(main ${GTEST_LIBRARIES} pthread)
# enable_testing()

# add_test(NAME Test1 COMMAND "./main")
