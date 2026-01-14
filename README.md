# TFG-VictorGonzalez

## Table of contents
- [Abstract](#abstract)
- [Gallery](#gallery)
- [How to install](#how-to-install)

## Abstract

This thesis focuses on optimization methods for the visualization of 3D point clouds in real time. These clouds have a lot of points, usually in the order of millions, this makes it so their visualization becomes computationally very costly and hard to do in real time, and so optimization techniques have to be introduced. In this context, this thesis focuses on two methods: the use of compute shaders and the use of the Lebesgue curve for the ordering of the points of the point cloud. In the first part of this thesis it formalizes and studies concepts such as the concept of a point cloud or the Lebesgue curve. It also explores other methods, like the use of traditional graphic pipeline or the elimination of Z-Test, and other orders, like randomly ordering the points or shuffling the points of the Lebesgue curve. The second part of this thesis consists in the design and implementation of a program that incorporates all the studied methods, so that one is able to visualize these point clouds in real time. It also includes a comparison of all the implemented methods and orders, with the objective of finding the best combination of rendering method and ordering of the points depending on the situation.

## Gallery
The following image illustrates the point clouds used and the performance of the methods implemented.

![alt text](https://github.com/VictorGonzalez02/TFG-VictorGonzalez/raw/main/images/GraphPointClouds.png)

## How to install

### Ubuntu

You will need the following libraries installed: GLEW, GLFW3 and GLM. To install them, you can use the following commands:

`sudo apt-get install libglew-dev`

`sudo apt-get install libglfw3-dev`

`sudo apt-get install libglm-dev`

Now, download the repository or clone it into Visual Studio Code and compile it using GCC. If you build it on VS Code, the first time you execute the program it will execute inside of the `build` folder. The program won't work, and you will have to go to the terminal and do the following command : `cd ../` and execute the program again.

### Windows

To access the Windows version of this program, navigate to the `Windows` branch of this repository. You will find a folder named `Built Project`. You can download the folder and execute the .exe file directly. In case you want to build this project by yourself, you will have to use Visual Studio Code with `vcpkg`, and have the same libraries installed as in the Ubuntu version. Then, to configure and build the project, use the following commands:

`mkdir build`

`cd build/`

`cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake` Substitute the path for your own path to `vcpkg`.

`cmake --build . --config Release`

You should now have a folder named `Release` inside of the `build` folder with the compiled executable file. To use the program, first copy the ``resources`` folder inside of the `Release` folder so the .exe file can read the shaders.

