In this project each student must implement the active contour algorithm. The program
must load a grayscale PPM image and a list of contour points. The contour points must
be processed through the active contour algorithm using the options given below. The
program must output a copy of the image with the initial contour drawn on top of it, and a
second image with the final contour drawn on top of it. The program must also output a
list of the final contour pixel coordinates.

The active contour should use 2 internal energy terms and 1 external energy term. The
internal energy terms are the square of the distances between points, and the square of the
deviation from the average distance between points. 
It can be assumed that the contour encloses an area, so that the last contour point can be
connected to the first contour point to calculate internal energy terms. The external
energy term is the square of the image gradient magnitude, and should be calculated
using convolution with a Sobel template.

The window around each contour point should be 7x7 pixels. Each energy term should
be normalized by rescaling from min-max value to 0-1. Each energy term should be
weighted equally. The active contour algorithm should run for 30 iterations.