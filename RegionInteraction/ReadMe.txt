In this project each student must implement interactive region growing.

The program should visualize the region growing by coloring pixels as they join the
growing region. The program should have a GUI option that allows the user to select the
color for pixels that join the region. The program should also have an option that clears
the result of a previous region grow, displaying the original image.

The program should have a menu option to grow the region in “play” mode or in “step”
mode. In play mode, a pixel should join the region each 1 ms. In step mode, a pixel
should join the region each time the user presses the key “j”. The program should allow
the user to change between modes while a region is growing.

The program should use a dialog box to allow the user to select values for two predicates
for joining the region. The first predicate is the absolute difference of the pixel intensity
to the average intensity of pixels already in the region. (To join, a pixel must be within
this range.) The second predicate is the distance of the pixel to the centroid of pixels
already in the region. (To join, a pixel must be within this range.) Both predicates
should be applied at the same time while growing a region.