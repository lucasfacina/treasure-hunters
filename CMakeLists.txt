cmake_minimum_required(VERSION 3.30)
project(trabalho_tilemap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(nlohmann_json httplib)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/src/*.h")

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

# --- CONFIGURAÇÃO DO HTTPS ---
#    Isso é o mesmo que colocar #define CPPHTTPLIB_OPENSSL_SUPPORT no topo do .cpp
target_compile_definitions(${PROJECT_NAME} PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
# --- FIM DA CONFIGURAÇÃO DO HTTPS ---

target_include_directories(${PROJECT_NAME} PRIVATE
        ${nlohmann_json_SOURCE_DIR}/include
        ${httplib_SOURCE_DIR}
)

IF (WIN32)
    target_link_libraries(${PROJECT_NAME} mingw32 allegro_monolith-debug)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(${PROJECT_NAME}
            OpenSSL::SSL
            OpenSSL::Crypto
    )
else ()
    include_directories(/opt/homebrew/Cellar/allegro/5.2.10.1/include)
    link_directories(/opt/homebrew/Cellar/allegro/5.2.10.1/lib)

    file(GLOB LIBRARIES "/opt/homebrew/Cellar/allegro/5.2.10.1/lib/*.dylib")
    find_package(OpenSSL REQUIRED)

    target_link_libraries(
            ${PROJECT_NAME}
            ${LIBRARIES}
            OpenSSL::SSL
            OpenSSL::Crypto
    )
ENDIF ()
