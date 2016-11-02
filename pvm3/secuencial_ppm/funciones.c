
#include "mandel.h"

unsigned short int Mandel(unsigned x,unsigned y)
{
  float X,Y,sR,sI,a2,b2;
  unsigned short int count=0;
  Y=YRMin+((YRMax-YRMin)*y)/SY;
  X=XRMin+((XRMax-XRMin)*x)/SX;
  sR=0.0;
  sI=0.0;
  do{
    a2=sR*sR;
    b2=sI*sI;
    sI=(2.0*sI*sR)+Y;
    sR=a2-b2+X;
    count ++;
  }while((count<n_col) && ((a2+b2)<Limit));
  return count;
}
