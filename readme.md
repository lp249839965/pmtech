# Welcome to pmtech!

This work is still very much a work in progress, The win32 platform with dx11 is mostly complete, I have been using this for a game project. I am currently porting to OSX and iOS, both platforms are up and running and the opengl renderer (gles3 and gl3) is now in progress.. more platforms and renderers like metal, vulkan, linux or android will hopefully be added in future.

**PEN** *pmtech engine* 

This project contains platform / operating system specific code, it provides abstractions for:
- Renderer (dx11, opengl)
- Window / Entry (win32, ox, ios)
- Input / OS Message Pump (win32)
- Audio (fmod)
- Memory (win32, posix)
- String (for portability with windows unicode)
- File System (win32, dirent / stdlib)
- Threads / Syncronisation Primitives (win32, posix)
- Timers (win32, posix)
- Job Manager (todo)

The renderer runs on its own thread, with all user submitted commands from the game thread being stored in a command buffer for dispatch laster on a dedicated thread or core, this paralellises all graphics api driver overhead, the audio api and physics api (see put) also run the same way. I still need to work on giving greater control to which cores these tasks get run on.

**PUT** *pmtech utility toolkit*

This project contains code that will be re-used across different projects but contains no platform specific code, it contains currently:
- "Loader" - Contains functions to load textures (DDS), models, skeletons, animations and shaders
- Bullet physics with a deferred command buffer API like the PEN renderer and audio, so that physics simulate can run independantly on it's own thread.
- Debug renderer helpers, for drawing lines, boxes and other debug primitives
- Simple font renderer (stb font), for debug use
- Scalar (float) Maths library - Vector, Matrix, Quaternion, Intersection tests and helper functions
- ImGui integration (todo)

**Tools**

Tool scripts written in python and using some c++ executables to build data:
- Collada to binary - Models, skeletons and animations
- Textures - Compression and conversion using NVTT (Nvidia)
- Premake5 - All projects are configured using premake and are IDE agnostic
- ios project genetion - Simple script to copy ios files and fixup xcode to support ios projects
- Shader Compiler - FXC offline shader compilation
- Generic Shader Language (todo)

**examples**

This solution / workspace contains multiple examples of how to use the API's and set up projects, I am currently using them to aid porting, starting with a simple windowed application using minimal dependencies, I will add more examples as I need to implement functionality. Currently these examples are available:

- empty_project - first port of call to get a platform compiling, it creates and empty window with no rendering contexy
- basic_traingle - introduces a graphics context, clear sceen, shader loading / binding vertex buffer and non-indexed rendering
- textures - introduces texture loading using the "put" library, index buffers, indexed draw calls and texture samplers and texture binding.
- render_target - introduces render target creation and binding, also shader program loading using the "put" library

*Getting started*

To start a new project make sure it is located in pmtech/<project_dir>/ this is important because all paths to tools and other libraries are relative to this location. The examples solution is set up illustrating this layout.. all paths are relative to avoid having to deal with setting environment variables to locate various parts of the project.

An application just needs to define and initialise the pen::window_creation_params pen_window struct, and will be given a main loop, which can be defined in a separate project, there are helper functions to easily create in pen/premake_app.lua and the pen_examples/premake5.lua has examples of how to set up and link pen and put. 

Use the make projects batch and exec files to generate IDE solutions or workspaces, edit these files to change configuration settings.
