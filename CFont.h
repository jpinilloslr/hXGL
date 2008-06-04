#ifndef CFontH
#define CFontH

#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<CTextureLoader.h>
#include<glaux.h>

class CFont
{
   private:
      GLuint Texture;
	  float PosX, PosY;
	  
	  
	  void DrawChar(int X, int Y);
	  void DrawChar(char c);	  
   public:
      CFont();
	  ~CFont();
	  float Size;
	  
      void LoadTexture(char *filename);	  
	  void DrawText(char *text, float X, float Y);
};

#endif