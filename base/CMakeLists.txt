cmake_minimum_required(VERSION 3.0)

project(tinyWeb_base_BASE CXX)

set(CXX_FLAGS -g -O2 -Wall -Werror)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_CXX_COMPILER "g++")

add_subdirectory(./src)

link_directories(./lib)

add_executable(Mutex_test Mutex_test.cc)
target_link_libraries(Mutex_test tinyWeb_base pthread)

add_executable(Condition_test Condition_test.cc)
target_link_libraries(Condition_test tinyWeb_base pthread)

add_executable(CountDownLatch_test CountDownLatch_test.cc)
target_link_libraries(CountDownLatch_test tinyWeb_base pthread)

add_executable(Atomic_test Atomic_test.cc)
target_link_libraries(Atomic_test tinyWeb_base pthread)

add_executable(Thread_test Thread_test.cc)
target_link_libraries(Thread_test tinyWeb_base pthread)

add_executable(Thread_bench Thread_bench.cc)
target_link_libraries(Thread_bench tinyWeb_base pthread)

add_executable(Singleton_test Singleton_test.cc)
target_link_libraries(Singleton_test tinyWeb_base pthread)

add_executable(ThreadLocal_test ThreadLocal_test.cc)
target_link_libraries(ThreadLocal_test tinyWeb_base pthread)

add_executable(SingletonThreadLocal_test SingletonThreadLocal_test.cc)
target_link_libraries(SingletonThreadLocal_test tinyWeb_base pthread)

add_executable(ThreadLocalSingleton_test ThreadLocalSingleton_test.cc)
target_link_libraries(ThreadLocalSingleton_test tinyWeb_base pthread)

add_executable(BlockingQueue_test BlockingQueue_test.cc)
target_link_libraries(BlockingQueue_test tinyWeb_base pthread)

add_executable(BoundedBlockingQueue_test BoundedBlockingQueue_test.cc)
target_link_libraries(BoundedBlockingQueue_test tinyWeb_base pthread)

add_executable(Timestamp_test Timestamp_test.cc)
target_link_libraries(Timestamp_test tinyWeb_base pthread)

add_executable(Timestamp_bench Timestamp_bench.cc)
target_link_libraries(Timestamp_bench tinyWeb_base pthread)

add_executable(FileUtil_test FileUtil_test.cc)
target_link_libraries(FileUtil_test tinyWeb_base pthread)

add_executable(LogStream_test LogStream_test.cc)
target_link_libraries(LogStream_test tinyWeb_base boost_unit_test_framework)
add_test(NAME LogStream_test COMMAND LogStream_test)

add_executable(LogStream_bench LogStream_bench.cc)
target_link_libraries(LogStream_bench tinyWeb_base)

add_executable(Logging_test Logging_test.cc)
target_link_libraries(Logging_test tinyWeb_base pthread)

add_executable(LogFile_test LogFile_test.cc)
target_link_libraries(LogFile_test tinyWeb_base pthread)

add_executable(ThreadPool_test ThreadPool_test.cc)
target_link_libraries(ThreadPool_test tinyWeb_base pthread)

add_executable(AsyncLogging_test AsyncLogging_test.cc)
target_link_libraries(AsyncLogging_test tinyWeb_base pthread)