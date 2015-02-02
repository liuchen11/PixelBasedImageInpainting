#ifndef __KDTREE_H__
#define __KDTREE_H__

#include "kdnode.h"

class kdtree{
public:
	int demension;
	int number;
	int** data;
	kdnode *root;
	kdtree(int d,int n,int** s);
	~kdtree();
	int search(int* query);
	int search(int* query,int* mask);
	void add_point(int *toadd);
private:
	double calc_var(int d,kdnode* node);
	void split(kdnode* node);
	double  find_split_value(int d,kdnode* node);
	int lookup(kdnode *base,int *query, int* mask, double* min);
};

#endif