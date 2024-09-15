# BattleBeyz

BattleBeyz is a 3D game that uses OpenGL for rendering graphics. My goal is to create the first realistic Beyblade game that handles high-speed collisions with custom physics, and I wanted to learn from the process of creating a game from scratch.

Any collaboration or feedback is welcome!

## Features (So far!)
- Custom working physics and collision detection
- Game logic in a finite state machine
- 3D rendering with OpenGL
- Font rendering with FreeType
- Camera control, lighting and shading
- Textures and dynamic meshes
- UI Elements/Navigation


## Building with Visual Studio 2022

The Visual Studio solution and project files are generated using **CMake** (I use **CMake 3.30.2**, but any recent versions should be fine):

1. **Open a command prompt** and navigate to the desired location.

2. **Clone the repository** to your local machine:
   ```bash
   git clone https://github.com/TheRickyZhang/BattleBeyz
   ```

3. **Navigate into the `BattleBeyz` directory**:
   ```bash
   cd BattleBeyz
   ```

4. **Generate the Visual Studio solution** by running CMake:
   ```bash
   cmake -S . -B build
   ```
   - Ensure there are no errors and that the correct version of **Visual C++** is found.

5. **Open the solution** in **Visual Studio 2022**:
   - Either double-click `...BattleBeyz\build\BattleBeyz.sln` or open it manually by going to:
     ```
     File -> Open -> Project/Solution
     ```

6. **Select the build configuration** (e.g., **Debug**, **Release**, etc.) in Visual Studio.

7. **Build the solution** by pressing **Ctrl+Shift+B**.

8. **Run or debug the application**:
   - Press **F5** to debug.
   - Press **Ctrl+F5** to run without debugging.

#### Issues
- Ensure you have git and cmake installed and added to your PATH for the above commands.
- If you encounter issues with glew32s, make sure that read-only is turned off for the glew32.lib file and that it is in the x64 folder, not Win32.
- If you have an existing build folder, make sure to remove it with rd /S /Q build (command prompt) or Remove-Item -Recurse -Force build (powershell)

### Libraries

#### GLFW, GLEW, GLM, FreeType, Assimp, Imgui, stb_image
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
curl -LO https://github.com/assimp/assimp/archive/refs/tags/v5.2.5.zip
unzip v5.2.5.zip
curl -LO https://github.com/ocornut/imgui/archive/refs/tags/v1.90.8.zip
unzip v1.90.8.zip
curl -LO https://github.com/nothings/stb/archive/refs/tags/v0.8.zip
unzip v0.8.zip
```
