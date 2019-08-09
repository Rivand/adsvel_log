cmake_minimum_required(VERSION 3.12)

project(adsvel_log)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
find_package (Threads)  # Треды подключаем независимо от платформы.

find_package(fmt)

add_executable (${PROJECT_NAME}
    adsvel_log/adsvel_log.h
    adsvel_log/adsvel_log.cpp
    adsvel_log/sinks/file_sink.h
    adsvel_log/sinks/stdout_sink.h
    main.cpp
    )

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} fmt::fmt stdc++fs)
