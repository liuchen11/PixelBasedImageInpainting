#include <cmath>
#include <vector>
#include <stdio.h>
#include "kdtree.h"
using namespace std;

void note_visited(kdnode* node){
	node->visited=true;
	if(node->left!=NULL)
		note_visited(node->left);
	if(node->right!=NULL)
		note_visited(node->right);
}

void clear_visited(kdnode* node){
	node->visited=false;
	if(node->left!=NULL)
		clear_visited(node->left);
	if(node->right!=NULL)
		clear_visited(node->right);
}

kdtree::kdtree(int d,int n,int** s){
	demension=d,number=n;
	data=s;
	root=new kdnode(demension);

	root->points.push_back(0);
	for(int i=0;i<demension;i++){
		root->high_bound[i]=data[0][i];
		root->low_bound[i]=data[0][i];
	}

	for(int i=1;i<number;i++){
		root->points.push_back(i);
		for(int j=0;j<demension;j++){
			if(data[i][j]>root->high_bound[j])
				root->high_bound[j]=data[i][j];
			if(data[i][j]<root->low_bound[j])
				root->low_bound[j]=data[i][j];
		}
	}

	//establish the tree
	split(root);
	clear_visited(root);
}

kdtree::~kdtree(){
	if(root!=NULL)
		delete root;
}

double kdtree::calc_var(int d,kdnode* node){
	double mean=0, var=0;
	int length=node->points.size();
	for(int i=0;i<length;i++){
		int n=node->points[i];
		mean+=data[n][d];
	}
	mean/=length;
	for(int i=0;i<length;i++){
		int n=node->points[i];
		var+=pow(data[n][d]-mean,2);
	}
	return var/length;
}

//hashmap
double kdtree::find_split_value(int d,kdnode* node){
	int high_lim=node->high_bound[d];
	int low_lim=node->low_bound[d];
	int *map=new int[high_lim-low_lim+1];
	for(int i=low_lim;i<=high_lim;i++)
		map[i-low_lim]=0;
	int len=node->points.size();
	for(int i=0;i<len;i++){
		int index=node->points[i];
		int value=data[index][d];
		map[value-low_lim]++;
	}
	int p=-1,num=0;
	while(num<len/2){
		p++;
		num+=map[p];
	}
	if(map[p]+len-2*num<0){
		delete[] map;
		return p+low_lim-1;
	}
	else{
		delete[] map;
		return p+low_lim;
	}
}

void kdtree::split(kdnode *node){
	if(node->points.size()<10)
		return;

	double max_var=calc_var(0,node);
	node->split_demension=0;
	for(int i=1;i<demension;i++){
		double var=calc_var(i,node);
		if(var>max_var){
			max_var=var;
			node->split_demension=i;
		}
	}

	//find the spliting point
	node->split_value=find_split_value(node->split_demension,node);

	//set up the left and right child
	if(max_var<1e-3){
		return;
	}

	node->left=new kdnode(node);
	node->right=new kdnode(node);

	for(int i=0;i<demension;i++){
		if(i!=demension){
			node->left->high_bound[i]=node->high_bound[i];
			node->left->low_bound[i]=node->low_bound[i];
			node->right->high_bound[i]=node->high_bound[i];
			node->right->low_bound[i]=node->low_bound[i];
		}else{
			node->left->high_bound[i]=node->split_value;
			node->left->low_bound[i]=node->low_bound[i];
			node->right->high_bound[i]=node->high_bound[i];
			node->right->low_bound[i]=node->split_value;
		}
	}
	for(int i=0;i<node->points.size();i++){
		int index=node->points[i];
		int v=data[index][node->split_demension];
		if(v<=node->split_value){
			node->left->points.push_back(index);
		}else{
			node->right->points.push_back(index);
		}
	}

	split(node->left);
	split(node->right);
	return;
}

void kdtree::add_point(int *toadd){
	kdnode* p=root;
	while(p->left!=NULL&&p->right!=NULL){
		int d=p->split_demension;
		int v=p->split_value;
		if(toadd[d]<=v)
			p=p->left;
		else
			p=p->right;
	}

	data[number]=toadd;
	p->points.push_back(number);
	number++;
	split(p);
}

int kdtree::lookup(kdnode *base,int *query,int *mask,double *min){
	if(base->visited==true)
		return -1;

	int min_point=-1;
	kdnode *p=base;

	note_visited(p);
	for(int i=0;i<p->points.size();i++){
		double dis=0;
		int index=p->points[i];
		for(int j=0;j<demension;j++){
			if(mask[j]!=0)
				dis+=pow(data[index][j]-query[j],2);
		}
		dis=sqrt(dis);
		if(dis<*min||*min<0){
			*min=dis;
			min_point=index;
		}
	}

	while(p!=NULL){
		kdnode* parent=p->parent;
		if(parent==NULL)
			break;
		bool overlap=false;
		for(int i=0;i<demension;i++){
			if(abs(query[i]-parent->high_bound[i])<*min){
				overlap=true;
				break;
			}
			if(abs(query[i]-parent->low_bound[i])<*min){
				overlap=true;
				break;
			}
		}
		if(overlap){
			p=p==parent->left?parent->right:parent->left;
			note_visited(parent);
			for(int i=0;i<p->points.size();i++){
				double dis=0;
				int index=p->points[i];
				for(int j=0;j<demension;j++){
					if(mask[j]!=0)
						dis+=pow(data[index][j]-query[j],2);
				}
				dis=sqrt(dis);
				if(dis<*min||*min<0){
					*min=dis;
					min_point=index;
				}
			}
		}else{
			break;
		}
		p=parent;
	}
	return min_point;
}

int kdtree::search(int* query){
	kdnode* p=root;
	while(p->left!=NULL&&p->right!=NULL){
		int d=p->split_demension;
		int v=p->split_value;
		if(query[d]<=v)
			p=p->left;
		else
			p=p->right;
	}

	double min_dis=-1;
	int* mask=new int[demension];
	for(int i=0;i<demension;i++)
		mask[i]=1;

	int min_point=lookup(p,query,mask,&min_dis);
	clear_visited(root);

	delete[] mask;
	return min_point;
}

int kdtree::search(int *query,int *mask){
	kdnode *p=root;
	vector<kdnode*> dict=vector<kdnode*>();
	dict.push_back(p);

	while(true){
		vector<kdnode*> new_dict=vector<kdnode*>();
		bool stop=true;
		for(int i=0;i<dict.size();i++){
			if(dict[i]->left!=NULL&&dict[i]->right!=NULL){
				stop=false;
			}else{
				new_dict.push_back(dict[i]);
				continue;
			}
			int d=dict[i]->split_demension;
			if(mask[d]!=0){
				int v=dict[i]->split_value;
				if(query[d]<=v)
					new_dict.push_back(dict[i]->left);
				else
					new_dict.push_back(dict[i]->right);
			}else{
				new_dict.push_back(dict[i]->left);
				new_dict.push_back(dict[i]->right);
			}
		}
		dict=new_dict;
		if(stop)
			break;
	}

	double min_dis=-1;
	int min_point=-1;
	for(int i=0;i<dict.size();i++){
		int res=lookup(dict[i],query,mask,&min_dis);
		if(res!=-1)
			min_point=res;
	}

	clear_visited(root);
	
	return min_point;
}