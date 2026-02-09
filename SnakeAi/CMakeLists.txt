cmake_minimum_required(VERSION 3.10)
project(SnakeAi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

include(FetchContent)

FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        2.6.0
)

FetchContent_MakeAvailable(SFML)

add_executable(SnakeAi SnakeAi.cpp StartScene.cpp)
target_link_libraries(SnakeAi PRIVATE sfml-system sfml-window sfml-graphics)

# Post-build: Copy SFML DLLs to the output directory (Windows/MSVC/Ninja)
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

install(TARGETS SnakeAi DESTINATION bin)
install(FILES SnakeAi.h DESTINATION include)
