#include <GL/glut.h>
#include <stdio.h>
#include "base.h"
#include "kdtree.h"
#include "picture2.h"

picture2::picture2(char* filename,int search_width):libbmp(filename){
	all=0,completed=0;
	data=NULL,mask=NULL,tree=NULL;
	this->search_width=search_width>=2?search_width:2;
}

picture2::picture2(GLubyte* pixel, GLint width, GLint height,int search_width):libbmp(pixel,width,height){
	all=0,completed=0;
	data=NULL,mask=NULL,tree=NULL;
	this->search_width=search_width>=2?search_width:2;
}

picture2::~picture2(){
	if(mask!=NULL){
		for(int i=0;i<height;i++)
			if(mask[i]!=NULL)
				delete[] mask[i];
		delete[] mask;
	}
	if(data!=NULL){
		for(int i=0;i<height;i++)
			if(data[i]!=NULL)
				delete[] data[i];
		delete[] data;
	}
	if(tree!=NULL)
		delete tree;
}

void picture2::init_mask(){
	mask=new GLubyte*[height];
	for(int i=0;i<height;i++){
		mask[i]=new GLubyte[width];
		for (int j=0;j<width;j++){
			if(pos[i][j].R==0&&pos[i][j].G==0&&pos[i][j].B==0){
				mask[i][j]=0;
				all++;
			}else{
				mask[i][j]=1;
			}
		}
	}
}

void picture2::init_data(){
	int max_size=(width-2*search_width)*(height-2*search_width);
	int len=(search_width*2+1)*(search_width*2+1)*3;
	data=new int*[max_size];
	data[0]=new int[len];
	printf("max_size=%d, len=%d\n",max_size,len);
	int number=0;
	for(int i=search_width;i<height-search_width;i++){
		for(int j=search_width;j<width-search_width;){
			int break_point=0;
			bool success=true;
			for(int k1=-search_width;k1<=search_width&&success;k1++){
				for(int k2=-search_width;k2<=search_width&&success;k2++){
					if(mask[i+k1][j+k2]==0){
						success=false;
						break_point=k2;
					}
					int base=((k1+search_width)*(search_width*2+1)+k2+search_width)*3;
					data[number][base+0]=pos[i+k1][j+k2].B;
					data[number][base+1]=pos[i+k1][j+k2].G;
					data[number][base+2]=pos[i+k1][j+k2].R;
				}
			}
			if(success){
				number++;
				data[number]=new int[len];
				j++;
			}else{
				j+=break_point+search_width+1;
			}
		}
	}
	//build up a tree
	tree=new kdtree(len,number,data);
	printf("kdtree built\n");
}

int picture2::find_dependents(PAIR position, int r){
	int i=position.i,j=position.j;
	if(i<0||i>=height||j<0||j>=width||mask[i][j]==1)
		return 0;
	int dependents=0;
	for(int di=-r;di<=r;di++){
		for(int dj=-r;dj<=r;dj++){
			int si=i+di,sj=j+dj;
			if(si>=0&&si<height&&sj>=0&&sj<width&&mask[si][sj]==1)
				dependents++;
		}
	}
	return dependents;
}

bool picture2::haspoint(PAIR point){
	int i=point.i,j=point.j;
	if(i<0||i>=height||j<0||j>=width||mask[i][j]==0)
		return false;
	return true;
}


PAIR picture2::choose_pixel(PAIR position){
	int pi=position.i, pj=position.j;
	if(pi>=0&&pj>=0){
		int up=find_dependents(PAIR(pi+1,pj),search_width);
		int down=find_dependents(PAIR(pi-1,pj),search_width);
		int left=find_dependents(PAIR(pi,pj-1),search_width);
		int right=find_dependents(PAIR(pi,pj+1),search_width);
		if(up>=down&&up>=left&&up>=right&&up>6)
			return PAIR(pi+1,pj);
		if(down>=left&&down>=right&&down>6)
			return PAIR(pi-1,pj);
		if(left>=right&&left>6)
			return PAIR(pi,pj-1);
		if(right>6)
			return PAIR(pi,pj+1);
	}
	int maxpoints=0;
	PAIR maxpos(-1,-1);
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			int dependents=find_dependents(PAIR(i,j),5);
			if(dependents>maxpoints){
				maxpoints=dependents;
				maxpos.i=i,maxpos.j=j;
			}
		}
	}
	return maxpos;
}

bool picture2::paint(PAIR position){
	int i=position.i, j=position.j;
	if(mask[i][j]==1){
		printf("Paint Error!\n");
		return false;
	}

	int *q_mask=new int[(search_width*2+1)*(search_width*2+1)*3];
	int *query=new int[(search_width*2+1)*(search_width*2+1)*3];
	for(int di=-search_width;di<=search_width;di++){
		for(int dj=-search_width;dj<=search_width;dj++){
			int index=((di+search_width)*(search_width*2+1)+(dj+search_width))*3;
			if(i+di<0||i+di>=height||j+dj<0||j+dj>=width||mask[i+di][j+dj]==0){
				q_mask[index+0]=0,q_mask[index+1]=0,q_mask[index+2]=0;
				query[index+0]=-1,query[index+1]=-1,query[index+2]=-1;
			}else{
				q_mask[index+0]=1,q_mask[index+1]=1,q_mask[index+2]=1;
				query[index+0]=pos[i+di][j+dj].B;
				query[index+1]=pos[i+di][j+dj].G;
				query[index+2]=pos[i+di][j+dj].R;
			}
		}
	}

	int res=tree->search(query,q_mask);
	int *selected=data[res];

	int offset=(search_width*(search_width*2+1)+search_width)*3;
	pos[i][j].B=selected[offset+0];
	pos[i][j].G=selected[offset+1];
	pos[i][j].R=selected[offset+2];
	mask[i][j]=1;
	completed++;
	printf("i=%d\tj=%d\t(%d/%d)\r",position.i,position.j,completed,all);
	if(completed==all)
		printf("Completed!\n");
	delete[] q_mask;
	delete[] query;
	return true;
}