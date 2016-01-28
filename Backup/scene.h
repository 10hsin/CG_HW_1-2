#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
using namespace std;

class scene
{
	public:
		// variable
		struct vertex
		{
			float x, y, z;
		};
		
		float	angle[100];
		vertex	rotation_axis_vector[100];
		vertex	scale_value[100];
		vertex	transfer_vector[100];
		
		string	obj_file_name[100];
		int		obj_num;
		
		string 	texture_name[100];
		int		texture_mode[100];
		int		texture_table[100];
		int		texture_num;
		GLuint	tex_obj[100];
		string	cube_img[6];
		
		// function
		scene();
		~scene();
		void load_scene(scene &scene, int test_scene);
		void load_texture(string Filename, int index);
		void load_cube(string file_name[6], int index);
};