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
	std::cout<<"ini\n";

	programManager::sharedInstance()
	.createProgram("default",
				   SHADERS_DIR "SimpleShader.vert",
				   SHADERS_DIR "SimpleShader.frag");

	GLuint program = programManager::sharedInstance().programWithID("default");

	// Obtain uniform variable handles:
	_fillColorUV  = glGetUniformLocation(program, "fillColor");

    //init the translation to Identity matrix
	modelMatVec.push_back(glm::mat4(1.f));
	transformationVec.push_back(generateRand(HIGH_RAND_RANGE, LOW_RAND_RANGE, 0));
	colorVec.push_back(generateRand(0.f, 1.f, 1));
    createTransfrom(0);

//    _translateVec = generateRand(HIGH_RAND_RANGE, LOW_RAND_RANGE);
//	transformationVec.push_back(generateRand(HIGH_RAND_RANGE, LOW_RAND_RANGE));
   


   GLint uniform_windowSize = glGetUniformLocation(program, "window");
    glUniform2f(uniform_windowSize, _width, _height);


	// Initialize vertices buffer and transfer it to OpenGL
	
		//initialise a circle centre of screen
		add_vertices(0.0f,0.0f);
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
	glUseProgram(program);

	//Set polygon draw mode
	GLenum polygonMode = GL_FILL;   // Also try using GL_FILL and GL_POINT
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// Draw using the state stored in the Vertex Array object:
	glBindVertexArray(_vao);

	//TODO
	size_t numberOfVertices = TRIANGLE_AMOUNT + 2;


	//loop through and draw all the circles, creating the correct transformations (TODO)

	for(int i = 0; i < _number_of_circles; ++i)
	{
		glUniform4f(_fillColorUV, colorVec[i].r, colorVec[i].g, colorVec[i].b, 1.0);
		glDrawArrays(GL_TRIANGLE_FAN, i*numberOfVertices, numberOfVertices);
		createTransfrom(i);
	}

    //Make the transform each frame TODO (what goes here? Has it been moved inside the above loop?)


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
 * Transform the vertices in the animation. Checks if window bounds were detected (in the path of the balls) and
 * change the the transformation/translation direction.
 *
 * TODO: Circle jump form window edge to the wanted direction
 * TODO: Make it work with more than one ball
 * TODO: Proper function commenting. 
 */
void Model::createTransfrom(int circle_id)
{
    GLuint program = programManager::sharedInstance().programWithID("default");
    glUseProgram(program);

    GLint uniform_trans = glGetUniformLocation(program, "translation");
    glm::mat4 translation;


    //TODO put into function
    //Checks if the transformation will force the ball to hit a wall
    //if so, bounces off the wall correctly.
    //TODO I don't understand any of these variable names. Please fix!


    if( (modelMatVec[circle_id][3].x > (1 - BALL_SIZE)) ||
			(modelMatVec[circle_id][3].x < -(1 - BALL_SIZE)) ) {

		transformationVec[circle_id].x*=-1.f;

	}
	if ( (modelMatVec[circle_id][3].y > (1 - BALL_SIZE)) || (modelMatVec[circle_id][3].y < -(1 - BALL_SIZE))) {

		transformationVec[circle_id].y*=-1.f;

	}
	//What's happening here? (TODO)
    translation = glm::translate(transformationVec[circle_id]);
    translation *= modelMatVec[circle_id];
    glUniformMatrix4fv(uniform_trans, 1, GL_FALSE, glm::value_ptr(translation));
	modelMatVec[circle_id] = translation;
//  printMat(_modelMatrix);
}





//generate random vec3 TODO Proper function commenting.
glm::vec3 Model::generateRand(float high, float low, int color)
{
	float x_trans = low + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	std::cout<<"x:"<< x_trans<< " ";
	float y_trans = low + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	float z_trans = 0.f;
	if(color)
	{
		z_trans = low + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	}
	std::cout<<"y:"<< y_trans<<std::endl;
	return glm::vec3(x_trans, y_trans, z_trans);
}



//using james' method. TODO (Is it correct?)
float Model::orthogonalise_x(int x_pos) {
	return ((float)(x_pos-_width)/_width)*2+1;
}

float Model::orthogonalise_y(int y_pos) {
	return (((float)(y_pos-_height)/_height)*2+1)*-1;
}


//using: http://jsfiddle.net/8onndhuw/2/ from http://stackoverflow.com/questions/33605109/converting-from-viewport-coordinates-to-1-12-vertex-opengl/33605229?noredirect=1#comment54993278_33605229
// float Model::orthogonalise_x(int x_pos) {
// 	return 2.0 * (x_pos - _width / 2.0) / _width;
// }

// float Model::orthogonalise_y(int y_pos) {

// 	return -2.0 * (y_pos - _height / 2.0) / _height;
// }


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
	//std::cout<<"create_new_ball\n";
	//std::cout<<"num circles pre: " << _number_of_circles << " \n";

	//increment number of circles/balls.
	_number_of_circles++;


	//std::cout<<"num circles post: " << _number_of_circles << " \n";


	//calculate the vertex co-ordinate based on the mouse click in the window.
	float x=orthogonalise_x(x_pos);
	float y=orthogonalise_y(y_pos);

	//std::cout<<"new x and y are: "<< x << " , " << y << "\n";


	//TODO What's happening here? Can't follow.

	modelMatVec.push_back(glm::translate(glm::vec3(x, y, 0.f)) * glm::mat4(1.f));
	transformationVec.push_back(generateRand(HIGH_RAND_RANGE, LOW_RAND_RANGE, 0));
	colorVec.push_back(generateRand(0.f, 1.f, 1));

	//Adds the relevant vertices of a ball, centred at x,y, to the vertex vector.
	add_vertices(x,y);

	//DEBUG Printing loop.
	// for (int i=0; i<vertices.size(); i++) {
	// 	if (i%124==0)std::cout<<"starting new circle \n";
	// 	std::cout<<vertices[i];
	// 	if ((i+1)%4==0) std::cout<<'\n';
	// 	else std::cout <<',';
	// }
	


	//Make sure the new vertices get 'added' to the buffer.
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
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
void Model::add_vertices(float x, float y) {
	//put the centre in.
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(0.0f);
	vertices.push_back(1.0f);
	//TODO add guards for creating next to walls
		for (int i=0; i <= TRIANGLE_AMOUNT; i++) {
			vertices.push_back((float)(BALL_SIZE * std::cos(2.0 * M_PI * i / TRIANGLE_AMOUNT)));
            vertices.push_back((float)(BALL_SIZE * std::sin(2.0 * M_PI * i / TRIANGLE_AMOUNT)));
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
		}
}







//not used?
void Model::printMat(glm::mat4 matrix)
{
    std::cout << "Matrix:";
//    for(int i = 0; i < 4; ++i)
//    {
//        for(int j = 0; j < 4; ++j)
//        {
//            std::cout << matrix[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
    std::cout << matrix[3].x << " " << matrix[3][1] << " " << matrix[3][2] << " " << matrix[3][3] << std::endl;
}



////generate random vec3
//glm::vec3 generateRand(float high, float low)
//{
//    float x_trans = low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
//    std::cout<<"x:"<< x_trans<< " ";
//    float y_trans = low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
//    std::cout<<"y:"<< y_trans<<std::endl;
//    return glm::vec3(x_trans, y_trans, 0.f);
//}