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
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "game.hpp"

Game *game;

using namespace std;
using namespace rapidxml;

#define ESCAPE 27
#define SPACEBAR 32

// timer variables
static int start_time;
static int previous_time;
static int timer = 10;

// ball
// static float ball_x = 0.0;
// static float ball_y = 0.0;
// static float ball_z = 0.0;
// 
// static float ball_speed_x = 5*0.1;
// static float ball_speed_y = 5*0.2;
// static float ball_speed_z = 5*0.15;
// 
// static int ball_direction_x = 1;
// static int ball_direction_y = 1;
// static int ball_direction_z = 1;

Punto *posicion_jugador = (Punto *) malloc(sizeof(Punto));
Punto *posicion_contrincantes;

Movimiento *movimiento_jugador;
Movimiento *movimiento_contrincantes;

// static int puntos_jugador_count = 1;
// Punto *punto_jugador_actual;
// Punto *punto_jugador_proximo;

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

void light_config() { 	
	GLfloat light_diffuse[] = {.2, .2, .2, 1.0};
	GLfloat light_ambient[] = {.8, .8, .8, 1.0};
	GLfloat light_specular[] = {.7, .7, .7, 1.0};
	GLfloat light_position[] = {-10.0, 10, 50, 0.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}
void importTrimesh(){
	int i;
		const char *filename[3] = {"modelos/bunny.ply", "modelos/armadillo.ply", "modelos/cow.ply"};
        for (i = 0; i < 3; i++) {
                TriMesh *themesh = TriMesh::read(filename[i]);
                themesh->need_normals();
                themesh->need_tstrips();
                themesh->need_bsphere();
                meshes.push_back(themesh);
                string xffilename = xfname(filename[i]);
                xffilenames.push_back(xffilename);

                xforms.push_back(xform());
                visible.push_back(true);
        }
}

void draw_floor () {
	glPushMatrix();
		glTranslatef(-50.0f,-50.0f,	50.0f);
		glColor3f(1,1,1);
		glLineWidth(3.0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,-100.0);
			glVertex3f(100.0,0.0,-100.0);
			glVertex3f(100.0,0.0,0.0);
		glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-50.0f,-50.0f,	50.0f);
	// glColor3f(0.0,0.0,1.0);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
		glVertex3f(100.0,0.0,0.0);
		glVertex3f(100.0,0.0,-100.0);
		glVertex3f(0.0,0.0,-100.0);
		glVertex3f(0.0,0.0,0.0);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.0,-100.0);
		glVertex3f(100.0,0.0,-100.0);
		glVertex3f(100.0,0.0,0.0);
	glEnd();
	glPopMatrix();
}

void draw_game_objects () {
	
	int i,j,k;
	Punto *punto;
	Jugador *contrincante = game->niveles->contrincantes;
	Jugador *jugador = &game->niveles->jugador;
	Objeto *objeto = game->niveles->objetos;

	glPushMatrix();
		glTranslatef(-50.0f,-50.0f,	50.0f);
		glColor4f(1.0,0.0,0.0,1.0);
		glLineWidth(2.0);	

	// contrincantes
	for(i = 0; i < game->niveles->cantidad_contrincantes; i++, contrincante++) {
		glBegin(GL_LINE_LOOP);
			for (j = 0, punto = contrincante->trayectoria.puntos; j < contrincante->cantidad_puntos; punto++, j++)
				glVertex3f(punto->x,0.0,-punto->y);
		glEnd();
	}
	
	// jugador
		glBegin(GL_LINE_LOOP);
			for (j = 0, punto = jugador->trayectoria.puntos; j < jugador->cantidad_puntos; punto++, j++)
				glVertex3f(punto->x,0.0,-punto->y);
		glEnd();
		
	glPopMatrix();
		
	// jugador
	glPushMatrix();
		glTranslatef(-50.0f,-46.0f,	50.0f);
		glTranslatef(posicion_jugador->x,0.0,- posicion_jugador->y);
		glScalef(0.07f,0.07f,0.07f);
		setup_lighting(1);
		draw_mesh(1);
	glPopMatrix();
	
	// contrincantes
	for (k = 0; k < game->niveles->cantidad_contrincantes; k++) {
		glPushMatrix();
			glTranslatef(-50.0f,-46.0f,	50.0f);
			glTranslatef(posicion_contrincantes[k].x,0.0,- posicion_contrincantes[k].y);
		glScalef(0.07f,0.07f,0.07f);
			setup_lighting(1);
			draw_mesh(1);
		glPopMatrix();
	}
	
	// objetos
	for(i = 0; i < game->niveles->cantidad_objetos; i++, objeto++) {
		glPushMatrix();
			glTranslatef(-50.0f,-45.f,50.0f);
			glTranslatef(objeto->x, 0.0f, - objeto->y);
			glColor4f(1.0,0.0,0.0,1.0);
			if (objeto->tipo == 1) {
				glPushMatrix();
					glScalef(10.0f,20.0f,20.0f);
					setup_lighting(2);
					draw_mesh(2);
				glPopMatrix();
			}
			else {
				if (strcmp(objeto->obj.gl.primitiva,"cubo")) {
					glutSolidCube(objeto->obj.gl.tamano);
				}
				else if (strcmp(objeto->obj.gl.primitiva,"esfera")) {
					glutSolidSphere(objeto->obj.gl.tamano,50,50);
				}
			}
			
		glPopMatrix();
	}
}



void display () {
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.9f ,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0,0.0,200.0,0.0,0.0,0.0,0.0,1.0,0.0);	
	light_config();
	glRotatef(camera_x,1.0,0.0,0.0);
	glRotatef(camera_y,0.0,1.0,0.0);

	draw_floor();
	draw_game_objects();
	
	
	glRotatef(0.0,camera_x,camera_y,0.0);
	// paint
	glutSwapBuffers();
}

static void play(int value) {
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int time_since_previous_frame = current_time - previous_time;
	int elapsed_time = current_time - start_time;
	
	// Set up the next timer tick (do this first)
    glutTimerFunc(timer, play, 0);
	
	Punto *temp = (Punto *) malloc(sizeof(Punto));
	float dx1, dy1, d1;
	float dx2, dy2, d2;
	 
	temp->x = posicion_jugador->x + movimiento_jugador->velocidad_x * 1;
	temp->y = posicion_jugador->y + movimiento_jugador->velocidad_y * 1;
	
	dx1 = movimiento_jugador->punto_proximo->x - posicion_jugador->x;
	dy1 = movimiento_jugador->punto_proximo->y - posicion_jugador->y;
	d1 = dx1*dx1 + dy1*dy1;
	
	dx2 = temp->x - posicion_jugador->x;
	dy2 = temp->y - posicion_jugador->y;
	d2 = dx2*dx2 + dy2*dy2;
	 
	if (d1 >= d2) {
		posicion_jugador->x = temp->x;
		posicion_jugador->y = temp->y;
	} else {
		
		movimiento_jugador->punto_actual = movimiento_jugador->punto_proximo;
		
		if (movimiento_jugador->actual == movimiento_jugador->cantidad) {
			movimiento_jugador->punto_proximo = game->niveles->jugador.trayectoria.puntos;
			movimiento_jugador->actual = 1;

		}
		else {
			movimiento_jugador->punto_proximo++;
			movimiento_jugador->actual++;
		}
		movimiento_jugador->dx = movimiento_jugador->punto_proximo->x - movimiento_jugador->punto_actual->x;
		movimiento_jugador->dy = movimiento_jugador->punto_proximo->y - movimiento_jugador->punto_actual->y;
		movimiento_jugador->d = sqrt(movimiento_jugador->dx * movimiento_jugador->dx + movimiento_jugador->dy * movimiento_jugador->dy); 
		movimiento_jugador->velocidad_x = movimiento_jugador->dx / movimiento_jugador->d;
		movimiento_jugador->velocidad_y = movimiento_jugador->dy / movimiento_jugador->d;
	}
	
	free(temp);
	
	for (int i = 0; i < game->niveles->cantidad_contrincantes; i++) {
		temp = (Punto *) malloc(sizeof(Punto));
		temp->x = posicion_contrincantes[i].x + movimiento_contrincantes[i].velocidad_x * 1;
		temp->y = posicion_contrincantes[i].y + movimiento_contrincantes[i].velocidad_y * 1;

		dx1 = movimiento_contrincantes[i].punto_proximo->x - posicion_contrincantes[i].x;
		dy1 = movimiento_contrincantes[i].punto_proximo->y - posicion_contrincantes[i].y;
		d1 = dx1*dx1 + dy1*dy1;

		dx2 = temp->x - posicion_contrincantes[i].x;
		dy2 = temp->y - posicion_contrincantes[i].y;
		d2 = dx2*dx2 + dy2*dy2;

		if (d1 >= d2) {
			posicion_contrincantes[i].x = temp->x;
			posicion_contrincantes[i].y = temp->y;
		} else {

			movimiento_contrincantes[i].punto_actual = movimiento_contrincantes[i].punto_proximo;

			if (movimiento_contrincantes[i].actual == movimiento_contrincantes[i].cantidad) {
				movimiento_contrincantes[i].punto_proximo = game->niveles->contrincantes[i].trayectoria.puntos;
				movimiento_contrincantes[i].actual = 1;

			}
			else {
				movimiento_contrincantes[i].punto_proximo++;
				movimiento_contrincantes[i].actual++;
			}
			movimiento_contrincantes[i].dx = movimiento_contrincantes[i].punto_proximo->x - movimiento_contrincantes[i].punto_actual->x;
			movimiento_contrincantes[i].dy = movimiento_contrincantes[i].punto_proximo->y - movimiento_contrincantes[i].punto_actual->y;
			movimiento_contrincantes[i].d = sqrt(movimiento_contrincantes[i].dx * movimiento_contrincantes[i].dx
			 	+ movimiento_contrincantes[i].dy * movimiento_contrincantes[i].dy); 
			movimiento_contrincantes[i].velocidad_x = movimiento_contrincantes[i].dx / movimiento_contrincantes[i].d;
			movimiento_contrincantes[i].velocidad_y = movimiento_contrincantes[i].dy / movimiento_contrincantes[i].d;
		}
	
		free(temp);
	}

	
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
			camera_x -= 5.0;
			break;
		case GLUT_KEY_UP:
			camera_x += 5.0;
			break;
		case GLUT_KEY_LEFT:
			camera_y -= 5.0;
			break;
		case GLUT_KEY_RIGHT:
			camera_y += 5.0;
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



Game * load_game () {
 	// int i;
 	//     string input_xml;
 	//     string linea;
	Game *g = (Game *) malloc(sizeof(Game));

	//     ifstream archivo("config.xml");
	//     while(getline(archivo,linea))
	//         input_xml += linea;
	// 
	//     vector<char> buffer(input_xml.begin(), input_xml.end());
	//     buffer.push_back('\0');
	//     rapidxml::xml_document<> doc;
	//     doc.parse<parse_declaration_node | parse_no_data_nodes>(&buffer[0]);
	// 
	//     xml_node<> *nodo = doc.first_node("proyecto");
	//     int niveles  = atoi(nodo->first_node("niveles")->value());
	//     cout << "Niveles: " << niveles << endl; 
	// 
	// g->niveles = (Nivel *) malloc(sizeof(Nivel) * niveles);
	// 
	// for (i = 0; i < niveles-1; i++) {
	//     	// for(xml_node<> *level = nodo->first_node("nivel"); level ; level = level->next_sibling()){
	// 		xml_node<> *level = nodo->first_node("nivel");
	// 		xml_node<> *level_attr = level->first_node("id");
	// 		game->niveles->id = (char *) malloc(sizeof((char *)level_attr->value()));
	//             cout << level_attr->value() << endl;
	// }
	
	Punto *punto;
	Jugador *contrincante;
	Objeto * objeto;
	
	g->niveles = (Nivel *) malloc(sizeof(Nivel) * 2);
	g->cantidad_niveles = 2;
	g->niveles->id = (char *) "uno";
	g->niveles->tiempo_juego = 360;
	g->niveles->cantidad_contrincantes = 2;
	
	g->niveles->jugador.disparo = 1.0;
	g->niveles->jugador.trayectoria.velocidad = 1;
	g->niveles->jugador.cantidad_puntos = 5;
	g->niveles->jugador.trayectoria.puntos = (Punto *) malloc(sizeof(Punto) * 5);
	
	punto = g->niveles->jugador.trayectoria.puntos;

	posicion_jugador->x = 20;
	posicion_jugador->y = 20;

	*punto++ = (Punto) {20, 20};
	// punto++;
	*punto++ = (Punto) {20, 40};
	// punto++;
	*punto++ = (Punto) {15, 60};
	// punto++;
	*punto++ = (Punto) {10, 80};
	// punto++;
	*punto++ = (Punto) {10, 40};
		
	movimiento_jugador = (Movimiento *) malloc(sizeof(Movimiento));
	movimiento_jugador->actual = 2;
	movimiento_jugador->cantidad = 5;
	movimiento_jugador->punto_actual = g->niveles->jugador.trayectoria.puntos;
	movimiento_jugador->punto_proximo = g->niveles->jugador.trayectoria.puntos + 1;
	movimiento_jugador->dx = movimiento_jugador->punto_proximo->x - movimiento_jugador->punto_actual->x;
	movimiento_jugador->dy = movimiento_jugador->punto_proximo->y - movimiento_jugador->punto_actual->y;
	movimiento_jugador->d = sqrt(movimiento_jugador->dx * movimiento_jugador->dx + movimiento_jugador->dy * movimiento_jugador->dy); 
	movimiento_jugador->velocidad_x = movimiento_jugador->dx / movimiento_jugador->d;
	movimiento_jugador->velocidad_y = movimiento_jugador->dy / movimiento_jugador->d;
	
	movimiento_contrincantes = (Movimiento *) malloc(sizeof(Movimiento) * g->niveles->cantidad_contrincantes);
	posicion_contrincantes = (Punto *) malloc(sizeof(Punto) * g->niveles->cantidad_contrincantes);
	Movimiento *mc = movimiento_contrincantes;
	Punto *pc = posicion_contrincantes;
	// cout << game->niveles->jugador.disparo << endl;
	
	g->niveles->cantidad_contrincantes = 2;
	g->niveles->contrincantes = (Jugador *) malloc(sizeof(Jugador) * 2);
	
	contrincante = g->niveles->contrincantes;
	
	(*contrincante).disparo = 0.9;
	(*contrincante).trayectoria.velocidad = 0.01;
	(*contrincante).cantidad_puntos = 2;
	(*contrincante).trayectoria.puntos = (Punto *) malloc(sizeof(Punto) * 2);
	
	punto = (*contrincante).trayectoria.puntos;
	
	*punto = (Punto) {40, 20};
		// *punto = (Punto) {25, 5};
	punto++;
	*punto = (Punto) {80, 20};
		// *punto = (Punto) {25, 15};
	
	mc->actual = 2;
	mc->cantidad = 2;
	mc->punto_actual = (*contrincante).trayectoria.puntos;
	mc->punto_proximo = (*contrincante).trayectoria.puntos + 1;
	movimiento_contrincantes[0].dx = movimiento_contrincantes[0].punto_proximo->x - movimiento_contrincantes[0].punto_actual->x;
	movimiento_contrincantes[0].dy = movimiento_contrincantes[0].punto_proximo->y - movimiento_contrincantes[0].punto_actual->y;
	movimiento_contrincantes[0].d = sqrt(movimiento_contrincantes[0].dx * movimiento_contrincantes[0].dx + movimiento_contrincantes[0].dy * movimiento_contrincantes[0].dy); 
	movimiento_contrincantes[0].velocidad_x = movimiento_contrincantes[0].dx / movimiento_contrincantes[0].d;
	movimiento_contrincantes[0].velocidad_y = movimiento_contrincantes[0].dy / movimiento_contrincantes[0].d;

	
	pc->x = 40;
	pc->y = 20;
	// pc->x = 25;
	// pc->y = 5;
	
	contrincante++;
	mc++;
	pc++;
	
	(*contrincante).disparo = 0.95;
	(*contrincante).trayectoria.velocidad = 0.5;
	(*contrincante).cantidad_puntos = 1;
	(*contrincante).trayectoria.puntos = (Punto *) malloc(sizeof(Punto) * 1);
	
	punto = (*contrincante).trayectoria.puntos;
	
	*punto = (Punto) {80, 50};
	
	mc->actual = 1;
	mc->cantidad = 1;
	mc->punto_actual = (*contrincante).trayectoria.puntos;
	mc->punto_proximo = (*contrincante).trayectoria.puntos;
	movimiento_contrincantes[1].dx = movimiento_contrincantes[1].punto_proximo->x - movimiento_contrincantes[1].punto_actual->x;
	movimiento_contrincantes[1].dy = movimiento_contrincantes[1].punto_proximo->y - movimiento_contrincantes[1].punto_actual->y;
	movimiento_contrincantes[1].d = sqrt(movimiento_contrincantes[1].dx * movimiento_contrincantes[1].dx + movimiento_contrincantes[1].dy * movimiento_contrincantes[1].dy); 
	movimiento_contrincantes[1].velocidad_x = movimiento_contrincantes[1].dx / movimiento_contrincantes[1].d;
	movimiento_contrincantes[1].velocidad_y = movimiento_contrincantes[1].dy / movimiento_contrincantes[1].d;
	
	pc->x = 80;
	pc->y = 50;
	
	g->niveles->cantidad_objetos = 2;
	g->niveles->objetos = (Objeto *) malloc(sizeof(Objeto) * 2);
	
	objeto = g->niveles->objetos;
	
	objeto->x = 50;
	objeto->y = 50;
	
	objeto->tipo = 1;
	objeto->obj.archivo = (char *) malloc(sizeof("modelos/bunny.ply"));
	objeto->obj.archivo = "modelos/bunny.ply";
	
	// objeto->obj.gl.tamano = 5;
	// objeto->obj.gl.primitiva = (char *) malloc(sizeof("esfera"));
	// objeto->obj.gl.primitiva = (char *) "esfera";
	
	objeto++;
	
	objeto->tipo = 2;
	objeto->x = 20;
	objeto->y = 5;
	// &objeto->obj.gl.tamano = (int) malloc(sizeof(int));
	objeto->obj.gl.tamano = 5;
	objeto->obj.gl.primitiva = (char *) malloc(sizeof("cubo"));
	objeto->obj.gl.primitiva = (char *) "cubo";
	
	
	return g;
	
	// free(g->niveles);
	// free(game);
}


int main (int argc, char *argv[]) {
	// window settings
	glutInit(&argc, argv);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(20,20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Play");
	
	// lights
	// light_config();
	// setup_lighting(1);
	
	// hooks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutSpecialFunc(key_special);
	glutPassiveMotionFunc(mouse);
	
	
	// glEnable(GL_BLEND); //Enable alpha blending
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function

	importTrimesh();
	
	// traverse_xml("config.xml");
	game = load_game();

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

