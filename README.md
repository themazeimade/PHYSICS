** IMPORTANT RUN THE COMPILE.BAT TO COMPILE SHADERS** GLSL required (bundled with Vulkan SDK)
# PHYSICS
2D spehere physics simulation. When you run it, click and move the cursor in the direction you want the 2D sphere to go, to let it go press space. After 3 second if the sphere is static it will dissappear. These simulation has wind, gravity, drag, friction with context and with other spheres. Finally if you want more juice on your spheres check (or uncheck otherwise) to disable drag and the spheres will move faster. Finally, don't click outside the box sometimes it breaks everything :( . 
## Compile it
Only tested on windows but 100% sure it works on linux (Vulkan FTW)
if you are crazy like me use the MinGW64 makefiles, otherwise make the visual studio build files.
If the project uses vulkan, make sure you have installed the SDK and set as an enviroment variable in your OS system/session.
Also the project needs the GLFW include files and binaries, you need to compile them. Down below are the instructions. Finally, run the compile.bat on the main directory to compile the shaders needed to run the project.
<pre>
// GLFW compilation
  cd dependencies/glfw
  mkdir build
  cd build
  //either
  cmake -G "MinGW Makefiles" .. -DCMAKE_BUILD_TYPE=Debug // or Release
  // or
  cmake -G "Visual Studio 16" -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF ..
  //finally
  cmake --build --config Debug // or Release
  cd ../../../
// Main executable
  mkdir build
  cd build
  //either
  cmake -G "MinGW Makefiles" .. -DCMAKE_BUILD_TYPE=Debug // or Release
  //or
  cmake -G "Visual Studio 16" ..
  //finally
  cmake --build . --config Debug // or Release
</pre>
## Known issues
I think there is a memory allocation problem because sometimes the z component of the simulation differs from 0.0f. But i think it has very low probabilities to happen. Warnedd.
