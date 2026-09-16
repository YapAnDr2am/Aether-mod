cmake_minimum_required(VERSION 3.21)
project(Aether)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ЭТА СТРОКА ВАЖНА: она заставляет Geode собираться из исходников
set(GEODE_LINK_NIGHTLY ON)

include(FetchContent)
FetchContent_Declare(
    geode-sdk
    GIT_REPOSITORY https://github.com/geode-sdk/geode.git
    GIT_TAG main
)
FetchContent_MakeAvailable(geode-sdk)

add_library(Aether MODULE src/main.cpp)
target_link_libraries(Aether PRIVATE Geode::Geode)

geode_mod(Aether
    ID "com.yourname.aether"
    NAME "Aether"
    VERSION "1.0.0"
    DEVELOPER "YapAnDr2am"
    DESCRIPTION "Adaptive AI trainer for Geometry Dash."
)
