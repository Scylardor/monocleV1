Monocle
=======

Monocle is a 3D rendering engine I develop on my spare time.

With it, I'd like to experiment with common features of modern game engines, such as:

- Multithreaded task scheduling
- Support for multiple rendering APIs such as OpenGL and DirectX
- An efficient, easy-to-use animation pipeline
- A full-fledged memory and profiling framework to measure performance of 3D applications using it
- A GUI editor to edit and create 3D worlds within the engine (using Qt ?!)
- and much more...


I work on this project mainly for educational and research purposes. That's why it is open source.




Building
========

To build Monocle you need [Premake 5](https://github.com/premake/premake-core).

Then, clone the repository and run premake5 to generate the project files of your choice (e.g. "premake vs2015" for a Visual Studio solution).

They will be generated in a "Workspace" directory at the root level of the repository.

When compiling, the output of the build will be stored in a "Build" directory at the root level of the repository.

Source code is divided into modules (Core, Animation, Rendering...) in the Source folder.




