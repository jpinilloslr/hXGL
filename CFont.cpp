#include "CFont.h"

CFont::CFont()
{
  PosX = 0.0f;
  PosY = 0.0f;
  Size = 1.0f;
}

CFont::~CFont()
{
}

void CFont::LoadTexture(char *filename)
{
    CTextureLoader TL;
	if(!TL.LoadTexture(filename, Texture))
	{
	    MessageBoxA(NULL, "Error cargando fuente", "Error", NULL);
	}
}

void CFont::DrawChar(int X, int Y)
{
	glTexCoord2f((1.0f/16.0f)*X, ((1.0f/16.0f)*(Y-1)));
	glVertex3f((0.002f*Size)+PosX, (-0.002f*Size)+PosY, 0.0f);

	glTexCoord2f((1.0f/16.0f)*X, (1.0f/16.0f)*Y);
	glVertex3f((0.002f*Size)+PosX,  (0.002f*Size)+PosY, 0.0f);

	glTexCoord2f(((1.0f/16.0f)*(X-1)), (1.0f/16.0f)*Y);
	glVertex3f((-0.002f*Size)+PosX,  (0.002f*Size)+PosY, 0.0f);

	glTexCoord2f(((1.0f/16.0f)*(X-1)), ((1.0f/16.0f)*(Y-1)));
	glVertex3f((-0.002f*Size)+PosX, (-0.002f*Size)+PosY, 0.0f);
}

void CFont::DrawChar(char c)
{
   int X = 0;
   int Y = 16;
   
   for(byte i=0; i<=c; i++)
   {
      X++;
	  if(X>16)
	  {
	    X = 1;
		Y--;
	  }
   }   
   DrawChar(X, Y);
}

void CFont::DrawText(char *text, float X, float Y)
{
    PosX = X;
    PosY = Y;
   
	glBindTexture(GL_TEXTURE_2D, Texture);
	glLoadIdentity();

    GLboolean bLighting, bBlend;
    glGetBooleanv(GL_LIGHTING, &bLighting);
    glGetBooleanv(GL_BLEND, &bBlend);	
	
    glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
   
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.1f);
    glBegin(GL_QUADS);   
	
    for(unsigned int i=0; i<strlen(text); i++)
    {
        if(text[i] == '\n')
	    {
	       PosX = X;
		   PosY -= 0.0035f*Size;
	    }
	    else
	    {
	       DrawChar(text[i]);
	       PosX += 0.0035f*Size;
	    } 
    }
    glEnd();
    glPopMatrix();   
   
	if(bLighting)
	{
	   glEnable(GL_LIGHTING);
	}
	
	if(!bBlend)
	{
	   glDisable(GL_BLEND);
	}   
}