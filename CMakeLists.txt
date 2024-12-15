cmake_minimum_required(VERSION 3.10)
project(BattleBeyz)

# Use vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows" CACHE PATH "Path to vcpkg installed packages")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/glew-2.2.0/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/glfw-3.4/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/glm-1.0.1)
include_directories(${PROJECT_SOURCE_DIR}/lib/freetype-2.13.2/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/stb)
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8)
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/backends)

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
include_directories(${PROJECT_SOURCE_DIR}/src/Config)


# Specify ImGui source files
set(IMGUI_SOURCES
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui_tables.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/imgui_widgets.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/backends/imgui_impl_opengl3.cpp
        ${PROJECT_SOURCE_DIR}/lib/imgui-1.90.8/backends/imgui_impl_glfw.cpp
)

# Add the preprocessor definitions. Otherwise you have trouble with cmath and opengl
add_compile_definitions(_USE_MATH_DEFINES)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "assets/*.cpp" ${IMGUI_SOURCES})

# Add GLFW
set(GLFW_DIR ${PROJECT_SOURCE_DIR}/lib/glfw-3.4)
add_subdirectory(${GLFW_DIR})

# Add GLEW
set(GLEW_DIR ${PROJECT_SOURCE_DIR}/lib/glew-2.2.0)
link_directories(${GLEW_DIR}/lib/Release/x64)
add_definitions(-DGLEW_STATIC)

# Add FreeType
set(FREETYPE_DIR ${PROJECT_SOURCE_DIR}/lib/freetype-2.13.2)
link_directories(${FREETYPE_DIR}/static/vs2015-2022/win64)

# Add external libraries
set(LIBS glfw glew32s ${OPENGL_LIBRARIES} freetype glu32 opengl32)

# Add assets directory to be copied to the build directory.  We don't need this in the build
# output, but assets must be a subfolder of the working directory at runtime.  Later we will
# create an installer that will set up assets properly.
#### file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Add our own sources
# file(GLOB_RECURSE MAIN_SOURCES src/*.cpp src/*.h)
# Combine sources
# set(SOURCES ${MAIN_SOURCES} ${IMGUI_SOURCES})

# Create source groups to appear in our Visual Studio project.
foreach(_source IN ITEMS ${SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${PROJECT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

# Add executable or library targets
add_executable(BattleBeyz ${SOURCES})

# Default runtime folder for debugging in Visual Studio:
set_property(TARGET BattleBeyz PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Set Battlebeyz as the default startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BattleBeyz)

# Link libraries
target_link_libraries(BattleBeyz PRIVATE ${LIBS})