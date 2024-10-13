# BattleBeyz

BattleBeyz is a 3D game that uses OpenGL for rendering graphics. My goal is to create the first realistic Beyblade game that handles high-speed collisions with custom physics, and I wanted to learn from the process of creating a game from scratch.

Any collaboration or feedback is welcome!

## Features (So far!)

- 3D rendering with OpenGL
- Font rendering with FreeType
- Basic Camera control
- Lighting and Shading
- Basic Textures
- Dynamic Meshes
- UI Elements/Navigation
- Scrolling Backgrounds

## Custom Settings
You may set the following features via the Customize Beyblade screen:

- Masses, Moments of Inertia, Coefficient of Friction, Coefficient of Restitution, Recoil Distribution Mean and StdDev.
- Save the above settings into named "profiles", and reuse them on subsequent runs.  Profiles are stored in beyblade_profile.ini
  in the same folder as the program file.  (This location is likely to change in the future.)  The ini file is
  automatically recreated if missing or damaged.
- You may currently not change the blade meshes from this interface, although you can customize the mesh files with a 3D
  graphics editor such as Blender.
- There is no mechanism to delete a profile from the UI.
 

## Building with Visual Studio 2022
The Visual Studio solution and project files are generated with CMake (we use CMake 3.30.2):

- Open a command prompt, and change to the project folder, for example, cd D:\BattleBeyz-dev
- Remove any existing build folder.  Windows command prompt: rd /S /Q build, or in PowerShell: Remove-Item -Recurse -Force build
- Run CMake:  cmake -S . -B build
  Read the output carefully, and make sure cmake ran withut errors and that it found the correct version of Visual C++.
- Close the command prompt if you no longer need it for other activities.
- Run Visual Studio and open the solution file, for example, from File -> Open -> Project/Solution, select D:\BattelBeyz-dev\build\BattleBeyz.sln.
- Select the build variant (Debug, Release, etc.), and then run Build Solution from the Build menu.
- Debug or run the application from the Debug menu or using menu shortcut (F5 to debug, Ctrl+F5 to run without debugging).

## Installation
TODO

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

#### PhysX
This library is too large to upload with git, so you will need to generate it yourself.
First, download the PhysX SDK from the NVIDIA website.
```sh
curl -LO https://github.com/NVIDIA/PhysX/archive/refs/tags/5.4.0.zip
unzip 5.4.0.zip
```

Then, you will need to build the library. You can follow the instructions provided, or you can use the following commands. 
```sh
cd PhysX-5.4.0/physx
generate_projects.bat
```
Open PhysX.sln in compiler/vc16win64 (or compiler/vc17win64) with Visual Studio and build in either Debug or Release mode.
    Then, move the PhysX SDK files to the `lib` folder under /physx-5.4.0 - ensure you have at least the `include` and `lib` (bin) folders.
```
cp -r path/to/physx/include/* path/to/BattleBeyz/lib/physx-5.4.0/include
cp -r path/to/physx/lib/* path/to/BattleBeyz/lib/physx-5.4.0/lib
```

If you are compiling on x86, you might also face an issue with unrecognized types. You'll need to resolve this by adjusting the logic to use the correct types.