cmake_minimum_required(VERSION 3.23)
project(3p6mok C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-lwsock32")

add_executable(3p6mok main.c)

add_executable(server server.c)
target_link_libraries(server wsock32 ws2_32)

add_executable(client client.c)
target_link_libraries(client wsock32 ws2_32)


if(${CMAKE_SYSTEM_NAME} MATCHES Windows)
    file(WRITE ${PROJECT_BINARY_DIR}\\crun.bat start\ /wait\ ccore.bat)
    file(WRITE ${PROJECT_BINARY_DIR}\\ccore.bat @echo\ off\ntitle\ ${CMAKE_PROJECT_NAME}\ncls\n${CMAKE_PROJECT_NAME}\npause\nexit)
endif()