# Invoking the CMake build from the command line is a two step process, first, *generate* a build by running the following:
#
#   cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release
# 
# Where `Release` can be replaced with `Debug`, which contains gdb and address sanitiser definitions already written for you.
# Once a build is created, proceed with *compilation*:
# 
#   cmake --build build --target RedNoise --config Release # optionally, for parallel build, append -j $(nproc)
#
# This creates the executable in the build directory. You only need to *generate* a build if you modify the CMakeList.txt file.
# For any other changes to the source code, simply recompile.


cmake_minimum_required(VERSION 3.12)
project(ComputerGraphics)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Note, we do this for glm because it's a header only library and because we shipped it with the project
# normally you would use find_package(<package_name>) for libraries with actual objects
set(GLM_INCLUDE_DIRS libs/glm-0.9.7.2)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(libs/sdw)

add_executable(
    ComputerGraphics
        libs/sdw/CanvasPoint.cpp
        libs/sdw/CanvasTriangle.cpp
        libs/sdw/Colour.cpp
        libs/sdw/DrawingWindow.cpp
        libs/sdw/ModelTriangle.cpp
        libs/sdw/RayTriangleIntersection.cpp
        libs/sdw/TextureMap.cpp
        libs/sdw/TexturePoint.cpp
        libs/sdw/Utils.cpp
        src/util/util.cpp
        src/render/Draw.cpp
        src/render/RaytracedDraw.cpp
        src/render/RasterisedDraw.cpp
        src/scene/Camera.cpp
        src/scene/Scene.cpp
        src/scene/Object.cpp
        src/scene/Object3d.cpp
        src/scene/Camera.cpp
        src/scene/Light.cpp
        src/Program.cpp
        src/animation/Animate.cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/framesout)

if (MSVC)
    target_compile_options(ComputerGraphics
            PUBLIC
            /W3
            /Zc:wchar_t
            )
    set(DEBUG_OPTIONS /MTd)
    set(RELEASE_OPTIONS /MT /GF /Gy /O2 /fp:fast)
    if (NOT DEFINED SDL2_LIBRARIES)
        set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main)
    endif()
else ()
    target_compile_options(ComputerGraphics
        PUBLIC
        -Wall
        -Wextra
        -Wcast-align
        -Wfatal-errors
        -Werror=return-type
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-ignored-attributes)

    set(DEBUG_OPTIONS -O2 -fno-omit-frame-pointer -g)
    set(RELEASE_OPTIONS -O3 -march=native -mtune=native)
    target_link_libraries(ComputerGraphics PUBLIC $<$<CONFIG:Debug>:-Wl,-lasan>)

endif()


target_compile_options(ComputerGraphics PUBLIC "$<$<CONFIG:RelWithDebInfo>:${RELEASE_OPTIONS}>")
target_compile_options(ComputerGraphics PUBLIC "$<$<CONFIG:Release>:${RELEASE_OPTIONS}>")
target_compile_options(ComputerGraphics PUBLIC "$<$<CONFIG:Debug>:${DEBUG_OPTIONS}>")
 
target_link_libraries(ComputerGraphics PRIVATE ${SDL2_LIBRARIES})