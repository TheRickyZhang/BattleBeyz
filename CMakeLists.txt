cmake_minimum_required(VERSION 3.14)

# Allow older CMake syntax in fetched dependencies
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum CMake policy version")

project(BattleBeyz)

include(FetchContent)

# Lets clangd and other tooling resolve the project's many include directories
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build Type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the build type: Debug, RelWithDebInfo, or Release" FORCE)
endif()

# ============================================================================
# Fetch External Dependencies
# ============================================================================

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

# GLM (header-only math library)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.8
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

# ImGuiFileDialog (just fetch, don't use its CMakeLists.txt)
FetchContent_Declare(
    imguifiledialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG v0.6.7
    GIT_SHALLOW TRUE
)
FetchContent_Populate(imguifiledialog)

# nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

# stb (header-only)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    # stb publishes no releases, so pin the commit; "master" moves under us
    GIT_TAG 2c980bb59875b0d32144a71867fbdebb2f77cd20
)
FetchContent_MakeAvailable(stb)

# tinyobjloader (header-only)
FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    # Last release-branch commit before the embedded fast_float copy, whose
    # constexpr helpers call a non-constexpr distance() and fail MSVC with C3615
    GIT_TAG 71a40e4975939aed3f5237313b7ff55205631e84
)
FetchContent_MakeAvailable(tinyobjloader)

# Freetype - fetched so no platform needs a system package.
# These flags stop it searching for optional dependencies it will not find here.
set(FT_DISABLE_ZLIB     ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-13-3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(freetype)

# glew-cmake unconditionally links the AGL framework on Apple, which modern macOS
# SDKs no longer ship, so the link dies with "ld: framework 'AGL' not found".
# Seeding the cache entry makes its find_library skip the search and append nothing.
# (Upgrading is not an option: glew-cmake 2.3.x marks that find_library REQUIRED.)
if(APPLE)
    set(AGL_LIBRARY "" CACHE FILEPATH "Disabled: AGL is gone from the macOS SDK" FORCE)
endif()

# GLEW - Build from source (using Perlmint/glew-cmake which has pre-generated sources)
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0
    GIT_SHALLOW TRUE
)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glew)

# ============================================================================
# System Dependencies
# ============================================================================

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# ============================================================================
# ImGui Library (compiled from fetched sources)
# ============================================================================

add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

# ============================================================================
# ImGuiFileDialog Library (compiled from fetched sources)
# ============================================================================

add_library(imguifiledialog_lib STATIC
    ${imguifiledialog_SOURCE_DIR}/ImGuiFileDialog.cpp
)
target_include_directories(imguifiledialog_lib PUBLIC
    ${imguifiledialog_SOURCE_DIR}
)
target_link_libraries(imguifiledialog_lib PUBLIC imgui_lib)

# ============================================================================
# doctest (unit test framework)
# ============================================================================

option(BATTLEBEYZ_BUILD_TESTS "Build the unit tests" ON)

if(BATTLEBEYZ_BUILD_TESTS)
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(doctest)
endif()

# ============================================================================
# Project Source Files
# ============================================================================

# Warnings are on by default. -Wno-unused-parameter is deliberate: the GameState
# interface hands every override a width/height/deltaTime it may not need.
if(MSVC)
    set(BATTLEBEYZ_WARNINGS /W4 /wd4100)
else()
    set(BATTLEBEYZ_WARNINGS -Wall -Wextra -Wformat-security -Wno-unused-parameter)
endif()

file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.tpp" "assets/*.h")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "assets/*.cpp")

# main() lives outside the library so the test binary can supply its own
list(REMOVE_ITEM SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/main.cpp")
set(SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${PROJECT_SOURCE_DIR}/src/main.cpp)

# ============================================================================
# Core Library (everything but main, so both the game and the tests link it)
# ============================================================================

add_library(battlebeyz_core STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(battlebeyz_core PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/Camera
    ${PROJECT_SOURCE_DIR}/src/Camera/CameraBehaviors
    ${PROJECT_SOURCE_DIR}/src/GameObjects
    ${PROJECT_SOURCE_DIR}/src/MeshObjects
    ${PROJECT_SOURCE_DIR}/src/Physics
    ${PROJECT_SOURCE_DIR}/src/Physics/Units
    ${PROJECT_SOURCE_DIR}/src/Rendering
    ${PROJECT_SOURCE_DIR}/src/RigidBodies
    ${PROJECT_SOURCE_DIR}/src/States
    ${PROJECT_SOURCE_DIR}/src/States/Menu
    ${PROJECT_SOURCE_DIR}/src/States/Customize
    ${PROJECT_SOURCE_DIR}/src/States/Gameplay
    ${PROJECT_SOURCE_DIR}/src/States/Intermediate
    ${PROJECT_SOURCE_DIR}/src/Config
    ${PROJECT_SOURCE_DIR}/src/UI
    ${stb_SOURCE_DIR}
    ${tinyobjloader_SOURCE_DIR}
)

target_link_libraries(battlebeyz_core PUBLIC
    imgui_lib
    imguifiledialog_lib
    nlohmann_json::nlohmann_json
    glfw
    OpenGL::GL
    libglew_static
    freetype
    glm::glm
    Threads::Threads
)

target_compile_definitions(battlebeyz_core PUBLIC
    _USE_MATH_DEFINES
    GLM_ENABLE_EXPERIMENTAL
    GLEW_STATIC
    # Stop GLFW pulling in gl.h, which makes glew's include order matter everywhere
    GLFW_INCLUDE_NONE
)

target_compile_options(battlebeyz_core PRIVATE ${BATTLEBEYZ_WARNINGS})

# ============================================================================
# Main Executable
# ============================================================================

add_executable(BattleBeyz ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(BattleBeyz PRIVATE battlebeyz_core)
target_compile_options(BattleBeyz PRIVATE ${BATTLEBEYZ_WARNINGS})

# ThreadSanitizer (opt-in): cmake -B build-tsan -DBATTLEBEYZ_SANITIZE_THREAD=ON
option(BATTLEBEYZ_SANITIZE_THREAD "Build BattleBeyz with ThreadSanitizer" OFF)
if(BATTLEBEYZ_SANITIZE_THREAD)
    target_compile_options(battlebeyz_core PUBLIC -fsanitize=thread -g -O1 -fno-omit-frame-pointer)
    target_link_options(battlebeyz_core PUBLIC -fsanitize=thread)
endif()

# ============================================================================
# Tests
# ============================================================================

if(BATTLEBEYZ_BUILD_TESTS)
    enable_testing()

    file(GLOB TEST_SOURCES "tests/*.cpp")
    add_executable(battlebeyz_tests ${TEST_SOURCES})
    target_link_libraries(battlebeyz_tests PRIVATE battlebeyz_core doctest::doctest)
    target_compile_options(battlebeyz_tests PRIVATE ${BATTLEBEYZ_WARNINGS})

    # Runs from the build directory, where the assets copy below lands, so the
    # mesh tests can open real .obj files by relative path
    add_test(NAME battlebeyz_tests COMMAND battlebeyz_tests
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# Add Assets to Build Directory
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Debugging Settings (Visual Studio specific)
set_property(TARGET BattleBeyz PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BattleBeyz)

# Organize Files for Visual Studio
foreach(_source IN ITEMS ${SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${PROJECT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

# Status Messages
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "ImGui source: ${imgui_SOURCE_DIR}")
message(STATUS "ImGuiFileDialog source: ${imguifiledialog_SOURCE_DIR}")
message(STATUS "stb source: ${stb_SOURCE_DIR}")
message(STATUS "tinyobjloader source: ${tinyobjloader_SOURCE_DIR}")
