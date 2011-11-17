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

			if (objeto->tipo == 1) {
				glPushMatrix();
				glTranslatef(-50.0f,-45.0f,50.0f);
				glTranslatef(objeto->x, 0.0f, - objeto->y);
				glColor4f(1.0,0.0,0.0,1.0);
				glPushMatrix();
					glScalef(10.0f,20.0f,20.0f);
					setup_lighting(2);
					draw_mesh(2);
				glPopMatrix();
				glPopMatrix();
			}
			else {

				glPushMatrix();
					glTranslatef(-50.0f,-47.5f,50.0f);
					glTranslatef(objeto->x, 0.0f, - objeto->y);
					glColor4f(1.0,1.0,0.0,1.0);
				
				if (strcmp(objeto->obj.gl.primitiva,"cubo")) {
					glutSolidCube(objeto->obj.gl.tamano);
				}
				else if (strcmp(objeto->obj.gl.primitiva,"esfera")) {
					glutSolidSphere(objeto->obj.gl.tamano,50,50);
				}
				glPopMatrix();
			}
			
		
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



Game * load_game (char * file) {
		// int i;
	    string input_xml;
	    string linea;
		Game *g = (Game *) malloc(sizeof(Game));
		Punto *punto;
		Jugador *contrincante;
		Objeto * objeto;
		movimiento_contrincantes = (Movimiento *) malloc(sizeof(Movimiento) * g->niveles->cantidad_contrincantes);
		posicion_contrincantes = (Punto *) malloc(sizeof(Punto) * g->niveles->cantidad_contrincantes);


		ifstream archivo(file);
	    while(getline(archivo,linea))
	        input_xml += linea;

	    vector<char> buffer(input_xml.begin(), input_xml.end());
	    buffer.push_back('\0');
	    rapidxml::xml_document<> doc;
	    doc.parse<parse_declaration_node | parse_no_data_nodes>(&buffer[0]);

	    xml_node<> *nodo = doc.first_node("proyecto");

		g->niveles = (Nivel *) malloc(sizeof(Nivel) * 2);
		g->cantidad_niveles = atoi(nodo->first_node("niveles")->value());

	    nodo = nodo->first_node("nivel");  

	    // nivel
	    for(int i = 0; i < g->cantidad_niveles ; i++, nodo = nodo->next_sibling("nivel")){
			// datos nivel
	        g->niveles->id = (char *) nodo->first_node("id")->value();
	        g->niveles->tiempo_juego = atoi(nodo->first_node("tiempo_juego")->value());
	
			// datos jugador
	        g->niveles->jugador.cantidad_puntos = atoi(
				nodo->first_node("jugador")->first_node("trayectoria")->first_node("puntos")->value());
	        g->niveles->jugador.disparo = atof(
				nodo->first_node("jugador")->first_node("disparo")->value());
	        g->niveles->jugador.trayectoria.velocidad = atof(
				nodo->first_node("jugador")->first_node("trayectoria")->first_node("velocidad")->value());

	        // trayectoria del jugador
	        xml_node<> *nodopuntos ; 
	        nodopuntos = nodo->first_node("jugador")->first_node("trayectoria")->first_node("punto");
			g->niveles->jugador.trayectoria.puntos = (Punto *) malloc(sizeof(Punto) * 5);
			
	        for(int j = 0; j < g->niveles->jugador.cantidad_puntos && nodopuntos ; j++){
	            g->niveles->jugador.trayectoria.puntos[j].x = atof(nodopuntos->first_node("x")->value());
	            g->niveles->jugador.trayectoria.puntos[j].y = atof(nodopuntos->first_node("y")->value());
	            nodopuntos = nodopuntos->next_sibling("punto");
				if (j == 0) {
					posicion_jugador->x = g->niveles->jugador.trayectoria.puntos[j].x;
					posicion_jugador->y = g->niveles->jugador.trayectoria.puntos[j].y;
				}
	        }  
	
			movimiento_jugador = (Movimiento *) malloc(sizeof(Movimiento));
			movimiento_jugador->actual = 2;
			movimiento_jugador->cantidad = g->niveles->jugador.cantidad_puntos;
			movimiento_jugador->punto_actual = g->niveles->jugador.trayectoria.puntos;
			movimiento_jugador->punto_proximo = g->niveles->jugador.trayectoria.puntos + 1;
			movimiento_jugador->dx = movimiento_jugador->punto_proximo->x - movimiento_jugador->punto_actual->x;
			movimiento_jugador->dy = movimiento_jugador->punto_proximo->y - movimiento_jugador->punto_actual->y;
			movimiento_jugador->d = sqrt(movimiento_jugador->dx * movimiento_jugador->dx + movimiento_jugador->dy * movimiento_jugador->dy); 
			movimiento_jugador->velocidad_x = movimiento_jugador->dx / movimiento_jugador->d;
			movimiento_jugador->velocidad_y = movimiento_jugador->dy / movimiento_jugador->d;


	        // contrincantes
	        xml_node<> *nodocon;
	        nodocon = nodo->first_node("contrincantes");
			g->niveles->cantidad_contrincantes = atoi(nodocon->value());
			
	        nodocon = nodo->first_node("contrincante");

	        for(int k = 0; k < g->niveles->cantidad_contrincantes ; k ++ , nodocon = nodocon->next_sibling("contrincante")){
				g->niveles->contrincantes = (Jugador *) malloc(sizeof(Jugador) * 2);
				
	            g->niveles->contrincantes[k].cantidad_puntos = atoi(
					nodocon->first_node("trayectoria")->first_node("puntos")->value());

	            g->niveles->contrincantes[k].disparo = atof(nodocon->first_node("disparo")->value());
	            g->niveles->contrincantes[k].trayectoria.velocidad = atof(
					nodocon->first_node("trayectoria")->first_node("velocidad")->value());

	            nodopuntos = nodocon->first_node("trayectoria")->first_node("punto");
				
				g->niveles->contrincantes[k].trayectoria.puntos = (Punto *) malloc(sizeof(Punto) *
				 	g->niveles->contrincantes[k].cantidad_puntos);
	            // trayectoria
	            for(int l = 0; l < g->niveles->contrincantes[k].cantidad_puntos && nodopuntos ; l++){
	                g->niveles->contrincantes[k].trayectoria.puntos[l].x = atof(nodopuntos->first_node("x")->value());
	                g->niveles->contrincantes[k].trayectoria.puntos[l].y = atof(nodopuntos->first_node("y")->value());
					if (l == 0) {
						posicion_contrincantes[k].x = g->niveles->contrincantes[k].trayectoria.puntos[l].x;
						posicion_contrincantes[k].y = g->niveles->contrincantes[k].trayectoria.puntos[l].y;;
					}
	                nodopuntos = nodopuntos->next_sibling("punto");
	            }
	
				movimiento_contrincantes[k].actual = 2;
				movimiento_contrincantes[k].cantidad = g->niveles->contrincantes[k].cantidad_puntos;
				movimiento_contrincantes[k].punto_actual =  g->niveles->contrincantes[k].trayectoria.puntos;
				movimiento_contrincantes[k].punto_proximo = g->niveles->contrincantes[k].trayectoria.puntos + 1;
				movimiento_contrincantes[k].dx = movimiento_contrincantes[k].punto_proximo->x - movimiento_contrincantes[k].punto_actual->x;
				movimiento_contrincantes[k].dy = movimiento_contrincantes[k].punto_proximo->y - movimiento_contrincantes[k].punto_actual->y;
				movimiento_contrincantes[k].d = sqrt(movimiento_contrincantes[k].dx * movimiento_contrincantes[k].dx + 
					movimiento_contrincantes[k].dy * movimiento_contrincantes[k].dy); 
				movimiento_contrincantes[k].velocidad_x = movimiento_contrincantes[k].dx / movimiento_contrincantes[k].d;
				movimiento_contrincantes[k].velocidad_y = movimiento_contrincantes[k].dy / movimiento_contrincantes[k].d;

	        }

	        // objetos
	        xml_node<> *objetos ;
	        objetos = nodo->first_node("objetos");
			
			g->niveles->cantidad_objetos = atoi(objetos->first_node("cantidad")->value());
			g->niveles->objetos = (Objeto *) malloc(sizeof(Objeto) * g->niveles->cantidad_objetos);

	        objetos = objetos->first_node("objeto");

	        for(int j = 0; j < g->niveles->cantidad_objetos ; j++ , objetos = objetos->next_sibling("objeto")){

	            string tipo = objetos->first_node()->name();

	            if (!tipo.compare("maya")) {
					g->niveles->objetos[j].tipo = 1;
					g->niveles->objetos[j].x = atof(objetos->first_node()->first_node("x")->value());
					g->niveles->objetos[j].y = atof(objetos->first_node()->first_node("y")->value());
	            } 
				else if (!tipo.compare("cubo")) {
					g->niveles->objetos[j].tipo = 2;
					g->niveles->objetos[j].x = atof(objetos->first_node()->first_node("x")->value()); 
					g->niveles->objetos[j].y = atof(objetos->first_node()->first_node("y")->value());
					g->niveles->objetos[j].obj.gl.tamano = 5;
					g->niveles->objetos[j].obj.gl.primitiva = (char *) malloc(sizeof("cubo"));
					g->niveles->objetos[j].obj.gl.primitiva = (char *) "cubo";
	            }
				else {
					g->niveles->objetos[j].tipo = 2;
					g->niveles->objetos[j].x = atof(objetos->first_node()->first_node("x")->value()); 
					g->niveles->objetos[j].y = atof(objetos->first_node()->first_node("y")->value());
					g->niveles->objetos[j].obj.gl.tamano = 5;
					g->niveles->objetos[j].obj.gl.primitiva = (char *) malloc(sizeof("esfera"));
					g->niveles->objetos[j].obj.gl.primitiva = (char *) "esfera";
	          	}
			}
	    }
	
	return g;
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
	game = load_game(argv[1]);

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

