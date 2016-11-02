#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/timeb.h>
#include "mandel.h"

#define CLAMP(x) (((x) > 255) ? 255 : ((x) < 0) ? 0 : (x))

// Para toda la imagen descomentar la línea siguente
//float Limit = 4.0, XRMin=-2.0, XRMax=1.0, YRMin= -1.3, YRMax=1.2;

// Para obtener los datos de la práctica usar los valores:
float Limit = 4.0, XRMin=-1.2, XRMax=-0.5, YRMin=0.01 , YRMax=0.7; //definicion de la zona de calculo

//unsigned int SX=1024,SY=768;   //Cuantos datos se van a calcular (puntos en el plano)--> RESOLUCION
unsigned int SX=1024,SY=1024;   //Cuantos datos se van a calcular (puntos en el plano)--> RESOLUCION
unsigned short int pantalla [MAX_X][MAX_Y];  //MAX_X: Lineas horizontales; MAX_Y componentes cada una; Estan definidas en mandel.h

unsigned short n_col = 1024;  // numero de vuelvas a dar


// variables utilizadas para medir el tiempo
struct timeval a_t,b_t;

int main(int argc,char* argv[],char **envp){
  
  
  unsigned char rgb[3];
   double rgb_f[3];
  
  printf("La direcciónde main es: %p\n",main); //en consola
  
  unsigned int x,y;
  FILE *pf;
  pf=fopen("/home/juando/pvm3/mandel.params","r");
  fscanf(pf,"%u%u%hu%f%f%f%f%f",&SX,&SY,&n_col,&Limit,&XRMin,&XRMax,&YRMin,&YRMax);
  printf("SX:%u; SY:%u;n_col:%hu;\nLimit:%g; XRmin:%g; XRmax:%g; YRmin:%g; YRMax:%g\n",SX,SY,n_col,Limit,XRMin,XRMax,YRMin,YRMax);
  fclose(pf);
  /*switch(argc){
	case 4:
	     n_col=atoi(argv[3]);
	case 3:
	     SY=(unsigned int)atoi(argv[2]); //convert string to integer
	     SX=(unsigned int)atoi(argv[1]);
 	     break;
	default:
  	   printf("utilización: Mandel SX SY [n_col]\n");
	   printf("USando configuración por defecto\n");
  }*/
  //****************crea archivo ppm-->paralelo_SX_SY.ppm***********************
  FILE *fp;
  char cadena[25];                 
  snprintf(cadena, sizeof(cadena),"secuen_%u_%u.ppm", SX, SY);
  fp = fopen(cadena, "w");  // se crea en carpeta LINUX64
  fprintf(fp, "P6\n%d %d\n255\n", SX, SY); // ppm header
  //***************************************************************************
  printf("configuración y tiempo:\n%d\t%d\t%d\t",SX,SY,n_col);
  
  gettimeofday(&a_t,NULL);
  
  for(y=0;y<SY;y++){
    for(x=0;x<SX;x++){
      /* en c: fila,columna --> (y,x) */
      pantalla[y][x]=Mandel(x,y);
      
      if(pantalla[y][x]==n_col){   //Pintamos de BLANCO
	rgb_f[0]=1;rgb_f[1]=1;rgb_f[2]=1;
	
      }
      else if(pantalla[y][x]<=1){                       //Negro
	rgb_f[0]=0;rgb_f[1]=0;rgb_f[2]=0;
	 
      }else{
	rgb_f[0]=((pantalla[y][x])%7+1)/8.0f;
	rgb_f[1]=((pantalla[y][x]+6)%7+1)/8.0f;
	rgb_f[2]=0.1 + (((float)pantalla[y][x])/((float)n_col));
      }
      rgb[0] = CLAMP(rgb_f[0]*255);
      rgb[1] = CLAMP(rgb_f[1]*255);
      rgb[2] = CLAMP(rgb_f[2]*255);
      fwrite(rgb, 3, 1, fp);
    }
  }
 fclose(fp);
 
 gettimeofday(&b_t,NULL);
    {
      double intervalo=1.0;
      intervalo=(double)(b_t.tv_sec-a_t.tv_sec);
      intervalo= intervalo + (b_t.tv_usec-a_t.tv_usec) / 1000000.0;
      
      printf("\nTIEMPO DE EJECUCIÓn : %f\n",intervalo);
      
    }
 exit(1);
}
