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

Changes made:Lots of new functions required to implement the feature below. Vertex and Fragment shader re-written.


Implementation notes:

Initial animation/colour of ball by: Randomly generating transformations/colour upon creation of a ball.
Wall collisions: Check to see if the ball will hit the wall. If so, reflect the transformation on the correct axis.
				i.e if it hit a vertical wall, reflection in the x axis of the translation.

Generation of new balls: Check to see if the new ball will be in range of a wall/other ball collision, and account
						for this.

Shader/light source: Calculated the direction between the fixed light source and the centre of each ball. Calculated the
					location of the 'highlight' on each ball, halfway from the centre. And then, normalised the
					directional vector to world coordinates. Calculated the distance between the highlight and every
					point on the ball. Adjusted the light proportionally.

Collisions between balls: Calculated whether the balls would collide. If so, shrunk them proportionally to avoid
						  the collision. Later on, growth function accounts for the regrowth of the balls, post-collision.













