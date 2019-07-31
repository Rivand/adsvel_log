cmake_minimum_required(VERSION 3.7)

project(adsvel_log)

set(CMAKE_CXX_STANDARD 17)

add_executable (${PROJECT_NAME}
    adsvel_log/adsvel_log.h
    adsvel_log/adsvel_log.cpp
    main.cpp
    )

target_link_libraries(${PROJECT_NAME} stdc++fs)
