cmake_minimum_required(VERSION 3.22)

project(Horizons VERSION 1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    raylib_cpp
    GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
    GIT_TAG v5.5.0
)
FetchContent_MakeAvailable(raylib_cpp)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.15
)
FetchContent_MakeAvailable(pugixml)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.15.0
)
FetchContent_MakeAvailable(EnTT)



set(PROJECT_EXECUTABLE_NAME "game")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# Recursively find all .cpp files in the src directory
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable or library, using the source files found
add_executable(${PROJECT_EXECUTABLE_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add include directories (e.g., for headers)
target_include_directories(${PROJECT_EXECUTABLE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add src as an include dir to support tests
include_directories(${CMAKE_SOURCE_DIR}/src)

if (MSVC)
    #Needed for supporting the logger macros on MSVC
    target_compile_options(${PROJECT_EXECUTABLE_NAME} PRIVATE /Zc:preprocessor)
else()
    target_compile_options(${PROJECT_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

target_link_libraries(
        ${PROJECT_EXECUTABLE_NAME} PUBLIC
        raylib
        raylib_cpp
        nlohmann_json::nlohmann_json
        EnTT::EnTT
        pugixml::pugixml
)
