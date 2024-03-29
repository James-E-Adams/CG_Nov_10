//
//  Model.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
// Extended by James Adams and Shiran Gazit.
//

#ifndef __ex0__Model__
#define __ex0__Model__

#include <iostream>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/detail/type_vec3.hpp>
#include <vector>

#else
#include <GL/gl.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/detail/type_vec3.hpp>
#include <vector>
#endif


#define BALL_SIZE 0.1f
#define TRIANGLE_AMOUNT 30.0
#define WINDOW_SIZE 600
#define HIGH_RAND_RANGE 0.02f
#define LOW_RAND_RANGE -0.02f
#define SCALE_FACTOR 0.965f



class Model {
	
	GLuint _vao, _vbo;

	// Attribute handle:
	GLint _posAttrib;
	
	// Uniform handle:
	GLint _fillColorUV;
	GLint _scaleUV;

	// View port frame:
	float _width, _height, _offsetX, _offsetY;

    //The model of the object, the current cordinate system the shape is in.
	glm::mat4 _modelMatrix;
    //The translation 3 factors, a,b,c, x+a, y+b, z+c
    glm::vec3 _translateVec;
	glm::vec3 _scaleVec;

	int _number_of_circles;

    std::vector<float> vertices;
	std::vector<glm::vec3> transformationVec;
	std::vector<glm::mat4> modelMatVec;
	std::vector<glm::vec3> colorVec;
	//ball_sizes stores the current ball_size, while original stores the ball size with which it was created
	std::vector<float> ball_sizes;
	std::vector<float> ball_sizes_original;
	const glm::vec4 _light_source = glm::vec4(2.f, 1.f, 0.f, 1.f);
public:
	Model();
	virtual ~Model();
	void init();
	void draw();
	void resize(int width, int height);
    void createTransform(int circle_id);
    void create_new_ball(int x_pos,int y_pos);


private:
	int check_border_x(int circle_id);
	int check_border_y(int circle_id);
	float find_ball_size(float x, float y);
	float pre_collision(int current_ball_id,float x, float y, float ball_size);
	void shrink();
	void grow();
	void add_vertices(float x, float y, float ball_size);
	void printMat(glm::mat4 matrix);
	glm::vec3 generateRand(float high, float low, int color);
	float orthogonalise_x(int width);
	float orthogonalise_y(int height);
};

#endif /* defined(__ex0__Model__) */
