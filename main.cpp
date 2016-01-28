#include "mesh.h"
#include "glew.h"
#include "glut.h"
#include "FreeImage.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "view.h"
#include "scene.h"
#define TESTSCENE 2
#define MAX_NUM_OBJECT 100
#define MAX_NUM_TEXTURE 100
using namespace std;

view v;
scene s;

// function define
void load_light();
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);

// parameter define
int windowSize[2];

int main(int argc, char** argv)
{
	v.load_view(v, TESTSCENE);

	glutInit(&argc, argv);
	glutInitWindowSize(v.viewport_width, v.viewport_height);
	glutInitWindowPosition(v.viewport_x, v.viewport_y);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("HW1-2");
	glewInit();
	glGenTextures(MAX_NUM_TEXTURE, s.tex_obj);
	s.load_scene(s, TESTSCENE);

	for (int i = 0; i < 101; i++)
		cout << s.texture_table[i] << endl;
	//for (int i = 0; i < 101; i++)
		//cout << s.tex_obj[i] << endl;

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

void load_light()
{
	ifstream ifs;
	string token;
	if (TESTSCENE == 1)
		ifs.open("Scene\\TestScene1\\park.light");
	if (TESTSCENE == 2)
		ifs.open("Scene\\TestScene2\\Chess.light");

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);

	int i = 0;
	float temp_x, temp_y, temp_z;

	while (ifs >> token && token == "light" && i<8)
	{
		ifs >> temp_x >> temp_y >> temp_z;
		GLfloat light_position[] = { temp_x, temp_y, temp_z, 1.0f };

		ifs >> temp_x >> temp_y >> temp_z;
		GLfloat light_ambient[] = { temp_x, temp_y, temp_z, 1.0f };

		ifs >> temp_x >> temp_y >> temp_z;
		GLfloat light_diffuse[] = { temp_x, temp_y, temp_z, 1.0f };

		ifs >> temp_x >> temp_y >> temp_z;
		GLfloat light_specular[] = { temp_x, temp_y, temp_z, 1.0f };

		// set light property
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient);
		i++;
	}

	ifs >> temp_x >> temp_y >> temp_z;
	GLfloat ambient[] = { temp_x, temp_y, temp_z };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	ifs.close();
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

													   // viewport transformation
	glViewport(v.viewport_x, v.viewport_y, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(v.fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], v.dnear, v.dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(v.eye.x, v.eye.y, v.eye.z,	// eye
		v.vat.x, v.vat.y, v.vat.z,  // center
		v.vup.x, v.vup.y, v.vup.z); // up	

									//注意light位置的設定，要在gluLookAt之後
	load_light();

	for (int t = 0, k = 0; k < s.obj_num; k++)
	{
		glPushMatrix();//儲存現在的矩陣 (目前是modelview)
		glTranslatef(s.transfer_vector[k].x, s.transfer_vector[k].y, s.transfer_vector[k].z);
		glRotatef(s.angle[k], s.rotation_axis_vector[k].x, s.rotation_axis_vector[k].y, s.rotation_axis_vector[k].z);
		glScalef(s.scale_value[k].x, s.scale_value[k].y, s.scale_value[k].z);

		int lastMaterial = -1;

		// bind texture
		switch (s.texture_mode[k])
		{
			case 1:
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s.tex_obj[s.texture_table[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);
				break;

			case 2:
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s.tex_obj[s.texture_table[k] - 1]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s.tex_obj[s.texture_table[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				break;

			case 3:
				glActiveTexture(GL_TEXTURE0);
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
				glEnable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, s.tex_obj[s.texture_table[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);
				break;
		}

		for (int i = 0; i < s.object[k]->fTotal; i++)
		{
			// set material property if this face used different material
			if (lastMaterial != s.object[k]->faceList[i].m)
			{
				lastMaterial = (int)s.object[k]->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, s.object[k]->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, s.object[k]->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, s.object[k]->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &s.object[k]->mList[lastMaterial].Ns);

				//you can obtain the texture name by object[k]->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}

			glBegin(GL_TRIANGLES);
			for (int j = 0; j < 3; j++)
			{
				if (s.texture_mode[k] == 1 || s.texture_mode[k] == 3)
					glTexCoord2fv(s.object[k]->tList[s.object[k]->faceList[i][j].t].ptr);
				if (s.texture_mode[k] == 2)
				{
					glMultiTexCoord2fv(GL_TEXTURE0, s.object[k]->tList[s.object[k]->faceList[i][j].t].ptr);
					glMultiTexCoord2fv(GL_TEXTURE1, s.object[k]->tList[s.object[k]->faceList[i][j].t].ptr);
				}

				glNormal3fv(s.object[k]->nList[s.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s.object[k]->vList[s.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
		}

		// unbind texture
		switch (s.texture_mode[k])
		{
			case 1:
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;

			case 2:
				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;

			case 3:
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_CUBE_MAP);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_S);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				break;
		}

		glPopMatrix();//拿stack中的矩陣到指定矩陣(modelview),會回到 I * ModelView
	}

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y)
{
	float x_rotate, z_rotate;
	switch (key)
	{
	case 'w':
		v.fovy += 1;
		break;

	case 's':
		v.fovy -= 1;
		break;

	case 'd':
		x_rotate = v.eye.x - v.vat.x;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.x = x_rotate*cos(-0.5) + z_rotate*sin(-0.5);
		v.eye.z = z_rotate*cos(-0.5) - x_rotate*sin(-0.5);
		break;

	case 'a':
		x_rotate = v.eye.x - v.vat.x;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.x = x_rotate*cos(0.5) + z_rotate*sin(0.5);
		v.eye.z = z_rotate*cos(0.5) - x_rotate*sin(0.5);
		break;

	default:
		break;
	}

	glutPostRedisplay();
}
