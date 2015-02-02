#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "libbmp.h"

#define BITMAP_ID 0x4D42
#define BI_RGB 0x0000

void print(BITMAPFILE bmpfile){
	printf("------------------header-------------------------\n");
	printf("bfType: %d\n",bmpfile.bfHeader.bfType);
	printf("bfSize: %d\n",bmpfile.bfHeader.bfSize);
	printf("bfReserved1: %d\n",bmpfile.bfHeader.bfReserved1);
	printf("bfReserved2: %d\n",bmpfile.bfHeader.bfReserved2);
	printf("bfOffBits: %d\n",bmpfile.bfHeader.bfOffBits);
	printf("--------------------info----------------------------\n");
	printf("biSize: %d\n",bmpfile.biInfo.bmiHeader.biSize);
	printf("biWidth: %d\n",bmpfile.biInfo.bmiHeader.biWidth);
	printf("biHeight: %d\n",bmpfile.biInfo.bmiHeader.biHeight);
	printf("biPlanes: %d\n",bmpfile.biInfo.bmiHeader.biPlanes);
	printf("biBitCount: %d\n",bmpfile.biInfo.bmiHeader.biBitCount);
	printf("biCompression: %d\n",bmpfile.biInfo.bmiHeader.biCompression);
	printf("biSizeImage: %d\n",bmpfile.biInfo.bmiHeader.biSizeImage);
	printf("biXPelsPerMeter: %d\n",bmpfile.biInfo.bmiHeader.biXPelsPerMeter);
	printf("biYPelsPerMeter: %d\n",bmpfile.biInfo.bmiHeader.biYPelsPerMeter);
	printf("biClrUsed: %d\n",bmpfile.biInfo.bmiHeader.biClrUsed);
	printf("biClrImportant: %d\n",bmpfile.biInfo.bmiHeader.biClrImportant);
}

libbmp::libbmp(char* filename){
	this->filename=filename;
	width=0,height=0;
	pixellength=0;
	pixel=NULL;
	pos=NULL;
}

libbmp::libbmp(GLubyte* pixel,GLint width,GLint height){
	filename=0;
	this->width=width,this->height=height;
	this->pixel=pixel;
	this->pixellength=width*height*3;
	this->pos=NULL;
}

libbmp::~libbmp(){
	if(pixel!=NULL)
		delete[] pixel;
	if(pos!=NULL){
		for(int i=0;i<height;i++){
			if(pos[i]!=NULL)
				delete[] pos[i];
		}
		delete[] pos;
	}
}

void libbmp::pos2pixel(){
	if(pixel!=0)
		free(pixel);
	pixel=(GLubyte*)malloc(sizeof(GLubyte)*pixellength);
	GLint pixelwidth=width*3;
	while(pixelwidth%4!=0)
		pixelwidth++;
	for(int index=0;index<pixellength;index++){
		int i=index/pixelwidth;
		int k=index%pixelwidth;
		if(k>width*3){
			pixel[index]=0;
		}else{
			int j=k/3;
			int sub=k%3;
			switch(sub){
				case 0:pixel[index]=pos[i][j].B;break;
				case 1:pixel[index]=pos[i][j].G;break;
				case 2:pixel[index]=pos[i][j].R;break;
				default:break;
			}
		}
	}
}

void libbmp::pixel2pos(){
	if(pos!=0){
		for(int i=0;i<width;i++){
			if(pos[i]!=0)
				free(pos[i]);
		}
		free(pos);
	}
	GLint pixelwidth=width*3;
	while(pixelwidth%4!=0)
		pixelwidth++;
	pos=(RGB**)malloc(sizeof(RGB*)*height);
	for(int i=0;i<height;i++){
		pos[i]=(RGB*)malloc(sizeof(RGB)*width);
		for(int j=0;j<width;j++){
			pos[i][j].B=pixel[pixelwidth*i+j*3+0];
			pos[i][j].G=pixel[pixelwidth*i+j*3+1];
			pos[i][j].R=pixel[pixelwidth*i+j*3+2];
		}
	}
}

bool libbmp::load(){
	FILE* pfile=fopen(filename,"rb");
	if (pfile==0){
		printf("%s: Open Failure\n", filename);
		return false;
	}

	BITMAPFILE bmpfile;
	
	fread(&bmpfile.bfHeader,sizeof(BITMAPFILEHEADER),1,pfile);
	fread(&bmpfile.biInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,pfile);

	width=bmpfile.biInfo.bmiHeader.biWidth;
	height=bmpfile.biInfo.bmiHeader.biHeight;
	GLint pixelwidth=width*3;
	while(pixelwidth%4!=0)
		pixelwidth++;
	pixellength=pixelwidth*height;
	pixel=(GLubyte*)malloc(pixellength);

	if (pixel==0){
		printf("Pixellength Info Error: width=%d, height=%d, length=%d.\n",width,height,pixellength);
		return false;
	}

	GLint offset=bmpfile.biInfo.bmiHeader.biSize-sizeof(BITMAPINFOHEADER);
	GLubyte* extra=(GLubyte*)malloc(offset);
	fread(extra,offset,1,pfile);

	fread(pixel,pixellength,1,pfile);
	fclose(pfile);

	print(bmpfile);
	return true;
}

static GLint imagewidth;
static GLint imageheight;
static GLubyte* imagepixeldata;

void draw(){
	glDrawPixels(imagewidth,imageheight,GL_BGR_EXT,GL_UNSIGNED_BYTE,imagepixeldata);
	glFlush();
	glutSwapBuffers();
}

bool libbmp::display(){
	imagewidth=width;
	imageheight=height;
	imagepixeldata=pixel;
	// must call  glutInit(&argc,argv);	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(width,height);
	glutCreateWindow(filename);
	glutDisplayFunc(&draw);
	glutMainLoop();
	imagewidth=0;
	imageheight=0;
	imagepixeldata=0;
	return true;
}

bool libbmp::output(char* filename){
	this->filename=filename;

	FILE* pfile=fopen(filename,"wb");
	if(pfile==0){
		printf("%s :Open File Failure\n",filename);
		return false;
	}

	unsigned int bmppitch=((width*24+31)>>5)<<2;
	unsigned int filesize=bmppitch*height;

	BITMAPFILE bmpfile;

	bmpfile.bfHeader.bfType=0x4D42;
	bmpfile.bfHeader.bfSize=filesize+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmpfile.bfHeader.bfReserved1=0;
	bmpfile.bfHeader.bfReserved2=0;
	bmpfile.bfHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	bmpfile.biInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpfile.biInfo.bmiHeader.biWidth=width;
	bmpfile.biInfo.bmiHeader.biHeight=height;
	bmpfile.biInfo.bmiHeader.biPlanes=1;
	bmpfile.biInfo.bmiHeader.biBitCount=24;
	bmpfile.biInfo.bmiHeader.biCompression=0;
	bmpfile.biInfo.bmiHeader.biSizeImage=0;
	bmpfile.biInfo.bmiHeader.biXPelsPerMeter=0;
	bmpfile.biInfo.bmiHeader.biYPelsPerMeter=0;
	bmpfile.biInfo.bmiHeader.biClrUsed=0;
	bmpfile.biInfo.bmiHeader.biClrImportant=0;

	fwrite(&(bmpfile.bfHeader),sizeof(BITMAPFILEHEADER),1,pfile);
	fwrite(&(bmpfile.biInfo.bmiHeader),sizeof(BITMAPINFOHEADER),1,pfile);

	fwrite(pixel,pixellength,1,pfile);

	fclose(pfile);
	print(bmpfile);
	return true;
}
