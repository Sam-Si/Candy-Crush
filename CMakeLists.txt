cmake_minimum_required(VERSION 3.10)
project(CandyCrush)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

include_directories(
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_IMAGE_INCLUDE_DIRS} 
    ${SDL2_TTF_INCLUDE_DIRS}
)

link_directories(
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_IMAGE_LIBRARY_DIRS}
    ${SDL2_TTF_LIBRARY_DIRS}
)

add_executable(CandyCrush 
    main.cpp 
    Animation.cpp 
    AnimationController.cpp 
    Board.cpp
    ECS.cpp
    Engine.cpp 
    GravityLogic.cpp
    MatchLogic.cpp
    ResourceController.cpp 
    SceneController.cpp
    UIManager.cpp
)

target_link_libraries(CandyCrush 
    ${SDL2_LIBRARIES} 
    ${SDL2_IMAGE_LIBRARIES} 
    ${SDL2_TTF_LIBRARIES}
)

if(SOFTWARE_ACC)
    target_compile_definitions(CandyCrush PRIVATE SOFTWARE_ACC)
endif()
