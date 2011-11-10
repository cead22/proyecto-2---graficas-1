#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif
#include <math.h>
#include "TriMesh.h"
#include "Trimesh.cc"
#include <iostream>

#define ESCAPE 27
#define SPACEBAR 32

// timer variables
static int start_time;
static int previous_time;
static int timer = 10;

// ball
static float ball_x = 0.0;
static float ball_y = 0.0;
static float ball_z = 0.0;

static float ball_speed_x = 5*0.1;
static float ball_speed_y = 5*0.2;
static float ball_speed_z = 5*0.15;

static int ball_direction_x = 1;
static int ball_direction_y = 1;
static int ball_direction_z = 1;



// camera
// static float camera_rotation_x = 0.0;
// static float camera_rotation_y = 1.0;
// static float camera_rotation_z = 0.0;

static float camera_x = 0.0;
static float camera_y = 0.0;



// static float rx = 0.0;
// static float ry = 0.0;
// static float rz = 0.0;
// static float ra = 0.0;
// 
// static void rotate(float x, float y, float z, float a) {
// 	glRotatef(x,y,z,a);
// }

void importTrimesh(){
	int i;
		const char *filename[1] = {"modelos/bunny.ply"};
	// TriMesh *m = TriMesh::read(filename[0]);
        // const char *filename[3] = {"OBJECTS/lion-03.obj","OBJECTS/cat-03.obj","OBJECTS/horse-01.obj"};
        for (i = 0; i < 1; i++) {
                TriMesh *themesh = TriMesh::read(filename[i]);
                themesh->need_normals();
                themesh->need_tstrips();
                themesh->need_bsphere();
                meshes.push_back(themesh);
                // triObjs[i].sphereX = themesh->bsphere.center[0];
                // triObjs[i].sphereY = themesh->bsphere.center[1];
                // triObjs[i].sphereZ = themesh->bsphere.center[2];
                // triObjs[i].radius = themesh->bsphere.r;

                string xffilename = xfname(filename[i]);
                xffilenames.push_back(xffilename);

                xforms.push_back(xform());
                visible.push_back(true);
        }
}

void display () {
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0f, 1.0f, 1.0f ,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// glPushMatrix();
	// glColor4f(0.0,1.0,0.0,0.2);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,1.0,0.5});
	// glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,0.0,1.0,0.1});
	// glutSolidCube(100.0);
	// glPopMatrix();
	
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){1.0,0.0,0.0,1.0});
	glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){1.0,0.0,0.0,1.0});
	glColor4f(1.0,0.0,0.0,1.0);
	glTranslatef(ball_x,ball_y,ball_z);
	glutSolidSphere(1.0,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){1.0,0.0,0.0,1.0});
	glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){1.0,0.0,0.0,1.0});
	glScalef(100,100,100);
	draw_mesh(0);
	glPopMatrix();
	// paint
	glutSwapBuffers();
}

static void play(int value) {
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int time_since_previous_frame = current_time - previous_time;
	int elapsed_time = current_time - start_time;
	
	// Set up the next timer tick (do this first)
    glutTimerFunc(timer, play, 0);
	

	// collision ball-cube
	if (ball_x >= 50.0 || ball_x <= -50.0) {
		// printf("collision x\n");
		ball_direction_x *= -1;
	}
	if (ball_y >= 50.0 || ball_y <= -50.0) {
		// printf("collision y\n");
		ball_direction_y *= -1;
	}
	if (ball_z >= 50.0 || ball_z <= -50.0) {
		// printf("collision z\n");
		ball_direction_z *= -1;
	}
	ball_x += ball_direction_x * ball_speed_x;
	ball_y += ball_direction_y * ball_speed_y;
	ball_z += ball_direction_z * ball_speed_z;
	
	// Force a redisplay to render the new image
	glutPostRedisplay();

	// timer
	previous_time = current_time;
}

static void key(unsigned char k, int x, int y) {
	// seed for random number
	// unsigned int iseed = (unsigned int) time (0);
	// srand (iseed);
	
	switch (k) {
	// escape
	case ESCAPE:
		// std::exit(0);
	break;
	case SPACEBAR:
		break;
	default:
		return;
	}

	// paint
	glutPostRedisplay();
}

void key_special(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_DOWN:
			// camera_y -= 5.0;
			glRotatef(-5.0,1.0,camera_y,0.0);
			camera_x += 5.0;
			break;
		case GLUT_KEY_UP:
			// camera_y += 5.0;
			glRotatef(5.0,1.0,camera_y,0.0);
			camera_x -= 5.0;
			break;
		case GLUT_KEY_LEFT:
			glRotatef(5.0,camera_x,1.0,0.0);
			camera_y -= 5.0;
			break;
		case GLUT_KEY_RIGHT:
			glRotatef(-5.0,camera_x,1.0,0.0);
			camera_y += 5.0;
			// camera_x += 5.0;
			break;
		default: 
			return;
	}
	// printf("x: %f\ny: %f\nz: %f\n\n", camera_x,camera_y,camera_z);
	glutPostRedisplay();
}

// move directional bar with mouse
void mouse (int x, int y) {
	
	// paint
	glutPostRedisplay();
}

void reshape (int width, int height) {
	// viewport transf.
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); 

	// project. matrix
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	gluPerspective(40, (GLfloat)width / (GLfloat)height, 100.0, 300.0); 
	
	// modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0,0.0,200.0,0.0,0.0,0.0,0.0,1.0,0.0);
	// gluLookAt(camera_x,camera_y,100.0,0.0,0.0,0.0,0.0,1.0,0.0);
}

void light_config() { 
	GLfloat light_diffuse[] = {.5, .5, .5, 1.0};
	GLfloat light_ambient[] = {.5, .5, .5, 1.0};
	GLfloat light_specular[] = {.7, .7, .7, 1.0};
	GLfloat light_position[] = {0.0, 60, 0, 0.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

int main (int argc, char *argv[]) {
	// window settings
	glutInit(&argc, argv);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(20,20);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("Play");
	
	// lights
	light_config();
	
	// hooks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutSpecialFunc(key_special);
	glutPassiveMotionFunc(mouse);
	
	
	glEnable(GL_BLEND); //Enable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function

	importTrimesh();	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Start the timer
    glutTimerFunc(timer, play, 0);

	// initialize the time vars
	start_time = glutGet(GLUT_ELAPSED_TIME);
	previous_time = start_time;




	glutMainLoop();
	return 0;
}

