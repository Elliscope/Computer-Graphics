README.md

Assignment 1 
CS 420



Code that I added:
1.Initialize the window in main
2.Draw the heightfield based on pixel input in the display function
3.set up the perspective camera in reshape function
4.Create Keyboardfunction to store keyboard command
5.change vertices/triangle/.. texture by key   1 2 3


Instruction to play around:

Run the program with command line like 	./assign1 SantaMonicaMountains-512.jpg

Move mouse to rotate 

Hold "control" and move mouse to move the object

Hold "shift" and move mouse to scale the object

Press the following key to get different texture

1  GL_POINT
2  GL_LINE
3  GL_FILL


Extra credits:
Render wireframe on top of solid triangles (use glPolygonOffset to avoid z-buffer fighting).
-1 2 3 

Color the vertices based on color values taken from another image of equal size. However, your code should still support also smooth color gradients as per the core requirements.
-I colored the vertices based on the height of the map -> pixel value 


Allow the user to interactively deform the landscape.
-User can interact by holding "shift","control";


Thanks for grading!

Mingzhe Fang
2015/9/17