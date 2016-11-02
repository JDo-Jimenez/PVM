/* definiciones básicas para usar la funcion de Mandelbrot */

/* Dimensiones maximas de la pantalla */

#define MAX_FILAS 4096
#define MAX_COL   4096
#define MAX_HIJOS 256


/* definicion de variables globales que determinan al problema */
extern float Limit , XRMin , XRMax , YRMin , YRMax;

extern unsigned int SX,SY;   /* tamaño actual del problema */
extern unsigned short n_col; /* nº de vueltas del algoritmo Mandelbrot*/


/* Funcion que calcula el valor de Mandelbrot para un punto */
/* utiliza las constantes definidas anteriormente           */
unsigned short Mandel(unsigned x,unsigned y,unsigned short n_col);



/* Constantes utilizadas en la comunicacion enntre tareas PVM*/
#define PARAMETROS_MSG 1
#define SIG_TAREA_MSG  2
#define RESULTADOS_MSG 3
#define SALIDA_MSG     4
#define DIBUJA_MSG     5  
#define ID_WORKER_MSG  6  


#define GRUPO_TRABAJO "TRABAJADORES MANDEL"
