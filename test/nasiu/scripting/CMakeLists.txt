find_package(Threads REQUIRED)

include_directories(
    v8w 
    ${nasiu_SOURCE_DIR}/include
    ${nasiu_SOURCE_DIR}/test
    ${nasiu_SOURCE_DIR}/thirdparty/gtest/include)

add_executable(
	v8w_test 
	v8w/test.cpp
	${nasiu_SOURCE_DIR}/thirdparty/gtest/src/gtest-all.cc
	${nasiu_SOURCE_DIR}/thirdparty/gtest/src/gtest_main.cc)

target_link_libraries(
    v8w_test 
    ${CMAKE_THREAD_LIBS_INIT} 
    v8_g)

message(${CMAKE_BINARY_DIR})
message(${nasiu_BINARY_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()

add_test(v8w_test v8w_test)  
	