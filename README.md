# BattleBeyz

BattleBeyz is the first 3D Beyblade game with accurate spinning top physics.

I also wanted to learn from the process of creating a game from scratch, so everything is written in C++ (OpenGL / ImGui) with no physics libraries. Any collaboration or feedback is welcome!

## Features (So far!)

- 3D Camera Control and Rendering
- Realistic Beyblade Physics (- some bugs)
- Centralized Game Engine and States
- Profile and Input Manager
- Texture, Mesh, Lighting/Shading
- Rendering with OpenGL and ImGui
- Logging and Debug Mode
- Font rendering with FreeType

# Coming Soon
- Save states
- Mesh import
- Pre-match selection screen and reset

# Installation Instructions for BattleBeyz

Follow these steps to clone, build, and run BattleBeyz using Visual Studio 2022:

## 1. Clone the Repository
1. Open your terminal (Command Prompt, PowerShell, or your preferred terminal).
2. Navigate to the folder where you'd like to store the project.
3. Run the following command to clone the repository:
   ```bash
   git clone https://github.com/TheRickyZhang/BattleBeyz
   ```
## 2. Prepare the Build Environment
1. **Remove any existing build folder**:
   - For PowerShell:
     ```powershell
     Remove-Item -Recurse -Force build
     ```
   - For Linux:
     ```bash
     rm -rf build
     ```
2. **Run CMake** to generate the Visual Studio solution and project files:
   ```bash
   cmake -S . -B build
   ```
   - **Note**: Carefully read the output to ensure that CMake runs without errors and detects the correct version of Visual C++.
## 3. Build the Project in Visual Studio
1. Open Visual Studio 2022.
2. Load the solution file generated by CMake:
   - From the menu, select **File -> Open -> Project/Solution**.
   - Navigate to the `build` folder (e.g., `D:\BattleBeyz\build`) and select `BattleBeyz.sln`.
3. Choose the desired build configuration:
   - **Debug** for development and debugging.
   - **Release** for optimized performance.
4. Build the solution:
   - From the menu, go to **Build -> Build Solution** (or press `Ctrl+Shift+B`).

## 4. Run the Application
- To debug the application:
  - Press `F5` or go to **Debug -> Start Debugging**.
- To run the application without debugging:
  - Press `Ctrl+F5` or go to **Debug -> Start Without Debugging**.

---

**Note**: Ensure that your system has the required dependencies, such as CMake (version 3.30.2 or later) and the correct Visual Studio C++ environment.

### Libraries

#### GLFW, GLEW, GLM, FreeType, Imgui, stb, tinyobjoader
These libraries are already built and included in the `lib` folder. If you want to build them yourself, you can follow
the links below for the corresponding versions.
```sh
cd lib
curl -LO https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
unzip glfw-3.4.zip
curl -LO https://sourceforge.net/projects/glew/files/glew/2.2.0/glew-2.2.0.zip
unzip glew-2.2.0.zip
curl -LO https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
unzip glm-0.9.9.8.zip
curl -LO https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.gz
tar -xzf freetype-2.13.2.tar.gz
curl -LO https://github.com/ocornut/imgui/archive/refs/tags/v1.90.8.zip
unzip v1.90.8.zip
curl -LO https://github.com/nothings/stb/archive/refs/tags/v0.8.zip
unzip v0.8.zip
curl -O https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/release/tiny_obj_loader.h
```
