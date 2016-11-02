/* definiciones básicas para usar la funcion de Mandelbrot */
#define MAX_X 4096 
#define MAX_Y 4096

extern float Limit , XRMin , XRMax , YRMin , YRMax;
extern unsigned int SX,SY;
extern unsigned short n_col;

unsigned short int Mandel(unsigned x,unsigned y);
