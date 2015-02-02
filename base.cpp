#include <cmath>
#include "base.h"

RGB::RGB(){R=0,G=0,B=0;}

RGB::RGB(int b,int g,int r){R=r,G=g,B=b;}

double disRGB(RGB c1,RGB c2){
	double square=pow((c1.R-c2.R),2)+pow((c1.G-c2.G),2)+pow((c1.B-c2.B),2);
	return sqrt(square);
}

PAIR::PAIR(int i,int j){this->i=i,this->j=j;}

PAIR::PAIR(){i=-1,j=-1;}

PAIR plusPAIR(PAIR v1,PAIR v2){return PAIR(v1.i+v2.i,v1.j+v2.j);}