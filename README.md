Diploma thesis project rendering realistic vegetation in realtime using C++/OpenGL/GLSL/VRUT.
by A. Kuchera

_CTU FEE. Department of Computer Graphics and Interaction_

The main goal of this project is to implement a robust application that can visualize and animate a tree-like object in real time as realisticaly as posible. The application is a module in a modular system called VRUT developed at CTU FEE.

The inspiration and the partial know-how comes from [Ralf Habel](http://www.cg.tuwien.ac.at/staff/RalfHabel.html), Alexander Kusternig, [Michael Wimmer](http://www.cg.tuwien.ac.at/staff/MichaelWimmer.html) and their works focused in that area: [Physically Based Real-Time Translucency for Leaves](http://www.cg.tuwien.ac.at/research/publications/2007/Habel_2007_RTT/) and [Physically Guided Animation of Trees](http://www.cg.tuwien.ac.at/research/publications/2009/Habel_09_PGT/)

## Actual state of the project

 * real-time framerate
 * fully animated 3D model - following the papers mentioned above
 * animated LOD version (pre-rendered slices -> 2D texture + animation)
 * instanced trees -> rendering more than 300 trees in real-time (using 2 levels of detail - full geometry, 3x3 slices )

## Videos

[Preview](https://www.youtube.com/embed/WraiCP3gT18)

[Part 1](https://www.youtube.com/embed/eqXCpCINTzQ)

[Part 2](https://www.youtube.com/embed/HWZUVE5hgNQ)

[Part 3](https://www.youtube.com/embed/muU_hzCpfHU)
