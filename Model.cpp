//
//  Model.cpp
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
//
//
// Extended by James Adams and Shiran Gazit.
//

#include "ShaderIO.h"
#include "Model.h"

#include <cmath>

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
//#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#define SHADERS_DIR "shaders/"



//constructor
Model::Model() :
_vao(0), _vbo(0), _width(WINDOW_SIZE), _height(WINDOW_SIZE), _modelMatrix(glm::mat4(1.f)),
_translateVec(glm::vec3(0.f)),_number_of_circles(0)
{

}
//deconstructor
Model::~Model()
{
	if (_vao != 0)
		glDeleteVertexArrays(1, &_vao);
	if (_vbo != 0)
		glDeleteBuffers(1, &_vbo);

}


//initialising function

void Model::init()
{
	programManager::sharedInstance()
	.createProgram("default",
				   SHADERS_DIR "SimpleShader.vert",
				   SHADERS_DIR "SimpleShader.frag");

	GLuint program = programManager::sharedInstance().programWithID("default");

	// Obtain uniform variable handles:
	_fillColorUV  = glGetUniformLocation(program, "fillColor");

	//init identity scale
	_scaleUV = glGetUniformLocation(program, "scale");
	glUniformMatrix4fv(_scaleUV, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));

    //init the translation to Identity matrix
	modelMatVec.push_back(glm::mat4(1.f));
	transformationVec.push_back(generateRand(HIGH_RAND_RANGE, LOW_RAND_RANGE, 0));
	colorVec.push_back(generateRand(0.f, 1.f, 1));
    createTransfrom(0);


   GLint uniform_windowSize = glGetUniformLocation(program, "window");
    glUniform2f(uniform_windowSize, _width, _height);


		//initialise a circle centre of screen
		add_vertices(0.0f,0.0f,BALL_SIZE);
		ball_sizes.push_back(BALL_SIZE);

		ball_sizes_original.push_back(BALL_SIZE);

		//increment circles counter
		_number_of_circles++;
		// Create and bind the object's Vertex Array Object:
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// Create and load vertex data into a Vertex Buffer Object:
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

		// Tells OpenGL that there is vertex data in this buffer object and what form that vertex data takes:

		// Obtain attribute handles:
		_posAttrib = glGetAttribLocation(program, "position");
		glEnableVertexAttribArray(_posAttrib);
		glVertexAttribPointer(_posAttrib, // attribute handle
							  4,          // number of scalars per vertex
							  GL_FLOAT,   // scalar type
							  GL_FALSE,
							  0,
							  0);

		// Unbind vertex array:
		glBindVertexArray(0);
	
}



void Model::draw()
{

	// Set the program to be used in subsequent lines:
	GLuint program = programManager::sharedInstance().programWithID("default");
	GLint uniform_direction = glGetUniformLocation(program, "highLight");
	GLint uniform_size = glGetUniformLocation(program, "ballSize");
	glUseProgram(program);

	//Set polygon draw mode
	GLenum polygonMode = GL_FILL;   // Also try using GL_FILL and GL_POINT
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// Draw using the state stored in the Vertex Array object:
	glBindVertexArray(_vao);

	//number of triangles
	size_t numberOfVertices = TRIANGLE_AMOUNT + 2;


	//loop through and draw all the circles, creating the correct transformations
	for(int i = 0; i < _number_of_circles; ++i)
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfVertices);
		createTransfrom(i);
		glUniform4f(_fillColorUV, colorVec[i].r, colorVec[i].g, colorVec[i].b, 1.0);

		glm::vec4 direction = (_light_source - modelMatVec[i][3]) / glm::distance(modelMatVec[i][3], _light_source) *
							  (ball_sizes[i] /2) + modelMatVec[i][3];
		glUniform4f(uniform_direction, direction.x, direction.y, direction.z, direction.w);
		glUniform1f(uniform_size, ball_sizes[i]);
	}

	// Unbind the Vertex Array object
	glBindVertexArray(0);

	// Cleanup, not strictly necessary
	glUseProgram(0);
}






//method to account for window resize with the given width and height.
void Model::resize(int width, int height)
{
    GLuint program = programManager::sharedInstance().programWithID("default");
    glUseProgram(program);

    _width	= width;
    _height = height;

    GLint uniform_windowSize = glGetUniformLocation(program, "window");
    glUniform2f(uniform_windowSize, _width, _height);
    _offsetX = 0;
    _offsetY = 0;
}





/**
 *
 *
 *********************************************************************
 * Function  :   createTransform
 * Arguments :
 * Returns   :
 * Throws    :   n/a
 *
 * Purpose   : Transform the vertices in the animation. Checks if window bounds were detected (in the path of the balls),
 * change the the transformation/translation direction, as well as calling the necessary grow/shrink functions.
 *
 *
 \******************************************************************/
void Model::createTransfrom(int circle_id)
{
    GLuint program = programManager::sharedInstance().programWithID("default");
    glUseProgram(program);

    GLint uniform_trans = glGetUniformLocation(program, "translation");
    glm::mat4 translation;

	//growing for collisions.
	grow();

	//checks if the ball is going to hit a wall. If so, reflect off the wall as required.
    if (check_border_x(circle_id)) transformationVec[circle_id].x*=-1.f;

    if (check_border_y(circle_id)) transformationVec[circle_id].y*=-1.f;

	//Performs the necessary translation

    translation = glm::translate(modelMatVec[circle_id], transformationVec[circle_id]);
    glUniformMatrix4fv(uniform_trans, 1, GL_FALSE, glm::value_ptr(translation));
	modelMatVec[circle_id] = translation;

	//shrinking for collisions.
	shrink();

}
/********************************************************************
 * Function  :   grow
 * Arguments :
 * Returns   :
 * Throws    :   n/a
 *
 * Purpose   :   This function handles the growing of balls, post-collision, with a safe-guard to ensure
 * 				 they don't end up larger than they started.
 *
 *
 \******************************************************************/

void Model::grow() {

	for (int i=0; i<_number_of_circles; i++) {

		//this means we need to resize (i.e enlarge) the ball.
		if (ball_sizes[i] < ball_sizes_original[i]) {
			//safeguard to avoid making the ball larger than it was originally.
			if (ball_sizes[i]/SCALE_FACTOR > ball_sizes_original[i]) {
				float adjusted_scale_factor = ball_sizes[i]/ball_sizes_original[i];
				glm::mat4 scale = glm::scale(modelMatVec[i], glm::vec3(1/adjusted_scale_factor,
																	   1/adjusted_scale_factor, 0.f));
			}
			else {
				glm::mat4 scale = glm::scale(modelMatVec[i], glm::vec3(1 / SCALE_FACTOR, 1 / SCALE_FACTOR, 0.f));
				modelMatVec[i] = scale;
				ball_sizes[i] *= 1 / SCALE_FACTOR;
			}
		}

	}
}
/********************************************************************
 * Function  :   shrink
 * Arguments :
 * Returns   :
 * Throws    :   n/a
 *
 * Purpose   :   This function handles the shrinking of balls, in order to avoid potential collisions.
 *
 *
 \******************************************************************/
void Model::shrink() {
	float x_1,x_2,y_1,y_2;

	//needs to be a nested loop, so that every ball-ball collision is considered.
	for (int i=0; i<_number_of_circles; i++) {
		//store ball 1's centre.
		x_1=modelMatVec[i][3].x;
		y_1=modelMatVec[i][3].y;

		for(int j=0; j<_number_of_circles; j++) {
			//skip over if looking at ball x colliding with ball x.
			if (i==j) continue;
			//store ball 2's centre.
			x_2=modelMatVec[j][3].x;
			y_2=modelMatVec[j][3].y;
			//calculate the distance between the two balls.
			float x_distance = x_2-x_1;
			float y_distance = y_2-y_1;
			float distance = sqrt(x_distance*x_distance + y_distance*y_distance);
			//scale both balls adequately
			glm::mat4 scale_i = glm::scale(modelMatVec[i], glm::vec3(SCALE_FACTOR, SCALE_FACTOR, 0.f));
			glm::mat4 scale_j = glm::scale(modelMatVec[j], glm::vec3(SCALE_FACTOR, SCALE_FACTOR, 0.f));
			if (distance<=(ball_sizes[i]+ball_sizes[j])) {
				if(ball_sizes[i] > 0.03)
				{
					modelMatVec[i] = scale_i;

					ball_sizes[i] *= SCALE_FACTOR;
				}
				if(ball_sizes[j] > 0.03)
				{
					modelMatVec[j] = scale_j;
					ball_sizes[j] *= SCALE_FACTOR;

				}
			}
		}
	}
}


/********************************************************************
 * Function  :   check_border_x
 * Arguments :   circle_id  : which circle to check
 * Returns   :   1 if hits the border, 0 if not.
 * Throws    :   n/a
 *
 * Purpose   :   This function handles the checking of horizontal border collisions.
 *
 \******************************************************************/
int Model::check_border_x(int circle_id) {
	float ball_size;
	if (ball_sizes.size()>0) ball_size=ball_sizes[circle_id];
	else ball_size= BALL_SIZE;

	if( (modelMatVec[circle_id][3].x > (1 - ball_size)) ||
			(modelMatVec[circle_id][3].x < -(1 - ball_size)) ) return 1;
	return 0;
}

/********************************************************************
 * Function  :   check_border_y
 * Arguments :   circle_id  : which circle to check
 * Returns   :   1 if hits the border, 0 if not.
 * Throws    :   n/a
 *
 * Purpose   :   This function handles the checking of vertical border collisions.
 *
 \******************************************************************/
int Model::check_border_y(int circle_id) {
	float ball_size;
	if (ball_sizes.size()>0) ball_size=ball_sizes[circle_id];
	else ball_size= BALL_SIZE;

	if ( (modelMatVec[circle_id][3].y > (1 - ball_size)) ||
			 (modelMatVec[circle_id][3].y < -(1 - ball_size))) return 1;
	return 0;
}




/********************************************************************
 * Function  :   generateRand
 * Arguments :   high  : upper bound for randomisation
 *               low  : lower bound for randomisation
 *               color: Switch. If 1, returns a vector which suits a randomised color.
 * Returns   :   randomised vector, suitable for a colour if the given mode is enabled.
 * Throws    :   n/a
 *
 * Purpose   :   This function handles the generation of a suitable random translation/colour.
 *
 \******************************************************************/
glm::vec3 Model::generateRand(float high, float low, int color)
{
	float x_trans = low + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	//std::cout<<"x:"<< x_trans<< " ";
	float y_trans = low + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	float z_trans = 0.f;
	if(color)
	{
		z_trans = low + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	}
	//std::cout<<"y:"<< y_trans<<std::endl;
	return glm::vec3(x_trans, y_trans, z_trans);
}



//using james' method. TODO (Is it correct?)
float Model::orthogonalise_x(int x_pos) {
	return ((float)(x_pos-_width)/_width)*2+1;
}

float Model::orthogonalise_y(int y_pos) {
	return (((float)(y_pos-_height)/_height)*2+1)*-1;
}

/********************************************************************
 * Function  :   create_new_ball
 * Arguments :   x_pos  : x value of the mouse location at which to create a new ball
 *               y_pos  : y value of the mouse location at which to create a new ball
 * Returns   :   n/a
 * Throws    :   n/a
 *
 * Purpose   :   This function handles the creation of a new ball at the given mouse click.
 *				Called within ex0::mouse function. 
 *
 \******************************************************************/
void Model::create_new_ball(int x_pos,int y_pos) {


	//increment number of circles/balls.
	_number_of_circles++;

	//calculate the vertex co-ordinate based on the mouse click in the window.
	float x=orthogonalise_x(x_pos);
	float y=orthogonalise_y(y_pos);

	//puts the new position into the matrix vector
	modelMatVec.push_back(glm::translate(glm::vec3(x, y, 0.f)) * glm::mat4(1.f));
	//generates the random direction which the new ball will move in
	transformationVec.push_back(generateRand(HIGH_RAND_RANGE, LOW_RAND_RANGE, 0));

	//generate random colour for the new ball, ensuring not too dark/light
	colorVec.push_back(generateRand(0.f,0.4f, 1));


	//initialise ball_size to use the check_border functions
	ball_sizes.push_back(BALL_SIZE);
	//find the real ball size according to potential collisions
	float ball_size = find_ball_size(x,y);
	ball_sizes.pop_back();
	ball_sizes.push_back(ball_size);
	//store original in old
	ball_sizes_original.push_back(ball_size);
	//matrix scaling
	float scale_factor=ball_size/BALL_SIZE;
	glm::mat4 scale = glm::scale(modelMatVec[_number_of_circles-1], glm::vec3(scale_factor, scale_factor, 0.f));

	//Make sure the new vertices get 'added' to the buffer.
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
}


/********************************************************************
 * Function  :   find_ball_size
 * Arguments :   x_pos  : x value of the mouse location at which to create a new ball
 *               y_pos  : y value of the mouse location at which to create a new ball
 * Returns   :   ball_size: ball size to create the new ball with.
 * Throws    :   n/a
 *
 * Purpose   :   This function works out what size to make the new ball, based on borders/other balls.
 *
 \******************************************************************/
float Model::find_ball_size(float x, float y) {
	//std::cout<<"entering find_ball_size\n";
	float ball_size=BALL_SIZE;

	//check to see if it will hit a wall.
	if (check_border_x(_number_of_circles-1) || check_border_y(_number_of_circles-1)) ball_size=ball_size/5;

	//loop is -1 because we don't want to check if it will collide with itself
	for(int i=0; i<_number_of_circles-1; i++) {

		float collision = pre_collision(i, x,y,ball_size);

		if (collision>0.0f) {
			//std::cout<<"There was a collision. \n";
			ball_size=collision;
		}
	}
	return ball_size;
}

/********************************************************************
 * Function  :   pre_collision
 * Arguments :   current_ball_id: The already existing ball we are comparing with.
 *               x : x value of the centre of the new ball to be created.
 *               y : y value of the centre of the new ball to be created.
 *               ball_size: current prospective ball size of the new ball to be created
 * Returns   :   new ball size
 * Throws    :   n/a
 *
 * Purpose   :   This function works out what size to make the new ball, based on borders/other balls.
 *
 \******************************************************************/
float Model::pre_collision(int current_ball_id,float x, float y, float ball_size) {

	//printMat(modelMatVec[current_ball_id]);
	float x_current=modelMatVec[current_ball_id][3].x;
	float y_current=modelMatVec[current_ball_id][3].y;

	float x_distance = x_current-x;
	float y_distance = y_current-y;
	float distance = sqrt(x_distance*x_distance + y_distance*y_distance);

	if (distance>(ball_sizes[current_ball_id] + ball_size)) return 0;
	else return distance-ball_sizes[current_ball_id];
}




/********************************************************************
 * Function  :   add_vertices
 * Arguments :   x  : x value of the centre vertex of the to-be-created circle.
 *               y : y value of the centre vertex of the to-be-created circle.
 * Returns   :   n/a
 * Throws    :   n/a
 *
 * Purpose   :   This function generates the required vertices and adds them to the vertex vector.
 *
 \******************************************************************/
void Model::add_vertices(float x, float y, float ball_size) {
	//put the centre in.
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(0.0f);
	vertices.push_back(1.0f);
	//TODO add guards for creating next to walls
		for (int i=0; i <= TRIANGLE_AMOUNT; i++) {
			vertices.push_back((float)(ball_size * std::cos(2.0 * M_PI * i / TRIANGLE_AMOUNT)));
            vertices.push_back((float)(ball_size * std::sin(2.0 * M_PI * i / TRIANGLE_AMOUNT)));
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
		}
}





//debug function for printing matrices

void Model::printMat(glm::mat4 matrix)
{
    std::cout << "Matrix:";
   for(int i = 0; i < 4; ++i)
   {
       for(int j = 0; j < 4; ++j)
       {
           std::cout << matrix[i][j] << " ";
       }
       std::cout << std::endl;
   }
}
