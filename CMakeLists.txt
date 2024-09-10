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
file(GLOB_RECURSE SOURCES "src/*.cpp" ${IMGUI_SOURCES})

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

# Add assets directory to be copied to the build directory
#### file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Add executable
add_executable(BattleBeyz ${SOURCES}
        src/ShaderProgram.cpp
        src/ShaderProgram.h
        src/Buffers.cpp
        src/Buffers.h
        src/Initialize.cpp
        src/Initialize.h
        src/TextRenderer.cpp
        src/TextRenderer.h
        src/ShaderPath.h
        src/Utils.cpp
        src/Utils.h
        src/GameObject.h
        src/Stadium.cpp
        src/Stadium.h
        src/Texture.cpp
        src/Texture.h
        src/Camera.cpp
        src/Camera.h
        src/Callbacks.cpp
        src/Callbacks.h
        src/GameControl.h
        src/UI.cpp
        src/UI.h
        src/QuadRenderer.cpp
        src/QuadRenderer.h
        src/Beyblade.cpp
        src/Beyblade.h
        src/BoundingBox.cpp
        src/BoundingBox.h
        src/Physics.cpp
        src/Physics.h
        src/RigidBody.cpp
        src/RigidBody.h
        src/PhysicsWorld.cpp
        src/PhysicsWorld.h
 "src/RigidBodies/BeybladeParts.h" "src/RigidBodies/RandomDistribution.h" "src/RigidBodies/StadiumBody.h" "src/RigidBodies/BeybladeBody.cpp")

# Default runtime folder for debugging in Visual Studio:
set_property(TARGET BattleBeyz PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Set BattleyBeyz as the default startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BattleBeyz)

# Link libraries
target_link_libraries(BattleBeyz PRIVATE ${LIBS})
