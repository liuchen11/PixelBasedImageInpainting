#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#include "libbmp.h"
#include "picture2.h"

int main(int argc, char *argv[])
{
	if(argc<4){
		printf("Usage ./demo <inputfile> <outputfile> <search_width>\n");
		return 0;
	}
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	picture2 p=picture2(argv[1],atoi(argv[3]));
	p.load();
	p.pixel2pos();
	p.init_mask();
	p.init_data();
	PAIR point(-1,-1);
	while(true){
		point=p.choose_pixel(point);
		if(point.i<0||point.j<0){
			break;
		}else{
			p.paint(point);
		}
	}
	p.pos2pixel();
	p.output(argv[2]);
	p.display();
	return 0;
}
