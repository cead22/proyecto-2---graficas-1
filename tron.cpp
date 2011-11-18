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
static int timer = 1;

// ball
static float ball_x = 80.0;
static float ball_y = 80.0;
// static float ball_z = 0.0;

static float ball_speed_x = 15*0.0;
static float ball_speed_y = 15*0.2;
// static float ball_speed_z = 5*0.15;

static int ball_direction_x = 1;
static int ball_direction_y = 1;

static float camera_x = 0.0;
static float camera_y = 0.0;


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
	
		// contrincantes paths
		for(i = 0; i < game->niveles->cantidad_contrincantes; i++, contrincante++) {
			glBegin(GL_LINE_LOOP);
				for (j = 0, punto = contrincante->trayectoria.puntos; j < contrincante->cantidad_puntos; punto++, j++)
					glVertex3f(punto->x,0.0,-punto->y);
			glEnd();
		}
		
		// jugador path
			glBegin(GL_LINE_LOOP);
				for (j = 0, punto = jugador->trayectoria.puntos; j < jugador->cantidad_puntos; punto++, j++)
					glVertex3f(punto->x,0.0,-punto->y);
			glEnd();
			
		glPopMatrix();
			
		// jugador
		if (game->niveles->jugador.impactos_restantes > 0) {
			// cout << game->niveles->jugador.impactos_restantes << endl;
			glPushMatrix();
				glTranslatef(-50.0f,-46.0f,	50.0f);
				glTranslatef(game->niveles->jugador.posicion->x,0.0,- game->niveles->jugador.posicion->y);
				glScalef(0.07f,0.07f,0.07f);
				setup_lighting(1);
				draw_mesh(1);
			glPopMatrix();
		}
		
		// contrincantes
	
		for (k = 0; k < game->niveles->cantidad_contrincantes; k++) {
			// if (game->niveles->contrincantes[k].impactos_restantes > 0) {
			
				glPushMatrix();
					glTranslatef(-50.0f,-46.0f,	50.0f);
					glTranslatef(game->niveles->contrincantes[k].posicion->x,0.0,- game->niveles->contrincantes[k].posicion->y);
					glScalef(0.07f,0.07f,0.07f);
					setup_lighting(1);
					draw_mesh(1);
				glPopMatrix();
			// }
		}
		
		// objetos
		for(i = 0; i < game->niveles->cantidad_objetos; i++) {
			// if (game->niveles->objetos[i].impactos_restantes > 0) {
				if (game->niveles->objetos[i].tipo == 1) {
					glPushMatrix();
					glTranslatef(-50.0f,-45.0f,50.0f);
					glTranslatef(game->niveles->objetos[i].x, 0.0f, - game->niveles->objetos[i].y);
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
						glTranslatef(game->niveles->objetos[i].x, 0.0f, - game->niveles->objetos[i].y);
						glColor4f(1.0,1.0,0.0,1.0);
	
					if (strcmp(game->niveles->objetos[i].obj.gl.primitiva,"cubo")) {
						glutSolidCube(game->niveles->objetos[i].obj.gl.tamano);
					}
					else if (strcmp(game->niveles->objetos[i].obj.gl.primitiva,"esfera")) {
						glutSolidSphere(game->niveles->objetos[i].obj.gl.tamano/2,50,50);
					}
					glPopMatrix();
				}
				
			// }
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
	
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){1.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){1.0,0.0,0.0,1.0});
		glColor4f(1.0,0.0,0.0,1.0);
		glTranslatef(-50.0f,-49.0f,50.0f);
		glTranslatef(ball_x,0,-ball_y);
		glutSolidSphere(1.0,20,20);
	glPopMatrix();
	
	
	// glRotatef(0.0,camera_x,camera_y,0.0);
	// paint
	glutSwapBuffers();
}

void collision_object (float block_left_bound, float block_right_bound,
						float block_top_bound, float block_bottom_bound, int &impactos_restantes) {
	if (ball_x >= block_left_bound - 1	
		&&	ball_x <= block_right_bound + 1)
	{
			// top
			if (abs(ball_y - block_top_bound) < 1 && ball_direction_y < 0) {
				ball_direction_y = 1;
				impactos_restantes--;
			}
			// bottom
			if (abs(ball_y - block_bottom_bound) < 1 && ball_direction_y > 0) {
				ball_direction_y = -1;
				impactos_restantes--;
			}
	}
	// collision with one of the sides
	if (ball_y >= block_bottom_bound - 1
		&&	ball_y <= block_top_bound + 1)
	{
		if( abs(ball_x - block_right_bound) < 1 && ball_direction_x < 0) {
			ball_direction_x *= -1;
			impactos_restantes--;
		}
		if( abs(ball_x - block_left_bound) < 1 && ball_direction_x > 0) {
			ball_direction_x *= -1;
			impactos_restantes--;
		}
	}
}

static void play(int value) {
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int time_since_previous_frame = current_time - previous_time;
	int elapsed_time = current_time - start_time;
	Punto *temp;
	float dx1, dy1, d1;
	float dx2, dy2, d2;
	int nivel_completado = 1;
	
	// Set up the next timer tick (do this first)
    glutTimerFunc(timer, play, 0);
	
	// ha perdido
	if (game->niveles->jugador.impactos_restantes <= 0) {
		cout << "HA PERDIDO" << endl;
		exit(0);
	}
	
	for (int i = 0; i < game->niveles->cantidad_contrincantes && nivel_completado; i++) {
		if (game->niveles->contrincantes[i].impactos_restantes > 0) {
			nivel_completado = 0;
			break;
		}
	}
	
	if (nivel_completado) {
		cout << "NIVEL PASADO" << endl;
		if(--game->cantidad_niveles == 0) {
			// cout<<game->cantidad_niveles<<endl;
			cout << "HA GANADO" << endl;
			exit(0);
		}
		game->niveles++;
	}
	
	if (game->niveles->jugador.impactos_restantes > 0 ) {
		temp = (Punto *) malloc(sizeof(Punto));
	 
		temp->x = game->niveles->jugador.posicion->x + game->niveles->jugador.movimiento.velocidad_x * 1;
		temp->y = game->niveles->jugador.posicion->y + game->niveles->jugador.movimiento.velocidad_y * 1;
	
		dx1 = game->niveles->jugador.movimiento.punto_proximo->x - game->niveles->jugador.posicion->x;
		dy1 = game->niveles->jugador.movimiento.punto_proximo->y - game->niveles->jugador.posicion->y;
		d1 = dx1*dx1 + dy1*dy1;
	
		dx2 = temp->x - game->niveles->jugador.posicion->x;
		dy2 = temp->y - game->niveles->jugador.posicion->y;
		d2 = dx2*dx2 + dy2*dy2;
	 
		if (d1 >= d2) {
			game->niveles->jugador.posicion->x = temp->x;
			game->niveles->jugador.posicion->y = temp->y;
		} else {
		
			game->niveles->jugador.movimiento.punto_actual = game->niveles->jugador.movimiento.punto_proximo;
		
			if (game->niveles->jugador.movimiento.actual >= game->niveles->jugador.movimiento.cantidad) {
				game->niveles->jugador.movimiento.punto_proximo = game->niveles->jugador.trayectoria.puntos;
				game->niveles->jugador.movimiento.actual = 1;
	
			}
			else {
				game->niveles->jugador.movimiento.punto_proximo++;
				game->niveles->jugador.movimiento.actual++;
			}
			game->niveles->jugador.movimiento.dx = game->niveles->jugador.movimiento.punto_proximo->x - game->niveles->jugador.movimiento.punto_actual->x;
			game->niveles->jugador.movimiento.dy = game->niveles->jugador.movimiento.punto_proximo->y - game->niveles->jugador.movimiento.punto_actual->y;
			game->niveles->jugador.movimiento.d = sqrt(game->niveles->jugador.movimiento.dx * game->niveles->jugador.movimiento.dx +
			 	game->niveles->jugador.movimiento.dy * game->niveles->jugador.movimiento.dy); 
			game->niveles->jugador.movimiento.velocidad_x = game->niveles->jugador.movimiento.dx / game->niveles->jugador.movimiento.d;
			game->niveles->jugador.movimiento.velocidad_y = game->niveles->jugador.movimiento.dy / game->niveles->jugador.movimiento.d;
		}
	
		free(temp);
	}
	else {
		game->niveles->jugador.posicion->x = -20;
		game->niveles->jugador.posicion->y = -20;
	}
	
	for (int i = 0; i < game->niveles->cantidad_contrincantes; i++) {
		if (game->niveles->contrincantes[i].impactos_restantes > 0) {
				temp = (Punto *) malloc(sizeof(Punto));
				temp->x = game->niveles->contrincantes[i].posicion->x + game->niveles->contrincantes[i].movimiento.velocidad_x * 1;
				temp->y = game->niveles->contrincantes[i].posicion->y + game->niveles->contrincantes[i].movimiento.velocidad_y * 1;
		
				dx1 = game->niveles->contrincantes[i].movimiento.punto_proximo->x - game->niveles->contrincantes[i].posicion->x;
				dy1 = game->niveles->contrincantes[i].movimiento.punto_proximo->y - game->niveles->contrincantes[i].posicion->y;
				d1 = dx1*dx1 + dy1*dy1;
		
				dx2 = temp->x - game->niveles->contrincantes[i].posicion->x;
				dy2 = temp->y - game->niveles->contrincantes[i].posicion->y;
				d2 = dx2*dx2 + dy2*dy2;
		
				if (d1 >= d2) {
					game->niveles->contrincantes[i].posicion->x = temp->x;
					game->niveles->contrincantes[i].posicion->y = temp->y;
				} else {
		
					game->niveles->contrincantes[i].movimiento.punto_actual = game->niveles->contrincantes[i].movimiento.punto_proximo;
		
					if (game->niveles->contrincantes[i].movimiento.actual >= game->niveles->contrincantes[i].movimiento.cantidad) {
						game->niveles->contrincantes[i].movimiento.punto_proximo = game->niveles->contrincantes[i].trayectoria.puntos;
						game->niveles->contrincantes[i].movimiento.actual = 1;
		
					}
					else {
						game->niveles->contrincantes[i].movimiento.punto_proximo++;
						game->niveles->contrincantes[i].movimiento.actual++;
					}
					game->niveles->contrincantes[i].movimiento.dx = game->niveles->contrincantes[i].movimiento.punto_proximo->x -
					 	game->niveles->contrincantes[i].movimiento.punto_actual->x;
					game->niveles->contrincantes[i].movimiento.dy = game->niveles->contrincantes[i].movimiento.punto_proximo->y -
					 	game->niveles->contrincantes[i].movimiento.punto_actual->y;
					game->niveles->contrincantes[i].movimiento.d = sqrt(game->niveles->contrincantes[i].movimiento.dx * game->niveles->contrincantes[i].movimiento.dx
					 	+ game->niveles->contrincantes[i].movimiento.dy * game->niveles->contrincantes[i].movimiento.dy); 
					game->niveles->contrincantes[i].movimiento.velocidad_x = game->niveles->contrincantes[i].movimiento.dx / game->niveles->contrincantes[i].movimiento.d;
					game->niveles->contrincantes[i].movimiento.velocidad_y = game->niveles->contrincantes[i].movimiento.dy / game->niveles->contrincantes[i].movimiento.d;
		
					free(temp);
				}
			}
			else {
				game->niveles->contrincantes[i].posicion->x = -10;
				game->niveles->contrincantes[i].posicion->y = -10 + i*10;
			}
	}

	
	// collision ball-cube

	if (ball_x >= 100.0 || ball_x <= 0.0) {
	  // printf("collision x\n");
	  ball_direction_x *= -1;
	}
	if (ball_y >= 100.0 || ball_y <= 0.0) {
	  // printf("collision y\n");
	  ball_direction_y *= -1;
	}
	
	// colision con jugador
	float block_left_bound = game->niveles->jugador.posicion->x - 2.5,
		block_right_bound = game->niveles->jugador.posicion->x + 2.5,
		block_top_bound = game->niveles->jugador.posicion->y + 2.5,
		block_bottom_bound = game->niveles->jugador.posicion->y - 2.5;
	
	collision_object(block_left_bound,block_right_bound, 
		block_top_bound, block_bottom_bound, game->niveles->jugador.impactos_restantes);

	for (int i = 0; i < game->niveles->cantidad_contrincantes; i++) {
		block_left_bound =   game->niveles->contrincantes[i].posicion->x - 2.5;
		block_right_bound =  game->niveles->contrincantes[i].posicion->x + 2.5;
		block_top_bound =    game->niveles->contrincantes[i].posicion->y + 2.5;
		block_bottom_bound = game->niveles->contrincantes[i].posicion->y - 2.5;
		collision_object(block_left_bound,block_right_bound,
		 	block_top_bound, block_bottom_bound, game->niveles->contrincantes[i].impactos_restantes);
	}
	
	for (int i = 0; i < game->niveles->cantidad_objetos; i++) {
		block_left_bound = game->niveles->objetos[i].x - 2.5;
		block_right_bound = game->niveles->objetos[i].x + 2.5;
		block_top_bound = game->niveles->objetos[i].y + 2.5;
		block_bottom_bound = game->niveles->objetos[i].y - 2.5;
		collision_object(block_left_bound,block_right_bound,
		 	block_top_bound, block_bottom_bound, game->niveles->objetos[i].impactos_restantes);
	}

	
	ball_x += ball_direction_x * ball_speed_x;
	ball_y += ball_direction_y * ball_speed_y;

	
	
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


		ifstream archivo(file);
	    while(getline(archivo,linea))
	        input_xml += linea;

	    vector<char> buffer(input_xml.begin(), input_xml.end());
	    buffer.push_back('\0');
	    rapidxml::xml_document<> doc;
	    doc.parse<parse_declaration_node | parse_no_data_nodes>(&buffer[0]);

	    xml_node<> *nodo = doc.first_node("proyecto");

		g->cantidad_niveles = atoi(nodo->first_node("niveles")->value());
		g->niveles = (Nivel *) malloc(sizeof(Nivel) * g->cantidad_niveles) ;
		

	    nodo = nodo->first_node("nivel");  

	    // nivel
	    for(int i = 0; i < g->cantidad_niveles ; i++, nodo = nodo->next_sibling("nivel")){
			// datos nivel
	        g->niveles[i].id = (char *) nodo->first_node("id")->value();

	        g->niveles[i].tiempo_juego = atoi(nodo->first_node("tiempo_juego")->value());
	
			// datos jugador
	        g->niveles[i].jugador.cantidad_puntos = atoi(
				nodo->first_node("jugador")->first_node("trayectoria")->first_node("puntos")->value());
			g->niveles[i].jugador.impactos_restantes = 3;
	        g->niveles[i].jugador.disparo = atof(
				nodo->first_node("jugador")->first_node("disparo")->value());
	        g->niveles[i].jugador.trayectoria.velocidad = atof(
				nodo->first_node("jugador")->first_node("trayectoria")->first_node("velocidad")->value());

	        // trayectoria del jugador
	        xml_node<> *nodopuntos ; 
	        nodopuntos = nodo->first_node("jugador")->first_node("trayectoria")->first_node("punto");
			g->niveles[i].jugador.trayectoria.puntos = (Punto *) malloc(sizeof(Punto) * g->niveles[i].jugador.cantidad_puntos);
			g->niveles[i].jugador.posicion = (Punto * ) malloc(sizeof(Punto));
			
	        for(int j = 0; j < g->niveles[i].jugador.cantidad_puntos && nodopuntos ; j++){
	            g->niveles[i].jugador.trayectoria.puntos[j].x = atof(nodopuntos->first_node("x")->value());
	            g->niveles[i].jugador.trayectoria.puntos[j].y = atof(nodopuntos->first_node("y")->value());
				if (j == 0) {
					g->niveles[i].jugador.posicion->x = g->niveles[i].jugador.trayectoria.puntos[j].x;
					g->niveles[i].jugador.posicion->y = g->niveles[i].jugador.trayectoria.puntos[j].y;
				}
				nodopuntos = nodopuntos->next_sibling("punto");
	        }  
	
			// g->niveles->jugador.movimiento = (Movimiento *) malloc(sizeof(Movimiento));
			g->niveles[i].jugador.movimiento.punto_actual = (Punto *) malloc(sizeof(Punto));
			g->niveles[i].jugador.movimiento.punto_proximo = (Punto *) malloc(sizeof(Punto));
			
			
			g->niveles[i].jugador.movimiento.actual = 2;
			g->niveles[i].jugador.movimiento.cantidad = g->niveles[i].jugador.cantidad_puntos;
			g->niveles[i].jugador.movimiento.punto_actual = g->niveles[i].jugador.trayectoria.puntos;
			if (g->niveles[i].jugador.movimiento.cantidad > 1) {
				g->niveles[i].jugador.movimiento.punto_proximo = g->niveles[i].jugador.trayectoria.puntos + 1;
			}
			else {
				g->niveles[i].jugador.movimiento.punto_proximo = g->niveles[i].jugador.trayectoria.puntos;
			}
			g->niveles[i].jugador.movimiento.dx = g->niveles[i].jugador.movimiento.punto_proximo->x - g->niveles[i].jugador.movimiento.punto_actual->x;
			g->niveles[i].jugador.movimiento.dy = g->niveles[i].jugador.movimiento.punto_proximo->y - g->niveles[i].jugador.movimiento.punto_actual->y;
			g->niveles[i].jugador.movimiento.d = sqrt(g->niveles[i].jugador.movimiento.dx * g->niveles[i].jugador.movimiento.dx +
			 	g->niveles[i].jugador.movimiento.dy * g->niveles[i].jugador.movimiento.dy); 
			g->niveles[i].jugador.movimiento.velocidad_x = g->niveles[i].jugador.movimiento.dx / g->niveles[i].jugador.movimiento.d;
			g->niveles[i].jugador.movimiento.velocidad_y = g->niveles[i].jugador.movimiento.dy / g->niveles[i].jugador.movimiento.d;

	        // contrincantes
	        xml_node<> *nodocon;
	        nodocon = nodo->first_node("contrincantes");
			g->niveles[i].cantidad_contrincantes = atoi(nodocon->value());


			// movimiento_contrincantes = (Movimiento *) malloc(sizeof(Movimiento) * g->niveles[i].cantidad_contrincantes);
			// posicion_contrincantes = (Punto *) malloc(sizeof(Punto) * g->niveles[i].cantidad_contrincantes);
			
	        nodocon = nodo->first_node("contrincante");

				g->niveles[i].contrincantes = (Jugador *) malloc(sizeof(Jugador) * g->niveles[i].cantidad_contrincantes);
	        for (int k = 0; k < g->niveles[i].cantidad_contrincantes ; k ++ , nodocon = nodocon->next_sibling("contrincante")){
				
	            g->niveles[i].contrincantes[k].cantidad_puntos = atoi(
					nodocon->first_node("trayectoria")->first_node("puntos")->value());
				g->niveles[i].contrincantes[k].impactos_restantes = 1;
	            g->niveles[i].contrincantes[k].disparo = atof(nodocon->first_node("disparo")->value());
	            g->niveles[i].contrincantes[k].trayectoria.velocidad = atof(
					nodocon->first_node("trayectoria")->first_node("velocidad")->value());

	            nodopuntos = nodocon->first_node("trayectoria")->first_node("punto");
				
				g->niveles[i].contrincantes[k].trayectoria.puntos = (Punto *) malloc(sizeof(Punto) *
				 	g->niveles[i].contrincantes[k].cantidad_puntos);
				
				g->niveles[i].contrincantes[k].posicion = (Punto *) malloc(sizeof(Punto));
	            // trayectoria
	            for(int l = 0; l < g->niveles[i].contrincantes[k].cantidad_puntos && nodopuntos ; l++){

	                g->niveles[i].contrincantes[k].trayectoria.puntos[l].x = atof(nodopuntos->first_node("x")->value());
	                g->niveles[i].contrincantes[k].trayectoria.puntos[l].y = atof(nodopuntos->first_node("y")->value());
					if (l == 0) {
						g->niveles[i].contrincantes[k].posicion->x = g->niveles[i].contrincantes[k].trayectoria.puntos[l].x;
						g->niveles[i].contrincantes[k].posicion->y = g->niveles[i].contrincantes[k].trayectoria.puntos[l].y;;
					}
	                nodopuntos = nodopuntos->next_sibling("punto");
	            }
	
				g->niveles[i].contrincantes[k].movimiento.punto_actual = (Punto *) malloc(sizeof(Punto));
				g->niveles[i].contrincantes[k].movimiento.punto_proximo = (Punto *) malloc(sizeof(Punto));
				
				g->niveles[i].contrincantes[k].movimiento.actual = 2;
				g->niveles[i].contrincantes[k].movimiento.cantidad = g->niveles[i].contrincantes[k].cantidad_puntos;
				g->niveles[i].contrincantes[k].movimiento.punto_actual =  g->niveles[i].contrincantes[k].trayectoria.puntos;
				if (g->niveles[i].contrincantes[k].movimiento.cantidad > 1) {
					g->niveles[i].contrincantes[k].movimiento.punto_proximo = g->niveles[i].contrincantes[k].trayectoria.puntos + 1;
				}
				else {
					g->niveles[i].contrincantes[k].movimiento.punto_proximo = g->niveles[i].contrincantes[k].trayectoria.puntos;
				}
				g->niveles[i].contrincantes[k].movimiento.dx = g->niveles[i].contrincantes[k].movimiento.punto_proximo->x - g->niveles[i].contrincantes[k].movimiento.punto_actual->x;
				g->niveles[i].contrincantes[k].movimiento.dy = g->niveles[i].contrincantes[k].movimiento.punto_proximo->y - g->niveles[i].contrincantes[k].movimiento.punto_actual->y;
				g->niveles[i].contrincantes[k].movimiento.d = sqrt(g->niveles[i].contrincantes[k].movimiento.dx * g->niveles[i].contrincantes[k].movimiento.dx + 
					g->niveles[i].contrincantes[k].movimiento.dy * g->niveles[i].contrincantes[k].movimiento.dy); 
				g->niveles[i].contrincantes[k].movimiento.velocidad_x = g->niveles[i].contrincantes[k].movimiento.dx / g->niveles[i].contrincantes[k].movimiento.d;
				g->niveles[i].contrincantes[k].movimiento.velocidad_y = g->niveles[i].contrincantes[k].movimiento.dy / g->niveles[i].contrincantes[k].movimiento.d;

	        }

	        // objetos
	        xml_node<> *objetos ;
	        objetos = nodo->first_node("objetos");
			
			g->niveles[i].cantidad_objetos = atoi(objetos->first_node("cantidad")->value());
			g->niveles[i].objetos = (Objeto *) malloc(sizeof(Objeto) * g->niveles[i].cantidad_objetos);

	        objetos = objetos->first_node("objeto");

	        for(int j = 0; j < g->niveles[i].cantidad_objetos ; j++ , objetos = objetos->next_sibling("objeto")){

	            string tipo = objetos->first_node()->name();
			cout<<tipo<<endl;
	            if (!tipo.compare("maya")) {
					g->niveles[i].objetos[j].tipo = 1;
					g->niveles[i].objetos[j].x = atof(objetos->first_node()->first_node("x")->value());
					g->niveles[i].objetos[j].y = atof(objetos->first_node()->first_node("y")->value());
	            } 
				else if (!tipo.compare("cubo")) {
					g->niveles[i].objetos[j].tipo = 2;
					g->niveles[i].objetos[j].x = atof(objetos->first_node()->first_node("x")->value()); 
					g->niveles[i].objetos[j].y = atof(objetos->first_node()->first_node("y")->value());
					g->niveles[i].objetos[j].obj.gl.tamano = 5;
					g->niveles[i].objetos[j].obj.gl.primitiva = (char *) malloc(sizeof("cubo"));
					g->niveles[i].objetos[j].obj.gl.primitiva = (char *) "cubo";
	            }
				else {
					g->niveles[i].objetos[j].tipo = 2;
					g->niveles[i].objetos[j].x = atof(objetos->first_node()->first_node("x")->value()); 
					g->niveles[i].objetos[j].y = atof(objetos->first_node()->first_node("y")->value());
					g->niveles[i].objetos[j].obj.gl.tamano = 5;
					g->niveles[i].objetos[j].obj.gl.primitiva = (char *) malloc(sizeof("esfera"));
					g->niveles[i].objetos[j].obj.gl.primitiva = (char *) "esfera";
	          	}
				g->niveles[i].objetos[j].impactos_restantes = 5;
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
	// game->niveles++;
	
	// cout << game->niveles->id << endl;
	// exit(0);

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

