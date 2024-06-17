# Overview

This repository contains implementation of a ray tracer. The ray tracer support 3 types of objects: (i) planes, (ii) spheres, and (iii) cubes. The object material properties (i.e., diffuse, specular, and reflection coeffients) are customizable. Also, the number and positions of the light sources and the level of recursion can be given as input. The implementation of this rendered is integrated with OpenGL so that outputs can be checked for correctness simply by comparing the output image of the renderer with the OpenGL window.



# Input Explanation 

The "description.txt" file contains the scence description which works as input to both the ray tracer and the OpenGL code. The text file is formatted as follows:


```
level of recursion
number of pixels along both axes
number of objects

type of object (1 for sphere)
center
radius
color
ambient, diffuse, specular, and reflection coefficients
shininess

type of object (2 for cube)
lowest x y z ordinate
length of side 
color
ambient, diffuse, specular, and reflection coefficients
shininess


number of light sources

positions of first light source

```


# Sample Outputs

<p align="center">
<img src="https://github.com/ieranik/raytracer/blob/main/images/output1.bmp">

<p align="center">
<img src="https://github.com/ieranik/raytracer/blob/main/images/output2.bmp">






