typedef struct {
	float x;
	float y;
} Punto;

typedef struct {
	int actual;
	int cantidad;
	Punto *punto_actual;
	Punto *punto_proximo;
	float dx, dy, d;
	float velocidad_x, velocidad_y;
	
} Movimiento;

typedef struct {
	int velocidad;
	Punto *puntos;
} Trayectoria;

typedef struct {
	float disparo;
	int cantidad_puntos;
	Trayectoria trayectoria;
	int impactos_restantes;
	Movimiento movimiento;
	Punto *posicion;
} Jugador;

typedef struct {
	int x;
	int y;
	int tipo; // 1 maya, 2 gl
	union {
		char *archivo;
		struct {
			int tamano;
			char *primitiva;
		} gl;
	} obj;
	int impactos_restantes;
} Objeto;

typedef struct {
	char * id;
	int tiempo_juego;
	Jugador jugador;
	int cantidad_contrincantes;
	Jugador *contrincantes;	
	int cantidad_objetos;
	Objeto *objetos;
} Nivel;

typedef struct {
	int cantidad_niveles;
	Nivel *niveles;
} Game;

