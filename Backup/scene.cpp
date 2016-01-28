#include "scene.h"
#include <iostream>


scene::scene()
{
	obj_num = 0;
	texture_num = 0;
}

scene::~scene()
{
	
}

void scene::load_scene(scene &scene, int test_scene)
{
	ifstream 		ifs;
	string			token, temp, cube[6];
	stringstream	ss;
	int				mode = 0;

	if (test_scene == 1)
		ifs.open("Scene\\TestScene1\\park.scene");
	else if (test_scene == 2)
		ifs.open("Scene\\TestScene2\\Chess.scene");
	else
		cout << "File not found";


	while (ifs >> token)
	{
		if (token == "model")
		{
			ifs >> token;
			
			if (test_scene == 1)
				scene.obj_file_name[obj_num] = "Scene\\TestScene1\\" + token;
			if (test_scene == 2)
				scene.obj_file_name[obj_num] = "Scene\\TestScene2\\" + token;

			ifs >> scene.scale_value[obj_num].x >> scene.scale_value[obj_num].y >> scene.scale_value[obj_num].z;
			ifs >> scene.angle[obj_num] >> scene.rotation_axis_vector[obj_num].x >> scene.rotation_axis_vector[obj_num].y >> scene.rotation_axis_vector[obj_num].z;
			ifs >> scene.transfer_vector[obj_num].x >> scene.transfer_vector[obj_num].y >> scene.transfer_vector[obj_num].z;

			scene.texture_mode[obj_num] = mode;
			scene.texture_table[obj_num] = texture_num-1;
			
			scene.obj_num++;
		}

		else if (token == "single-texture") 
		{
			mode = 1;
			getline(ifs, token);
			ss.str("");
			ss.clear();
			ss << token;

			ss >> temp;
			if (test_scene == 1)
				temp = "Scene\\TestScene1\\" + temp;
			if (test_scene == 2)
				temp = "Scene\\TestScene2\\" + temp;
			load_texture(temp, texture_num);
			scene.texture_num++;
		}

		else if (token == "multi-texture")
		{
			mode = 2;
			getline(ifs, token);
			ss.str("");
			ss.clear();
			ss << token;

			while (ss >> temp)
			{
				if (test_scene == 1)
					temp = "Scene\\TestScene1\\" + temp;
				if (test_scene == 2)
					temp = "Scene\\TestScene2\\" + temp;

				load_texture(temp, texture_num);
				scene.texture_num++;
			}
		}

		else if (token == "cube-map")
		{
			mode = 3;
			getline(ifs, token);
			ss.str("");
			ss.clear();
			ss << token;
			int k = 0;

			while (ss >> temp)
			{
				if (test_scene == 1)
					temp = "Scene\\TestScene1\\" + temp;
				if (test_scene == 2)
					temp = "Scene\\TestScene2\\" + temp;

				cube[k++] = temp;
			}

			load_cube(cube, texture_num);
			scene.texture_num++;
		}

		else
			mode = 0;
	}

	ifs.close();
	
}

void scene::load_texture(string Filename, int index)
{
	char* pFilename;
	pFilename = (char*)Filename.c_str();

	FIBITMAP *pImage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int width = FreeImage_GetWidth(p32BitsImage);
	int height = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, tex_obj[index]);

#ifdef MIPMAP
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, iWidth, iHeight, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
}

void scene::load_cube(string Filename[6], int index)
{
	char* pFilename[6];
	for (int i = 0; i<6; i++)
		pFilename[i] = (char*)Filename[i].c_str();

	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_obj[index]);
	for (int i = 0; i<6; i++) {
		FIBITMAP *pImage = FreeImage_Load(FreeImage_GetFileType(pFilename[i], 0), pFilename[i]);
		FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
		int width = FreeImage_GetWidth(pImage);
		int height = FreeImage_GetHeight(pImage);
#ifdef MIPMAP
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 4, iWidth, iHeight, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
#else
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
#endif
		FreeImage_Unload(p32BitsImage);
		FreeImage_Unload(pImage);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}