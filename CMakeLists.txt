cmake_minimum_required(VERSION 3.21)
project(Aether)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создаём наш мод как модуль
add_library(Aether MODULE src/main.cpp)

# Линкуем его с Geode. GitHub Actions сам подставит нужные пути.
target_link_libraries(Aether PRIVATE Geode::Geode)

# Добавляем метаданные мода
geode_mod(Aether
    ID "com.yourname.aether"
    NAME "Aether"
    VERSION "1.0.0"
    DEVELOPER "YapAnDr2am"
    DESCRIPTION "Adaptive AI trainer for Geometry Dash."
)
