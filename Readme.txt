jadams 777635004
shirelga 201299344

Description
***********

<<Filename>>: ***<Changes>

Model.cpp: ***Window vec2 into the the vertex shader in order to calculate the window ratio change.
	   ***Calculation of the vertices on the circumference of the circle with the following:
	   x= r * cos((2 * pi) * i(iteration) / amount of triangles.
	   y= r * sin((2 * pi) * i(iteration) / amount of triangles.
	   The vertices were then used in a 'triangle_fan' with the centre (0,0) as the main point.
	   **Adjusted the glbufferdata function call to account for the size of the utilised data struct.
	   **Used GL_Fill insntead of GL_Draw. 
	   
	   
	   

SimpleShader.vert: ***Calculate the ratio and change the vertices location according to the window ratio; maintains uniformity of circle, preventing it transmorphing into an ellipses. 

SimpleShader.frag: *** Implemented the checkerboard pattern. Takes the mod(x) of the co-ordinate, depending on the scale x, and uses the results to determine whether the given fragment should be blue (the fill colour) or black,the supplied colour. 




Files:
******
ex0.cpp
ex0.h
Model.cpp
Model.h
ShaderIO.cpp
ShaderIO.h
Makefile
Readme.txt

URLs used: https://www.opengl.org/discussion_boards/showthread.php/170331-Fragment-shader-checkerboard
	   http://stackoverflow.com/questions/8043923/gl-triangle-fan-explanation



EX1b additional notes:




