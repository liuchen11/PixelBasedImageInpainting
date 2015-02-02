#ifndef __PICTURE2_H__
#define __PICTURE2_H__

#include <GL/glut.h>
#include "base.h"
#include "libbmp.h"
#include "kdtree.h"

class picture2: public libbmp{
public:
	picture2(char* filename,int search_width);
	picture2(GLubyte* pixel, GLint width, GLint height,int search_width);
	~picture2();
	GLubyte** mask;
	void init_mask();
	void init_data();
	bool paint(PAIR position);
	PAIR choose_pixel(PAIR position);
private:
	int find_dependents(PAIR position,int r);
	bool haspoint(PAIR point);
	int search_width;
	int **data;
	int all;
	int completed;
	kdtree *tree;
};

#endif