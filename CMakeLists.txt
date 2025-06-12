cmake_minimum_required(VERSION 3.30)
project(trabalho_tilemap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/src/*.h")

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

IF (WIN32)
    target_link_libraries(${PROJECT_NAME} mingw32 allegro_monolith-debug)
else ()
    include_directories(/opt/homebrew/Cellar/allegro/5.2.10.1/include)
    link_directories(/opt/homebrew/Cellar/allegro/5.2.10.1/lib)

    file(GLOB LIBRARIES "/opt/homebrew/Cellar/allegro/5.2.10.1/lib/*.dylib")

    target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
ENDIF ()
