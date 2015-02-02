#include <sys/types.h> 

#ifndef _BASE_H_
#define _BASE_H_
 
#pragma pack(push, 1)
 
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;
 
typedef struct RGB
{
    RGB();
    RGB(int b,int g,int r);
    U8 B;
    U8 G;
    U8 R;
} RGB;

double disRGB(RGB c1,RGB c2);

typedef struct PAIR
{
    PAIR();
    PAIR(int i,int j);
    int i;
    int j;
} PAIR;

PAIR plusPAIR(PAIR v1,PAIR v2);

typedef struct BITMAPFILEHEADER
{
    U16 bfType;
    U32 bfSize;
    U16 bfReserved1;
    U16 bfReserved2;
    U32 bfOffBits;
} BITMAPFILEHEADER;
 
typedef struct BITMAPINFOHEADER
{
    U32 biSize;
    U32 biWidth;
    U32 biHeight;
    U16 biPlanes;
    U16 biBitCount;
    U32 biCompression;
    U32 biSizeImage;
    U32 biXPelsPerMeter;
    U32 biYPelsPerMeter;
    U32 biClrUsed;
    U32 biClrImportant;
} BITMAPINFOHEADER;
 
typedef struct RGBQUAD
{
    U8 rgbBlue;
    U8 rgbGreen;
    U8 rgbRed;
    U8 rgbReserved;
} RGBQUAD;
 
typedef struct BITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
} BITMAPINFO;
 
 
typedef struct BITMAP
{
    BITMAPFILEHEADER bfHeader;
    BITMAPINFO biInfo;
}BITMAPFILE;
 
#pragma pack(pop)
 
#endif