#include <vector>
#include <stdio.h>
#include "kdnode.h"
using namespace std;

kdnode::kdnode(int d){
	demension=d;
	split_demension=-1;
	split_value=0;
	points=vector<int>();
	points.clear();
	high_bound=new int[demension];
	low_bound=new int[demension];
	visited=false;
	parent=NULL,left=NULL,right=NULL;
}

kdnode::kdnode(kdnode *p){
	demension=p->demension;
	split_demension=-1;
	split_value=0;
	points= vector<int>();
	points.clear();
	high_bound=new int[demension];
	low_bound=new int[demension];
	visited=false;
	parent=p,left=NULL,right=NULL;
}

kdnode::kdnode(kdnode *p,int d){
	demension=d;
	split_demension=-1;
	split_value=0;
	points=vector<int>();
	high_bound=new int[demension];
	low_bound=new int[demension];
	visited=false;
	parent=p,left=NULL,right=NULL;
}


kdnode::~kdnode(){
	if(high_bound!=NULL)
		delete[] high_bound;
	if(low_bound!=NULL)
		delete[] low_bound;

	if(left!=NULL)
		delete left;
	if(right!=NULL)
		delete right;
}