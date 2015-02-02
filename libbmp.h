#ifndef __LIBBMP_H__
#define __LIBBMP_H__

#include <GL/glut.h>
#include "base.h"

class libbmp{
public:
	libbmp(char* filename);
	libbmp(GLubyte* pixel,GLint width,GLint height);
	~libbmp();
	void pos2pixel();
	void pixel2pos();
	bool load();
	bool display();
	bool output(char* filename);
	GLint width;
	GLint height;
	GLint pixellength;
	GLubyte *pixel;
	RGB **pos;
protected:
	char* filename;
};

#endif