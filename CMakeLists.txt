cmake_minimum_required(VERSION 3.14)

# Allow older CMake syntax in fetched dependencies
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum CMake policy version")

project(BattleBeyz)

include(FetchContent)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build Type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type: Debug or Release" FORCE)
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
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(stb)

# tinyobjloader (header-only)
FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG release
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tinyobjloader)

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
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)

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
# Project Source Files
# ============================================================================

# Include directories for project source
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/Camera)
include_directories(${PROJECT_SOURCE_DIR}/src/Camera/CameraBehaviors)
include_directories(${PROJECT_SOURCE_DIR}/src/GameObjects)
include_directories(${PROJECT_SOURCE_DIR}/src/MeshObjects)
include_directories(${PROJECT_SOURCE_DIR}/src/mini)
include_directories(${PROJECT_SOURCE_DIR}/src/Physics)
include_directories(${PROJECT_SOURCE_DIR}/src/Physics/Units)
include_directories(${PROJECT_SOURCE_DIR}/src/Rendering)
include_directories(${PROJECT_SOURCE_DIR}/src/RigidBodies)
include_directories(${PROJECT_SOURCE_DIR}/src/States)
include_directories(${PROJECT_SOURCE_DIR}/src/States/Menu)
include_directories(${PROJECT_SOURCE_DIR}/src/States/Customize)
include_directories(${PROJECT_SOURCE_DIR}/src/States/Gameplay)
include_directories(${PROJECT_SOURCE_DIR}/src/States/Intermediate)
include_directories(${PROJECT_SOURCE_DIR}/src/Config)
include_directories(${PROJECT_SOURCE_DIR}/src/UI)

# Add Source and Header Files
file(GLOB_RECURSE HEADER_FILES "src/*.h" "assets/*.h")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "assets/*.cpp")
set(SOURCES ${SOURCE_FILES} ${HEADER_FILES})

# ============================================================================
# Main Executable
# ============================================================================

add_executable(BattleBeyz ${SOURCES})

# Include directories for fetched header-only libraries
target_include_directories(BattleBeyz PRIVATE
    ${stb_SOURCE_DIR}
    ${tinyobjloader_SOURCE_DIR}
)

# Link libraries
target_link_libraries(BattleBeyz PRIVATE
    imgui_lib
    imguifiledialog_lib
    nlohmann_json::nlohmann_json
    glfw
    OpenGL::GL
    libglew_static
    Freetype::Freetype
    ZLIB::ZLIB
    glm::glm
)

# Preprocessor Definitions
target_compile_definitions(BattleBeyz PRIVATE
    _USE_MATH_DEFINES
    GLM_ENABLE_EXPERIMENTAL
    GLEW_STATIC
)

# Add Assets to Build Directory
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/assets)

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
