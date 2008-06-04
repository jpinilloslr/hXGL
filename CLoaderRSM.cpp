#include "CLoaderRSM.h"

void CLoaderRSM::LoadTexture(char *filename, GLuint &Texture)
{
	CTextureLoader TL;
	TL.LoadTexture(filename, Texture);
}

CLoaderRSM::CLoaderRSM()
{
	memset(&Header, 0, sizeof(CHeaderRSM));
	Surfaces = NULL;
	Textures = NULL;
	Lights   = NULL;
	DisplayList = -1;
	nTick = GetTickCount();
}

CLoaderRSM::~CLoaderRSM()
{
	if(Surfaces)
	{
	   delete[] Surfaces;
	}
	
	if(Textures)
	{
	   delete[] Textures;
	}	
	
	if(Lights)
	{
	   delete[] Lights;
	}	
}

int CLoaderRSM::Load(char *file)
{
	FILE *fpModel;
	char *Buffer, *p;
	int nCurrentTexture;
	
	fpModel = fopen(file, "rb");
	DisplayList = -1;
	
	if (!fpModel)
	{
	   return RSM_ERROR_FILE;
	}
	
	//leemos la cabecera del archivo
	fread(&Header, 1, sizeof(CHeaderRSM), fpModel);

	//verificamos que el archivo tenga el formato correcto
	if(strcmp(Header.szIdent, "RSM"))
	{
		fclose(fpModel);
		return RSM_ERROR_IDENT;
	}

	//verificamos que se trate de la version 1, la única que he escrito hasta el momento
	if(1 != Header.nVersion)
	{
		fclose(fpModel);
		return RSM_ERROR_VERSION;
	}	
	
	//terminadas las verificaciones leemos la cantidad de triángulos que usará el modelo y reservamos memoria para ellos
	Surfaces = new CSurfaceRSM[Header.nSurfaces*Header.nFrames];
	fread(Surfaces, 1, sizeof(CSurfaceRSM)*(Header.nSurfaces*Header.nFrames), fpModel);

	//procedemos a cargar las texturas del modelo
	Textures = new GLuint[Header.nTextures];

	//leemos el resto del fichero en un Buffer
	long nPos, nEof, nRest;

	nPos = ftell(fpModel);
	fseek(fpModel, 0L, SEEK_END);
	nEof = ftell(fpModel);
	fseek(fpModel, nPos, SEEK_SET);	
	nRest = nEof-nPos;
	
	Buffer = new char[nRest];
	fread(Buffer, 1, nRest, fpModel);

	//cargamos cada una de las texturas
	p = Buffer;
	nCurrentTexture = 0;
	
	while(strlen(p) > 0)
	{
		LoadTexture(p, Textures[nCurrentTexture]);
		p = p+strlen(p)+1;
		nCurrentTexture++;
	}	
	
	delete[] Buffer;
	fclose(fpModel);
	
	return RSM_SUCCESS;
}

bool CLoaderRSM::Draw()
{
    if(Surfaces)
    {
	   
	if(Header.nSurfaces > 0)
	{
	      glBindTexture(GL_TEXTURE_2D, Textures[Surfaces[0].Texture]);
	}	  
	   
	glBegin(GL_TRIANGLES);
	//recorremos todos los triangulos del modelo y los dibujamos
	for(unsigned int i=0; i<Header.nSurfaces; i++)
	{
		if((i > 0) && (Textures[Surfaces[i].Texture] != Textures[Surfaces[i-1].Texture]))
		{
			glEnd();
			glBindTexture(GL_TEXTURE_2D, Textures[Surfaces[i].Texture]);
			glBegin(GL_TRIANGLES);
		}   
		  
		for(int j=0; j<3; j++)
		{
			GenIlumination(Surfaces[i].Vertex[j]);
			glTexCoord2f(Surfaces[i].TextCoord[j].X, Surfaces[i].TextCoord[j].Y);
			glNormal3f(Surfaces[i].Normal[j].X, Surfaces[i].Normal[j].Y, Surfaces[i].Normal[j].Z);
			glVertex3f(Surfaces[i].Vertex[j].X, Surfaces[i].Vertex[j].Y, Surfaces[i].Vertex[j].Z);
		}
	}
	glEnd();
	return TRUE;
    }
    return FALSE;
}

bool CLoaderRSM::CallDisplayList()
{
	if(DisplayList != -1)
	{
		glCallList(DisplayList);
		return TRUE;
	}  

	return FALSE;	
}

bool CLoaderRSM::DrawFrame(unsigned int Frame)
{
    if((Surfaces) && (Frame<Header.nFrames))
    {
       int ArrayPos;
	   
	   if(Header.nSurfaces > 0)
	   {
	      glBindTexture(GL_TEXTURE_2D, Textures[Surfaces[0].Texture]);
	   }	  	   
	   
	   glBegin(GL_TRIANGLES);
	   for(unsigned int i=0; i<Header.nSurfaces; i++)
	   {
            ArrayPos = i+(Header.nSurfaces*Frame);
		   
			if((i > 0) && (Textures[Surfaces[i].Texture] != Textures[Surfaces[i-1].Texture]))
			{
			   glEnd();
			   glBindTexture(GL_TEXTURE_2D, Textures[Surfaces[i].Texture]);
			   glBegin(GL_TRIANGLES);
			}  		   
	        
	        for(int j=0; j<3; j++)
		    {
	            glTexCoord2f(Surfaces[ArrayPos].TextCoord[j].X, Surfaces[ArrayPos].TextCoord[j].Y);
			    glNormal3f(Surfaces[ArrayPos].Normal[j].X, Surfaces[ArrayPos].Normal[j].Y, Surfaces[ArrayPos].Normal[j].Z);
			    glVertex3f(Surfaces[ArrayPos].Vertex[j].X, Surfaces[ArrayPos].Vertex[j].Y, Surfaces[ArrayPos].Vertex[j].Z);
		    }
       }
	   glEnd();
  	   return TRUE;
    }
    return FALSE;
}

bool CLoaderRSM::InterpolateFrame(unsigned int Frame, unsigned int NextFrame, unsigned int Delay)
{
    if((Surfaces) && (Frame<Header.nFrames))
    {
       unsigned int ArrayPosC, ArrayPosN, nLeft;
	   float fProgress;
	   CVertex CVert, NVert, CNorm, NNorm,
	           VertDiv, VertPol, NormDiv, NormPol;
	   
	   if((nLeft = (GetTickCount() - nTick)) > Delay)
	   {
		   nTick = GetTickCount();
	   }	   
	   
	   //si el modelo lleva textura mapea la del primer triangulo
	   if(Header.nSurfaces > 0)
	   {
	      glBindTexture(GL_TEXTURE_2D, Textures[Surfaces[0].Texture]);
	   }	 	   
	   
	   glBegin(GL_TRIANGLES);
	   for(unsigned int i=0; i<Header.nSurfaces; i++)
	   {
            ArrayPosC = i+(Header.nSurfaces*Frame);
		    ArrayPosN = i+(Header.nSurfaces*NextFrame);
			
			//asigna textura solo cuando la necesita
			if((i > 0) && (Textures[Surfaces[i].Texture] != Textures[Surfaces[i-1].Texture]))
			{
			   glEnd();
			   glBindTexture(GL_TEXTURE_2D, Textures[Surfaces[i].Texture]);
			   glBegin(GL_TRIANGLES);
			}   			
            
	        for(int j=0; j<3; j++)
		    {
	            //calcula la interpolacion de normales y vertices
				CVert = Surfaces[ArrayPosC].Vertex[j];
			    NVert = Surfaces[ArrayPosN].Vertex[j];
				
				CNorm = Surfaces[ArrayPosC].Normal[j];
				NNorm = Surfaces[ArrayPosN].Normal[j];
				
			    VertDiv.X = NVert.X - CVert.X;
			    VertDiv.Y = NVert.Y - CVert.Y;
			    VertDiv.Z = NVert.Z - CVert.Z;
				
			    NormDiv.X = NNorm.X - CNorm.X;
			    NormDiv.Y = NNorm.Y - CNorm.Y;
			    NormDiv.Z = NNorm.Z - CNorm.Z;				
				
			    fProgress = 100.0f*(float)nLeft/(float)Delay;
				
			    VertPol.X = CVert.X + (VertDiv.X*fProgress/100.0f);
			    VertPol.Y = CVert.Y + (VertDiv.Y*fProgress/100.0f);
			    VertPol.Z = CVert.Z + (VertDiv.Z*fProgress/100.0f);
				
			    NormPol.X = CNorm.X + (NormDiv.X*fProgress/100.0f);
			    NormPol.Y = CNorm.Y + (NormDiv.Y*fProgress/100.0f);
			    NormPol.Z = CNorm.Z + (NormDiv.Z*fProgress/100.0f);				
				
				GenIlumination(VertPol);
			    glTexCoord2f(Surfaces[ArrayPosC].TextCoord[j].X, Surfaces[ArrayPosC].TextCoord[j].Y);
				glNormal3f(NormPol.X, NormPol.Y, NormPol.Z);
			    glVertex3f(VertPol.X, VertPol.Y, VertPol.Z);
		    }
	        
       }
	   glEnd();
       return TRUE;
    }
    return FALSE;
}

bool CLoaderRSM::Animate(unsigned int StartFrame, unsigned int EndFrame, unsigned int Delay, bool bInterpolated)
{
    unsigned int nNextFrame;
	
	if((0 == StartFrame) && (0 == EndFrame))
	{
	    EndFrame = Header.nFrames-1;
	}
	
	if((EndFrame < Header.nFrames) && (StartFrame < EndFrame))
	{
	    if(GetTickCount() - nTick >= Delay)
		{
		    nCurrFrame++;
			nTick = GetTickCount();
		}
		
		if((nCurrFrame > EndFrame) || (nCurrFrame < StartFrame))
		{
		   nCurrFrame = StartFrame;
		}
		
		nNextFrame = nCurrFrame+1;
		
		if(nNextFrame > EndFrame)
		{
		   nNextFrame = StartFrame;
		}
		
		if(bInterpolated)
		{
		   return InterpolateFrame(nCurrFrame, nNextFrame, Delay);
		}   
		return DrawFrame(nCurrFrame);
	}
	return FALSE;
}

void CLoaderRSM::Reset()
{
	if(Surfaces)
	{
	   delete[] Surfaces;
	   Surfaces = NULL;
	}
	
    if(Textures)
	{
	   glDeleteTextures(Header.nTextures, Textures);
	   delete[] Textures;
	   Textures = NULL;
	}	
	
    memset(&Header, 0, sizeof(CHeaderRSM));		
}

bool CLoaderRSM::BuildDisplayList()
{
   if(IsAnimated())
   {
      return FALSE;
   }
   
   DisplayList = glGenLists(1);
   
   glNewList(DisplayList, GL_COMPILE);
   Draw();
   glEndList();

   return TRUE;   
}

bool CLoaderRSM::SetLightsSources(int LightsCount, CLight *LightsSrc)
{
	Lights  = LightsSrc;
	nLights = LightsCount;
	
	return(Lights);
}

bool CLoaderRSM::GenIlumination(CVertex Vertex)
{
    if(Lights)
	{
	    float  fDist, fMaxDist, fAtte;
		CColor Color;
		
		memset(&Color, 0, sizeof(Color));
		
		for(unsigned int i=0; i<nLights; i++)
	    {
	       fDist = sqrt( pow((Vertex.X-Lights[i].Position.X),2) + pow((Vertex.Y-Lights[i].Position.Y),2) + 
		                 pow((Vertex.Z-Lights[i].Position.Z),2) );
		   
		   fMaxDist = Lights[i].Intensity;
		   
		    if(fDist < fMaxDist)
		    {
		       fAtte = (fDist*100)/fMaxDist;
		       fAtte = (100-fAtte)/100;
			   
			   Color.Red   += Lights[i].Color.Red   *fAtte;
			   Color.Green += Lights[i].Color.Green *fAtte;
			   Color.Blue  += Lights[i].Color.Blue  *fAtte;
			} 
	    }
		glColor3f(Color.Red, Color.Green, Color.Blue);
		return TRUE;
	}
	return FALSE;
}

bool CLoaderRSM::GetFirstTriangle(CVertex &X, CVertex &Y, CVertex &Z)
{
    if(Header.nSurfaces > 0)
	{
	    nCurrTriangle = 0;
		X = Surfaces[0].Vertex[0];
		Y = Surfaces[0].Vertex[1];
		Z = Surfaces[0].Vertex[2];
		
		nCurrTriangle++;
		return TRUE;
	}	
	return FALSE;
}

bool CLoaderRSM::GetNextTriangle(CVertex &X, CVertex &Y, CVertex &Z)
{
    if(nCurrTriangle < Header.nSurfaces)
	{
		X = Surfaces[nCurrTriangle].Vertex[0];
		Y = Surfaces[nCurrTriangle].Vertex[1];
		Z = Surfaces[nCurrTriangle].Vertex[2];
		
		nCurrTriangle++;
		return TRUE;
	}	
	return FALSE;
}