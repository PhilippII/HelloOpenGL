## tl;dr
- Use cmake to build this project.
- On Windows you need to specify **-A x64** to cmake when configuring.
- On Linux you first need to install glfw and glew on your system.
- Apple is not supported unfortunately.
- This project is an exercise to myself and it should also become a portfolio, that I can show to potential employers.
- this project is initially based on cherno's OpenGL-tutorial series, but it also contains some additional stuff that was not covered in cherno's tutorial series. (e.g. Framebuffers, loading .obj-files from disk manually without a helper library, ...)

# HelloOpenGL

This project serves two main purposes. The first is as an exercise for myself to get more familiar with 
- c++
- the c++-build process, how libraries work in c++ and how to use CMake (that is how to write my own CMakeLists.txt-files to configure my project.)
- the OpenGL-API

Unfortunately I feel like those more practical topics were a little neglected at my university as it was very often more focused on theory. Also while we did learn java and even a few more exotic programming languages like haskell, we never officially learned c++. While we did sometimes do a few exercises where we had to use c++, c++ itself was never the main focus in those exercises. Nor was c++ explained in the lecture it was just assumed we should be able to program c++ either from private studies or maybe by using it basically like java.
While this kind of works I later found there is much more to learn about c++ in order to write good c++-code. Like managing memory and other resources through a helper class that acts as a handle according to the RAII-principle. Also one needs to know about L-values and R-values, templates, important stuff from the standard library. What can be put in header files and what should remain in \*.cxx files to respect the one definition rule even if the header is included in multiple source files. (It seems simple at first: declarations in the header definitions in the \*.cxx files. That seems to be true for regular global variables and functions. But what about templates, inline functions, ...  And why can we put class definitions in the header-file not just the declarations.) There is a difference between unspecified behavior and undefined behavior? And how is undefined behavior different from a regular error? And so on.

The second purpose of this repository is that as it progresses I may use it as a portfolio that I can show to potential employers.

## Sources
This project is initially based on the OpenGL-Tutorial series by The Cherno:
https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
Cherno's series did provide a good starting point for my own project. Most importantly it showed how to write a few helper classes to abstract away the usage of the raw (sometimes a little ugly) OpenGL-functions. 
Cherno's series also showed how to set up a "Test"-framework. This framework allows to embed multiple different Tests (or Demos as I call them) into one main application. The main application will show a menu to select such a Demo. Each Demo is basically its own subapplication. Each Demo is implemented as its own subclass of an abstract class Demo. This test framework allows us to do different experiments and show different OpenGL concepts in different Demos. This allows us to keep the code a little more concise as each Demo need only demonstrate one concept at a time. Also the tedious initialization code for glfw, glew and imgui is not required inside the Demo as the main()-function will take care of that already before starting a Demo.
The project also contains stuff not (yet) covered in Cherno's OpenGL series. Most notably I implemented a function to load OBJ-files using only file-io functions from the c++-standard library. Also I implemented a way to move a camera in 3D and to create a camera matrix and a projection matrix for this camera (with the help of glm). I also added a simple lighting calculation using a single sun-light source and a Phong-reflection model. (I also want to implement a more physically based BSDF with proper energy conservation later.) I also experimented with framebuffer objects.


### Third party libraries used
- GLFW https://www.glfw.org/
- GLEW http://glew.sourceforge.net/
- Dear ImGui https://github.com/ocornut/imgui
- GLM https://github.com/g-truc/glm
- stb_image https://github.com/nothings/stb/blob/master/stb_image.h
- Guidelines Support Library (for gsl::span<...>) https://github.com/microsoft/gsl

### Non-Code Third-Party Resources:
The 3D Model of the bed is from 3dmodelhaven.com . I converted it into the OBJ-file format with the help of Blender.
The monkey head (suzanne) is of course the built-in test-model that is shipped with Blender. I exported it into the .obj format.

## How to Build this Project
On Linux you first need to install glfw and glew on your system (e.g. from the package manager).
Windows versions of the glfw and glew library are already included in the repository
(in 3rd_party/windows). Therefore you do not need to install any additional libraries on windows.
But the included windows libraries are 64-bit versions. Therefore on windows you need
to compile the application with 64-bit as well. 
To do that **pass the -A x64 flag to cmake if you are on windows** when configuring your build directory.

Otherwise you can configure and build the project as usual like any other cmake-project, for example a debug build on linux:
```
$ cd path/to/HelloOpenGL
$ mkdir build_Debug
$ cd build_Debug
$ cmake .. -DCMAKE_BUILD_TYPE=Debug
$ cmake --build .
$ ./OpenGLDemos
```

(You may speed up the build a little bit by using
`cmake --build . -- -j8`
or by using ninja instead of the default Makefile-generator.)


On windows with Visual Studio you do not need to specify the build type when configuring the build directory. CMake will create one Visual Studio project, which contains all build-types (including Debug *and* Release) instead of requiring you to create a separate build-directory for each build-type. But do not forget to add the **-A x64** flag as mentioned before. For example:
```
$ cd path/to/HelloOpenGL
$ mkdir build
$ cd build
$ cmake .. -A x64
```

Now you could also build and run the project from the command line as in the previous example on linux. But at this point it is probably more convenient to just open the generated \*.sln-file with Visual Studio. Then you can build and run the project from the GUI of Visual Studio.

For more info on how to use CMake, see also:
<https://cmake.org/cmake/help/latest/guide/user-interaction/index.html#guide:User%20Interaction%20Guide>
