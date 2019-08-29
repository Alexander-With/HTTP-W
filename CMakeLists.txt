project(http-w)
cmake_minimum_required(VERSION 3.15)


if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(SOURCES 
	http_mime.cpp
	http_response.cpp
	http_server.cpp
	http_ssl.cpp
)

link_libraries(crypto ssl)
add_executable(http-w http_main.cpp ${SOURCES})
add_executable(https-w https_main.cpp ${SOURCES})
