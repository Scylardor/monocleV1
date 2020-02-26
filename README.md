Monocle
=======

**This project is not maintained anymore. It was a design experiment that grew too complex, and I preferred recycling the ideas by reworking it from scratch.**

**Still, I decided to open source it in case you would like to see how Premake can be used or some dark C++ metaprogramming template tricks.**


Monocle, or MOE (for MOnocle Engine) is a 3D rendering engine I develop on my spare time.

With it, I'd like to experiment with common features of modern game engines, such as:

- Multithreaded task scheduling
- Support for multiple rendering APIs such as OpenGL and DirectX
- An efficient, easy-to-use animation pipeline
- A full-fledged memory and profiling framework to measure performance of 3D applications using it
- A GUI editor to edit and create 3D worlds within the engine (using Qt ?!)
- and much more...


I work on this project mainly for educational and research purposes.
It has various inspirations, from technical articles (especially from [Molecule](https://blog.molecular-matters.com/) and [Bitsquid](http://bitsquid.blogspot.fr/) developers) to engines I have worked with (Unreal, idTech, Dunia).
That's why I make it open source.


Compatibility
=============

Currently Monocle is built with Windows and Linux platforms in mind.

Support of other operating systems isn't guaranteed.


Building
========

To build Monocle you need [Premake 5](https://github.com/premake/premake-core).

Then, clone the repository and run premake5 to generate the project files of your choice (e.g. "premake vs2015" for a Visual Studio solution).

They will be generated in a "Workspace" directory at the root level of the repository.

When compiling, the output of the build will be stored in a "Build" directory at the root level of the repository.

Source code is divided into modules (Core, Animation, Rendering...) in the Source folder.

To build unit tests :

premake5 vs2015 --file=MonocleUnitTests_premake5.lua



Workflow
========

Currently I work on two branches:

- master for "stable" releases
- develop for day-to-day work

This helps to keep a clean release workflow.

If want to make a pull request, please read CONTRIBUTING.md first, and branch from develop.

