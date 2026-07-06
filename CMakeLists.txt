cmake_minimum_required(VERSION 3.16)
project(GuariteiroDaMeiaNoite CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Encontra dependências ──────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3  CONFIG REQUIRED)
find_package(glm    CONFIG REQUIRED)
find_package(glad   CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# ── Fontes ────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(guariteiro ${SOURCES})

# ── Includes ──────────────────────────────────────────────────────
target_include_directories(guariteiro PRIVATE
    include
)

# ── Link ──────────────────────────────────────────────────────────
target_link_libraries(guariteiro PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    glad::glad
    assimp::assimp
)

if(WIN32)
    target_link_libraries(guariteiro PRIVATE opengl32)
else()
    target_link_libraries(guariteiro PRIVATE dl)
endif()

# ── DLLs das dependências ──────────────────────────────────────────
set(VCPKG_BIN "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows/bin")
set(VCPKG_BIN_DBG "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows/debug/bin")

# ── Copia shaders, assets e DLLs para o build ────────────────────
add_custom_command(TARGET guariteiro POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:guariteiro>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets  $<TARGET_FILE_DIR:guariteiro>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN}/glfw3.dll" $<TARGET_FILE_DIR:guariteiro>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN}/assimp-vc145-mt.dll" $<TARGET_FILE_DIR:guariteiro>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN}/poly2tri.dll" $<TARGET_FILE_DIR:guariteiro>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN}/kubazip.dll" $<TARGET_FILE_DIR:guariteiro>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN}/minizip.dll" $<TARGET_FILE_DIR:guariteiro>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN}/pugixml.dll" $<TARGET_FILE_DIR:guariteiro>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN}/z.dll" $<TARGET_FILE_DIR:guariteiro>
    COMMENT "Copiando shaders, assets e DLLs..."
)
