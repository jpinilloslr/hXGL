#ifndef CLoaderRSMH
#define CLoaderRSMH

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <CLight.h>
#include <CTextureLoader.h>
#include <glaux.h>

#define  RSM_SUCCESS        0
#define  RSM_ERROR_FILE     1
#define  RSM_ERROR_IDENT    2
#define  RSM_ERROR_VERSION  3

typedef struct
{
   float X;
   float Y;
} CTextCoord;

typedef struct
{
   float X;
   float Y;
   float Z;
} CVertex;

typedef struct
{
    char          szIdent[4];
	char          szName[15];
	unsigned int  nVersion;
	unsigned int  nSurfaces;
	unsigned int  nTextures;
	unsigned int  nFrames;
} CHeaderRSM;

typedef struct
{
    CVertex      Vertex[3];
	CVertex      Normal[3];
	CTextCoord   TextCoord[3];
    unsigned int Texture;
} CSurfaceRSM;

class CLoaderRSM
{
    private:
	   CHeaderRSM    Header;
	   CSurfaceRSM   *Surfaces;
	   GLuint        *Textures;
	   CLight        *Lights;
	   GLuint        DisplayList;
	   unsigned int  nLights;
	   unsigned int  nTick;
	   unsigned int  nCurrFrame, nCurrTriangle;
	   
	   void LoadTexture(char *filename, GLuint &Texture);
	   bool GenIlumination(CVertex Vertex);
	   
	public:
	   CLoaderRSM();
	   ~CLoaderRSM();
	   
	   int Load(char *file);
	   bool DrawFrame(unsigned int Frame);
	   bool InterpolateFrame(unsigned int Frame, unsigned int NextFrame, unsigned int Delay);
	   bool Animate(unsigned int StartFrame, unsigned int EndFrame, unsigned int Delay, bool bInterpolated);
	   bool Draw();
	   bool CallDisplayList();
	   void Reset();
	   bool BuildDisplayList();
	   int  GetTrianglesCount()              { return Header.nSurfaces;     }
	   int  GetFramesCount()                 { return Header.nFrames;       }	   
	   int  GetTexturesCount()               { return Header.nTextures;     }
	   void GetName(char *Name)              { strcpy(Name, Header.szName); }
	   bool IsAnimated()                     { return (Header.nFrames > 1); }
	   
	   bool SetLightsSources(int LightsCount, CLight *LightsSrc);
	   
	   bool GetFirstTriangle(CVertex &X, CVertex &Y, CVertex &Z);
	   bool GetNextTriangle(CVertex &X, CVertex &Y, CVertex &Z);
};

#endif