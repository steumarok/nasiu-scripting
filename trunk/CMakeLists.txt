cmake_minimum_required (VERSION 2.6)
project (nasiu)

find_package(Boost REQUIRED)

install(DIRECTORY include/nasiu DESTINATION include)

add_subdirectory(test/nasiu/scripting)

enable_testing()

	