cmake_minimum_required(VERSION 3.10)
project(BattleBeyz)

# Toolchain and Prefix Path
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows-static" CACHE PATH "Path to vcpkg installed packages")

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build Type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type: Debug or Release" FORCE)
endif()

# Libraries (non-vcpkg)
include_directories(${PROJECT_SOURCE_DIR}/lib)      
include_directories(${PROJECT_SOURCE_DIR}/lib/headers)                 # For tinyobjloader, stb_image, json
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui)                  # Local ImGui
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui/backends)

# Libraries (vcpkg)
include_directories("C:/vcpkg/installed/x64-windows/include")
link_directories("C:/vcpkg/installed/x64-windows/lib")

# Find Packages
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(ZLIB REQUIRED)


# Include directories
include_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8)
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/backends)
# ImGui Sources
set(IMGUI_SOURCES
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui_tables.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui_widgets.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/backends/imgui_impl_opengl3.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/backends/imgui_impl_glfw.cpp
        ${PROJECT_SOURCE_DIR}/lib/ImGuiFileDialog/ImGuiFileDialog.cpp
)

# Include the src/ directory and all its subdirectories
include_directories(${PROJECT_SOURCE_DIR}/assets)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/Camera)
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
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "assets/*.cpp" ${IMGUI_SOURCES})
set(SOURCES ${SOURCE_FILES} ${HEADER_FILES})

# Libraries
set(LIBS glfw OpenGL::GL GLEW::GLEW freetype ZLIB::ZLIB)

# Add Assets to Build Directory
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/assets)

# Setup
add_executable(BattleBeyz ${SOURCES})
target_link_libraries(BattleBeyz PRIVATE ${LIBS})

# Debugging Settings (must come after the target is created)
set_property(TARGET BattleBeyz PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BattleBeyz)

# Preprocessor Definitions
add_compile_definitions(_USE_MATH_DEFINES)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# Organize Files for Visual Studio
foreach(_source IN ITEMS ${SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${PROJECT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

# Status Messages
message(STATUS "Using CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
# message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
# message(STATUS "Linked Libraries: ${LIBS}")
