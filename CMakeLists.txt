cmake_minimum_required(VERSION 3.23)

project(WinterDevBake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

file(GLOB SOURCES "src/*")
add_executable(WinterDevBake ${SOURCES})

if (WIN32)
    target_compile_options(WinterDevBake PRIVATE /W4 /WX)
elseif (APPLE OR UNIX)
    target_compile_options(WinterDevBake PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Find the MariadbCpp library
if(WIN32)
    find_library(MARIADBCPP_LIBRARIES NAMES mariadbcpp PATHS "C:/Program Files/MariaDB/MariaDB C++ Connector 64-bit")
    set(MARIADBCPP_DLL "C:/Program Files/MariaDB/MariaDB C++ Connector 64-bit/mariadbcpp.dll")
elseif(APPLE)
    find_library(MARIADBCPP_LIBRARIES NAMES mariadbcpp)
    set(MARIADBCPP_DLL "/usr/local/lib/libmariadbcpp.dylib")
else()
    find_library(MARIADBCPP_LIBRARIES NAMES mariadbcpp)
    set(MARIADBCPP_DLL "/usr/lib/x86_64-linux-gnu/libmariadbcpp.so")
endif()

# Add the MariadbCpp include directories
if(WIN32)
    target_include_directories(WinterDevBake PRIVATE "C:/Program Files/MariaDB/MariaDB C++ Connector 64-bit/include")
elseif(APPLE)
    target_include_directories(WinterDevBake PRIVATE "/usr/local/include/mariadb")
else()
    target_include_directories(WinterDevBake PRIVATE "/usr/include/mariadb")
endif()

# Link against the MariadbCpp library
target_link_libraries(WinterDevBake ${MARIADBCPP_LIBRARIES})

# Copy the mariadb.dll library to the executable folder (Windows only)
if(WIN32)
    add_custom_command(TARGET WinterDevBake POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MARIADBCPP_DLL} $<TARGET_FILE_DIR:WinterDevBake>)
endif()