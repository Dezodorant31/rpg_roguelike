cmake_minimum_required(VERSION 3.28)
project(rpg_roguelike LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Отключаем сканирование C++-модулей — мы их не используем,
# а на заголовках SFML GCC спотыкается.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# ---------------------------------------------------------------------------
# SFML: сначала системная установка, иначе локальная в проекте
# ---------------------------------------------------------------------------
find_package(SFML 3.1 COMPONENTS Graphics Window System QUIET)

if(NOT SFML_FOUND)
    message(STATUS "Системный SFML не найден, ищу локальный в SFML-3.1.0/")
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML-3.1.0/lib/cmake/SFML"
        CACHE PATH "SFML cmake config dir" FORCE)
    find_package(SFML 3.1 COMPONENTS Graphics Window System REQUIRED)
endif()

message(STATUS "SFML найден: ${SFML_DIR}")

# ---------------------------------------------------------------------------
# Общие флаги (используются в нескольких таргетах)
# ---------------------------------------------------------------------------
function(apply_common_flags target)
    target_include_directories(${target} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/third_party/doctest
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined -fno-omit-frame-pointer -g)
        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined)
    endif()
endfunction()

# ---------------------------------------------------------------------------
# Основной исполняемый файл
# ---------------------------------------------------------------------------
add_executable(rpg_roguelike
    src/main.cpp
    src/world/Map.cpp
    # World.hpp — header-only, .cpp не нужен
    # Command.hpp — header-only после Спринта 1
)
target_link_libraries(rpg_roguelike PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
apply_common_flags(rpg_roguelike)

# ---------------------------------------------------------------------------
# Тесты (doctest)
# ---------------------------------------------------------------------------
enable_testing()

# test_map
add_executable(test_map
    tests/test_map.cpp
    src/world/Map.cpp
)
target_link_libraries(test_map PRIVATE SFML::System)
apply_common_flags(test_map)
add_test(NAME MapTests COMMAND test_map)

# test_world
add_executable(test_world
    tests/test_world.cpp
)
target_link_libraries(test_world PRIVATE SFML::System)
apply_common_flags(test_world)
add_test(NAME WorldTests COMMAND test_world)
