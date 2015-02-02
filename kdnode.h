#ifndef __KDNODE_H__
#define __KDNODE_H__

#include <vector>
using namespace std;

class kdnode{
public:
	int demension;
	int split_demension,split_value;
	vector<int> points;
	int *high_bound,*low_bound;
	bool visited;
	kdnode *parent;
	kdnode *left,*right;
	kdnode(int d);
	kdnode(kdnode *p);
	kdnode(kdnode *p,int d);
	~kdnode();
private:
};

#endif