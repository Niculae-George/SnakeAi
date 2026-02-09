cmake_minimum_required(VERSION 3.10)
project(SnakeAi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_HEADLESS "Build without GUI" OFF)

# Core sources (used by both)
set(CORE_SOURCES
    Core/AiAgent.cpp
    Core/SnakeGame.cpp
)

include_directories(Core)

if (BUILD_HEADLESS)
    add_definitions(-DHEADLESS_BUILD)
    
    # Disable SFML components we don't need
    set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

    find_package(SFML 2.6 COMPONENTS system)
    
    if (NOT SFML_FOUND)
        include(FetchContent)
        set(WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.0)
        FetchContent_MakeAvailable(SFML)
    endif()

    add_executable(SnakeAiHeadless SnakeAi.cpp ${CORE_SOURCES})
    if (TARGET SFML::System)
        target_link_libraries(SnakeAiHeadless PRIVATE SFML::System)
    else()
        target_link_libraries(SnakeAiHeadless PRIVATE sfml-system)
    endif()
    
    install(TARGETS SnakeAiHeadless DESTINATION bin)
else()
    # GUI build
    find_package(SFML 2.6 COMPONENTS system window graphics)

    if (NOT SFML_FOUND)
        include(FetchContent)
        set(WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.0)
        FetchContent_MakeAvailable(SFML)
    endif()

    set(GUI_SOURCES
        SnakeAi.cpp
        StartScene.cpp
        GameScene.cpp
    )

    add_executable(SnakeAi ${GUI_SOURCES} ${CORE_SOURCES})

    if (TARGET SFML::Graphics)
        target_link_libraries(SnakeAi PRIVATE SFML::System SFML::Window SFML::Graphics)
    else()
        target_link_libraries(SnakeAi PRIVATE sfml-system sfml-window sfml-graphics)
    endif()

    if (WIN32)
        add_custom_command(TARGET SnakeAi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-graphics-d-2.dll
                $<TARGET_FILE_DIR:SnakeAi>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-window-d-2.dll
                $<TARGET_FILE_DIR:SnakeAi>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-system-d-2.dll
                $<TARGET_FILE_DIR:SnakeAi>
        )
    endif()

    install(TARGETS SnakeAi DESTINATION bin)
endif()
