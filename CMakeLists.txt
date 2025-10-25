# ===================================================================
# 1. CONFIGURAÇÃO BÁSICA DO PROJETO

cmake_minimum_required(VERSION 3.30)
project(trabalho_tilemap)

# Define o padrão do C++ para C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# ===================================================================

# ===================================================================
# 2. GERENCIAMENTO DE DEPENDÊNCIAS (FetchContent)

# Habilita o FetchContent para baixar bibliotecas do GitHub
include(FetchContent)

# Declara a biblioteca nlohmann/json (para JSON)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)

# Declara a biblioteca cpp-httplib (para requisições HTTP)
FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.15.3
)

# Baixa e disponibiliza as bibliotecas
FetchContent_MakeAvailable(nlohmann_json httplib)
# ===================================================================

# ===================================================================
# 3. CONFIGURAÇÃO DE BIBLIOTECAS LOCAIS

# Adiciona o diretório /src aos caminhos de include
include_directories(${PROJECT_SOURCE_DIR}/src)

# Configura os caminhos para a biblioteca Allegro 5 no Windows
IF (WIN32)
    set(ALLEGRO_ROOT "C:/Allegro")
    include_directories("${ALLEGRO_ROOT}/include")
    link_directories("${ALLEGRO_ROOT}/lib")

ENDIF ()
# ===================================================================

# ===================================================================
# 4. DEFINIÇÃO DO EXECUTÁVEL

# Encontra todos os arquivos .cpp e .h dentro da pasta /src
file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/src/*.h")

# Cria o executável principal
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

# Define o nome do arquivo .exe final (de "trabalho_tilemap.exe" para "treasure-hunters.exe")
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "treasure-hunters"
)

# --- Comando Pós-Build: Copiar Assets ---
# Copia a pasta /assets para o diretório de build (ex: cmake-build-debug)
# Isso é necessário para rodar o executável de dentro do CLion

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copiando pasta 'assets' para o diretório de build..."
)

# --- Comando Pós-Build: Copiar config.json ---
# Copia o arquivo config.json para o diretório de build

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/config.json"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/config.json"
        COMMENT "Copiando config.json"
)

# ===================================================================


# ===================================================================
# 5. LINKAGEM DE BIBLIOTECAS E DEFINIÇÕES

# Define a macro CPPHTTPLIB_OPENSSL_SUPPORT para habilitar HTTPS no httplib
target_compile_definitions(${PROJECT_NAME} PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

# Adiciona os caminhos de include das bibliotecas baixadas (json e httplib)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${nlohmann_json_SOURCE_DIR}/include
        ${httplib_SOURCE_DIR}
)

# --- Linkagem Específica por Plataforma ---
IF (WIN32)
    # Encontra a biblioteca OpenSSL (instalada via MSYS2)
    set(OPENSSL_ROOT_DIR "C:/msys64/mingw64")
    find_package(OpenSSL REQUIRED)

    # Linka todas as bibliotecas necessárias para o Windows
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            mingw32 # Lib do MinGW

            # *** CORREÇÃO DE BUG CRÍTICO ***
            # Usa "generator expressions" para linkar a biblioteca correta:
            # -debug no modo Debug, -5.2 (release) no modo Release.
            $<$<CONFIG:Debug>:allegro_monolith-debug>
            $<$<CONFIG:Release>:allegro_monolith>

            # Bibliotecas OpenSSL
            OpenSSL::SSL
            OpenSSL::Crypto

            # Bibliotecas do Windows Sockets (para rede)
            ws2_32
            crypt32
    )

ELSE ()
    # Configuração para macOS (Homebrew)
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
# ===================================================================

# ===================================================================
# 6. CONFIGURAÇÃO DE PACOTE (CPack) - Para criar o .zip de distribuição


# 1. Instala o executável ("treasure-hunters.exe") na pasta "bin" do pacote
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
)

# 2. Instala a pasta de assets na pasta "bin" (ao lado do .exe)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/assets"
        DESTINATION bin
)

# 3. Instala todas as DLLs de dependência (apenas para Windows)
IF(WIN32)
    set(ALLEGRO_BIN_DIR "${ALLEGRO_ROOT}/bin")
    set(MINGW_BIN_DIR "C:/msys64/mingw64/bin")

    install(FILES
            # DLL do Allegro (Release)
            "${ALLEGRO_BIN_DIR}/allegro_monolith-5.2.dll"

            # DLLs do OpenSSL
            "${MINGW_BIN_DIR}/libssl-3-x64.dll"
            "${MINGW_BIN_DIR}/libcrypto-3-x64.dll"

            # DLLs do Runtime MinGW (essenciais para rodar em outras máquinas)
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"

            # Cofiguração de JSON (arquivo de exemplo)
            "${PROJECT_SOURCE_DIR}/config.json"

            DESTINATION bin # Copia todas para a pasta "bin"

    )
ENDIF()

# 4. Configura o CPack para gerar um arquivo .zip
set(CPACK_GENERATOR "ZIP")

# Define o nome do arquivo .zip final (ex: "treasure-hunters-1.0.0-win64.zip")
set(CPACK_PACKAGE_NAME "treasure-hunters")
set(CPACK_PACKAGE_VERSION "1.0.1")

# Inclui o CPack para habilitar o comando "cpack"
include(CPack)
# ===================================================================
