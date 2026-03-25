cmake_minimum_required(VERSION 3.10)
project(CandyCrush)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

# Include directories for all source folders
include_directories(
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_IMAGE_INCLUDE_DIRS} 
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    src
)

link_directories(
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_IMAGE_LIBRARY_DIRS}
    ${SDL2_TTF_LIBRARY_DIRS}
    ${SDL2_MIXER_LIBRARY_DIRS}
)

# Collect all source files from organized folders
add_executable(CandyCrush 
    src/main.cpp 
    # Engine
    src/engine/Engine.cpp 
    src/engine/SceneController.cpp
    # ECS
    src/ecs/ECS.cpp
    # Systems
    src/systems/AnimationSystem.cpp 
    src/systems/AudioSystem.cpp
    src/systems/GravitySystem.cpp
    src/systems/InputSystem.cpp
    src/systems/MatchSystem.cpp
    src/systems/RenderSystem.cpp
    src/systems/ScoreSystem.cpp
    # Game logic
    src/game/Board.cpp
    src/game/GravityLogic.cpp
    src/game/MatchLogic.cpp
    # Controllers
    src/controllers/AudioController.cpp
    src/controllers/ResourceController.cpp 
    src/controllers/UIManager.cpp
)

target_link_libraries(CandyCrush 
    ${SDL2_LIBRARIES} 
    ${SDL2_IMAGE_LIBRARIES} 
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

if(SOFTWARE_ACC)
    target_compile_definitions(CandyCrush PRIVATE SOFTWARE_ACC)
endif()
