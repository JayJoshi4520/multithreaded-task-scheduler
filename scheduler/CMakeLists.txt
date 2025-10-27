cmake_minimum_required(VERSION 3.14)
project(multithreaded_scheduler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(scheduler
  main.cpp
  Utils.cpp
)

# Link sqlite3 and pthread
find_package(Threads REQUIRED)

# Try system sqlite3 first
find_path(SQLITE3_INCLUDE_DIR sqlite3.h)
find_library(SQLITE3_LIBRARY sqlite3)

if (SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
  message(STATUS "Using system sqlite3")
  target_include_directories(scheduler PRIVATE ${SQLITE3_INCLUDE_DIR})
  target_link_libraries(scheduler PRIVATE ${SQLITE3_LIBRARY} Threads::Threads)
else()
  message(FATAL_ERROR "sqlite3 dev headers/library not found. Install libsqlite3-dev (Debian/Ubuntu) or sqlite-devel (RHEL/Fedora).")
endif()
