#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pvm3.h"
#include "mandel.h"




float Limit, XRMin, XRMax, YRMin, YRMax;

unsigned int SX,SY;

unsigned short int **pantalla=NULL;

unsigned short n_col;



unsigned short *datos;


int main(int argc,char* argv[],char **envp)
{
  int myTid,numt;
  int bufId;
  int masterTid;
  int cuantos;
  unsigned int linea;
  unsigned int col; 
  unsigned int mias=0;
  int inicial=0;
  int un_id;

  if((myTid=pvm_mytid())<0){
    pvm_perror("[otros_INICIO] ERROR mytid");
  }

  
  /* Recive la información del master */
  if((bufId=pvm_recv(-1,PARAMETROS_MSG))<0){
    pvm_perror("[ESCLAVO] recibniendo parametros");
    pvm_exit();
    exit(1);
  }

  pvm_bufinfo(bufId,(int*)NULL,(int*)NULL,&masterTid);
  pvm_upkuint(&SX,1,1);                                   //acceso a los contenidos de un msg. Una vez extraido un campo se pasa al siguiente
  pvm_upkuint(&SY,1,1);
  pvm_upkushort(&n_col,1,1);            

  pvm_upkfloat(&Limit,1,1);
  pvm_upkfloat(&XRMin,1,1);
  pvm_upkfloat(&XRMax,1,1);
  pvm_upkfloat(&YRMin,1,1);
  pvm_upkfloat(&YRMax,1,1);
  pvm_upkint(&numt,1,1);

  datos=(unsigned short *) malloc( SX*(sizeof(unsigned short)) );
  
  cuantos=numt;   //cuantos somos

  //Quien soy de esos N 
  pvm_recv(-1,ID_WORKER_MSG);    //Rx bloqueante de un msg. Espera cualquier msg (-1). NO hace nada hasta la rx del msg. devuelve ID de buffer de msg
  pvm_upkint(&un_id,1,1);        


  // SAbiendo cuantos, cuantas lineas y quien soy puedo saber
  // cuantas lineas me tocan a mi (redondeado por arriba)
  {
    int a,b;
    a= SY / cuantos;
    b= SY % cuantos; // modulo.
    
    mias=a;           // seguo que me tocan estas.
    inicial=un_id; // Que línea es la primera que debo calcular:

    if(b){
      // no es un divisor exacto;
      if(b>un_id){
	mias=mias+1;
      }
      // se desplza la inicial de cada uno
    }
  }
  
  linea=inicial;
  
  while(linea<=(SY-cuantos+inicial)){   
    unsigned short *a = datos; 
   for(col=0;col<SX;col++)
      *(a++)= Mandel(col,linea,n_col);

    pvm_initsend(PvmDataRaw);  // INforma a PVM q el proceso va a realizar un envio y q quiere preparar un msg
    pvm_pkuint(&linea,1,1);
    pvm_pkushort(datos,SX,1);
 
    if(pvm_send(masterTid,RESULTADOS_MSG)){  //envio de un msg. Destino--> RESULTADOS_MSG;  Identificador de tipo de msg: masterTid
      pvm_perror("[MASTER] al mandadr un num columna");
      pvm_exit();
      exit(1);
    }
    linea+=cuantos;  
  }

  if((bufId=pvm_recv(-1,SALIDA_MSG))<0){
    pvm_perror("[ESCLAVO] recibniendo parametros");
    pvm_exit();
    exit(1);
  }
  pvm_exit();
  exit(1);
}
