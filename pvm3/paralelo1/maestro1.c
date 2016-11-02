/*
 *   Arquitectura de Computadores y Sistemas Operativos II
 *
 *   Practica 3: PVM
 *
 *   Profesores:
 *     Miguel A. Mateo Pla
 *     Julio Pons Terol
 *
 *
 *  Descripcion
 *     Versión paralelo del algoritmo de calculo.
 *     Para su funcionamiento utiliza n tareas PVM de tipo trabajador.
 *
 */



// includes de la libreria PVM y gestion de tiempos
#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include "pvm3.h"
#include "mandel.h"

#define CLAMP(x) (((x) > 255) ? 255 : ((x) < 0) ? 0 : (x))
//

// Variables utilizadas para la definicion del problema
short unsigned int n_col; // Numero de estados de un punto

// Para toda la imagen descomentar la línea siguente
//float Limit = 4.0, XRMin=-2.0, XRMax=1.0, YRMin= -1.3, YRMax=1.2;

// Para obtener los datos de la práctica usar los valores:
//float Limit = 4.0, XRMin=-1.2, XRMax=-0.5, YRMin=0.01 , YRMax=0.7;
float Limit , XRMin, XRMax, YRMin, YRMax;
/* Almacenamiento de la pantalla */
unsigned short int pantalla[MAX_FILAS][MAX_COL];


/* Para saber cuando acabamos */
unsigned int recividas=0;

/* Para almacenar a los hijos */
int myTid,TaskId[MAX_HIJOS];
int numt;

int graficos = 0; //SIN SALIDA GRAFICA


// variables utilizadas para medir el tiempo
struct timeval a_t,b_t;


int main( int argc, char **argv ){
  unsigned int SX=0,SY=0;
  int numt2,i;
  //int dibujador;
  
  
  unsigned char rgb[3];
  double rgb_f[3];
  
     
  /* Configuración por defecto */ //La LEE del fichero mandel.params
  
  FILE *pf;
  pf=fopen("/home/juando/pvm3/mandel.params","r");
  fscanf(pf,"%u%u%hu%f%f%f%f%f%d",&SX,&SY,&n_col,&Limit,&XRMin,&XRMax,&YRMin,&YRMax,&numt);
  printf("numt:%d; SX:%u; SY:%u;n_col:%hu;\nLimit:%g; XRmin:%g; XRmax:%g; YRmin:%g; YRMax:%g\n",numt,SX,SY,n_col,Limit,XRMin,XRMax,YRMin,YRMax);
  fclose(pf);
  //n_col=2048;
  //SX=1024;
  //SY=768;
  
  
  /*switch(argc){
    case 1:
      break;
    case 2:
      numt=atoi(argv[1]);
      break;
    case 3:
      SX=atoi(argv[1]);
      SY=atoi(argv[2]);
      numt=10;
      break;
    case 4:
      SX=atoi(argv[2]);
      SY=atoi(argv[3]);
      numt=atoi(argv[1]);
      break;
    case 5:
      n_col= atoi(argv[4]);
      SX=atoi(argv[2]);
      SY=atoi(argv[3]);
      numt=atoi(argv[1]);
      break;
    case 6:
      graficos = 1;  //SALIDA GRAFICA
      n_col= atoi(argv[4]);
      SX=atoi(argv[2]);
      SY=atoi(argv[3]);
      numt=atoi(argv[1]);
      break;
    default:
      printf(" Posibilidades\n");
      printf(" maestro \n");
      printf(" maestro n_esclavos\n");
      printf(" maestro SX SY\n");
      printf(" maestro n_esclavos SX SY\n");
      printf(" maestro n_esclavos SX SY vueltas\n");
      printf(" maestro n_esclavos SX SY vueltas graf\n");
  }*/
  printf("configuracion esclavos(%d), pantalla(%d,%d)\n",numt,SX,SY);
  
  //****************crea archivo ppm-->paralelo_SX_SY.ppm***********************
  FILE *fp;
  char cadena[25];                 
  snprintf(cadena, sizeof(cadena),"paral1_%u_%u.ppm", SX, SY);
  fp = fopen(cadena, "w");  // se crea en carpeta LINUX64
  fprintf(fp, "P6\n%d %d\n255\n", SX, SY); // ppm header
  //***************************************************************************
  
  if (  (numt>MAX_HIJOS) || (numt<0)||
    (SX>MAX_COL) ||(SY>MAX_FILAS) || (SX<=0) || (SY<=0) ){
    printf("Error en los parametros \n");
  exit(-1);
    }
    /*
     *  Creación de la máquina virtual y paso de parámetros
     *
     */
    gettimeofday(&a_t,NULL);
    if((myTid=pvm_mytid())<0){       //identificar de la tarea actual. si es la primera llamada la aplicacion se registra en la PVM
      // no ha sido posible conectarcon pvm
      pvm_perror("[INCIO] error al empezar");
      exit(-1);
    }
    numt2=numt;
    numt=pvm_spawn("trabajador",NULL,0,NULL,numt2,TaskId);  //Una tarea PVM arranca otra tarea PVM ("trabajador"); numt2 nº de compias para empezar.
    //TaskId: Int array de long numt2 devuelve los tids de los PVM empezados
    //DEVUELVE EL NUMERO DE TAREAS QUE SIGUEN ABIERTAS
    if(numt<0){
      pvm_perror("[MAESTRO] error en spawn\n");
      pvm_exit();
      exit(-1);
    }
       
    if(numt<numt2){          //Hay abiertos menos trabajadores de los que se especifica
      int i;
      for(i=0;i<numt;i++){
	if(TaskId[i]<0){
	  printf("Error en la numero %d\n",i);
	}else{
	  pvm_sendsig(TaskId[i],15);  	
	}	
      }
      pvm_exit();                          // SE ME QUEDA SIEMPRE AQUÍ......
      exit(-1);
    }

    
    /* paso parametros a los esclavos para que empiezen a calcular*/
    pvm_initsend(PvmDataRaw);
    pvm_pkuint(&SX,1,1);
    pvm_pkuint(&SY,1,1);
    pvm_pkushort(&n_col,1,1);
    
    pvm_pkfloat(&Limit,1,1);
    pvm_pkfloat(&XRMin,1,1);
    pvm_pkfloat(&XRMax,1,1);
    pvm_pkfloat(&YRMin,1,1);
    pvm_pkfloat(&YRMax,1,1);
    pvm_pkint(&numt,1,1);
    
    if(pvm_mcast(TaskId,numt,PARAMETROS_MSG)){  //envio de msg multidestino. Se envian a los tids que contiene numt
      pvm_perror("[MASTER] al mandadr param");
      pvm_exit();
      exit(1);
    }
    
    for(i=0;i<numt;i++){
      // printf("En el indice %d tengo:%08X\n",i,TaskId[i]);
      pvm_initsend(PvmDataRaw);
      pvm_pkint(&i,1,1);
      pvm_send(TaskId[i],ID_WORKER_MSG);
    }
    /* limpio el vctor de lineas recividas*/
    
    recividas=0;
        
    while(recividas < SY){
      // no hemos recivido todas las lineas.
      // Nos bloqueamos esperando una.
      int bufId,contestar_a;
      unsigned int fila_c;
      
      if((bufId=pvm_recv(-1,RESULTADOS_MSG))<0){
	pvm_perror("[MASTER] recibniendo resultado");
	pvm_exit();
	exit(1);
      }
      pvm_bufinfo(bufId,(int*)NULL,(int*)NULL,&contestar_a);
      
      /* obtenemos los datos */                
      
      pvm_upkuint(&fila_c,1,1);			//( unsigned int *ip, int nitem, int stride )
      pvm_upkushort(pantalla[fila_c],SX,1);	//pvm_upkushort( unsigned short *ip, int nitem, int stride-->paso al desmpaquetar)
      
       
      recividas ++;
    }//FIN WHILE
    
         /********************************************   DIBUJAR CON pantalla[fila_c] *************************************************/
      gettimeofday(&b_t,NULL);
      unsigned int ii,j;
      for(ii=0;ii<SY;ii++){
	for(j=0; j<SX ; j++){
	int y,x;
	y=ii;
	x=j;
	
	if(pantalla[y][x]>n_col-2){   //Pintamos de BLANCO
	  rgb_f[0]=1;rgb_f[1]=1;rgb_f[2]=1;
	  
	}
	else if(pantalla[y][x]<=1){                       //Negro
	  rgb_f[0]=0;rgb_f[1]=0;rgb_f[2]=0;
	  
	}else{
	  rgb_f[0]=((pantalla[y][x])%7+1)/8.0f;
	  rgb_f[1]=((pantalla[y][x]+6)%17+1)/18.0f;
	  rgb_f[2]=0.1 + (((float)pantalla[y][x])/((float)n_col));
	}
	rgb[0] = CLAMP(rgb_f[0]*255);rgb[1] = CLAMP(rgb_f[1]*255);rgb[2] = CLAMP(rgb_f[2]*255);
	fwrite(rgb, 3, 1, fp);
      }
      }
      /****************************************************************************************************************************/     

    fclose(fp); //cerramos fichero despues de recibier todas las lineas
    
    /* Matar los esclavos  */
    // MATANDO A LOS TRABAJADORES
    pvm_initsend(PvmDataRaw);
    if(pvm_mcast(TaskId,numt,SALIDA_MSG)){ //msg multidestino
      pvm_perror("[MASTER] al salir");
      pvm_exit();
    }
    
    {
      double intervalo=1.0;
      intervalo=(double)(b_t.tv_sec-a_t.tv_sec);
      intervalo= intervalo + (b_t.tv_usec-a_t.tv_usec) / 1000000.0;
      
      printf("\nTIEMPO DE EJECUCIÓn : %f\n",intervalo);
      
    }
    return 0;
}
