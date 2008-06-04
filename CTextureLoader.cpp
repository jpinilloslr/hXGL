#include "CTextureLoader.h"

CTextureLoader::CTextureLoader()
{
}

CTextureLoader::~CTextureLoader()
{
}

void *CTextureLoader::ReadTGA(char *filename, int *tam)
{
	GLubyte TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
	GLubyte TGAcompare[12];
	GLubyte header[6];
	GLuint bytesPerPixel;
	GLuint imageSize;
	GLuint temp,i;
	glImage texture;
	GLubyte *aux;
	
	FILE *file = fopen(filename, "rb");
	
	if (!file)
	{
	    return NULL;
	}
	
	fread(TGAcompare, 1, sizeof(TGAcompare), file);
	
	if (memcmp(TGAheader, TGAcompare,sizeof(TGAheader)) !=0 )
	{
	    return NULL;
	}
	
	fread(header, 1, sizeof(header), file);
	
	texture.width = header[1] * 256 + header[0];
	texture.height = header[3] * 256 + header[2];
	texture.bpp = header[4];
	
	bytesPerPixel = texture.bpp/8;
	imageSize = texture.width*texture.height*bytesPerPixel;
	texture.imageData=(GLubyte *)malloc(imageSize);
	
	if( texture.imageData==NULL || fread(texture.imageData, 1, imageSize, file) != imageSize)
	{
	    if(texture.imageData!=NULL)
		{
	        free(texture.imageData);
		}	
		
		fclose(file);
	    return NULL;
	}
	
	for(i=0; i<(GLuint)(imageSize); i+=bytesPerPixel)
	{
	    temp = texture.imageData[i];
	    texture.imageData[i] = texture.imageData[i + 2];
	    texture.imageData[i + 2] = temp;
	}
	fclose (file);
	
	aux=(GLubyte *)malloc(imageSize);
	
	for(i=0; i<texture.height; i++)
	{
	    memcpy(&aux[imageSize-((i+1)*texture.width*4)],&texture.imageData[i*texture.width*4],texture.width*4);
	}
	
	*tam=texture.width;
	free(texture.imageData);
	return aux;
}

bool CTextureLoader::LoadTGA(char *filename, GLuint &Texture)
{
	int nSize;
	void *texture_data = (char *)ReadTGA(filename, &nSize);
	
	if (!texture_data) 
	{
		return FALSE;
	}
	
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, nSize, nSize, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);	
	
	free(texture_data);
	return TRUE;
}

bool CTextureLoader::LoadTexture(char *filename, GLuint &Texture)
{
    char *p;
    p = filename + strlen(filename) - 4;
   
    for(int i=0; i<4; i++)
    {
        p[i] = tolower(p[i]);
    }
   
    if(!strcmp(p, ".tga"))
    {
        return LoadTGA(filename, Texture);
    }
    
	return BuildTexture(filename, Texture);
}

int CTextureLoader::BuildTexture(char *szPathName, GLuint &texid)		// Load Image And Convert To A Texture
{
	HDC			hdcTemp;												// The DC To Hold Our Bitmap
	HBITMAP		hbmpTemp;												// Holds The Bitmap Temporarily
	IPicture	*pPicture;												// IPicture Interface
	OLECHAR		wszPath[MAX_PATH+1];									// Full Path To Picture (WCHAR)
	char		szPath[MAX_PATH+1];										// Full Path To Picture
	long		lWidth;													// Width In Logical Units
	long		lHeight;												// Height In Logical Units
	long		lWidthPixels;											// Width In Pixels
	long		lHeightPixels;											// Height In Pixels
	GLint		glMaxTexDim ;											// Holds Maximum Texture Size

	strcpy(szPath, szPathName);
	
	GetCurrentDirectory(MAX_PATH, szPath);
	
	if (strstr(szPathName, "http://") || strstr(szPathName, ":\\"))        
	{
		strcpy(szPath, szPathName);
	}
	else																// Otherwise... We Are Loading From A File
	{
		strcat(szPath, "\\");											// Append "\" After The Working Directory
		strcat(szPath, szPathName);										// Append The PathName
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);		// Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))														// If Loading Failed
		return FALSE;													// Return False

	hdcTemp = CreateCompatibleDC(GetDC(0));								// Create The Windows Compatible Device Context
	if(!hdcTemp)														// Did Creation Fail?
	{
		pPicture->Release();											// Decrements IPicture Reference Count
		return FALSE;													// Return False (Failure)
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);					// Get Maximum Texture Size Supported
	
	pPicture->get_Width(&lWidth);										// Get IPicture Width (Convert To Pixels)
	lWidthPixels	= MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);										// Get IPicture Height (Convert To Pixels)
	lHeightPixels	= MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// Resize Image To Closest Power Of Two
	if (lWidthPixels <= glMaxTexDim) // Is Image Width Less Than Or Equal To Cards Limit
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else  // Otherwise  Set Width To "Max Power Of Two" That The Card Can Handle
		lWidthPixels = glMaxTexDim;
 
	if (lHeightPixels <= glMaxTexDim) // Is Image Height Greater Than Cards Limit
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else  // Otherwise  Set Height To "Max Power Of Two" That The Card Can Handle
		lHeightPixels = glMaxTexDim;
	
	//	Create A Temporary Bitmap
	BITMAPINFO	bi = {0};												// The Type Of Bitmap We Request
	DWORD		*pBits = 0;												// Pointer To The Bitmap Bits

	bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);				// Set Structure Size
	bi.bmiHeader.biBitCount		= 32;									// 32 Bit
	bi.bmiHeader.biWidth		= lWidthPixels;							// Power Of Two Width
	bi.bmiHeader.biHeight		= lHeightPixels;						// Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression	= BI_RGB;								// RGB Encoding
	bi.bmiHeader.biPlanes		= 1;									// 1 Bitplane

	//	Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hbmpTemp)														// Did Creation Fail?
	{
		DeleteDC(hdcTemp);												// Delete The Device Context
		pPicture->Release();											// Decrements IPicture Reference Count
		return FALSE;													// Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);									// Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)				// Loop Through All Of The Pixels
	{
		BYTE* pPixel	= (BYTE*)(&pBits[i]);							// Grab The Current Pixel
		BYTE  temp		= pPixel[0];									// Store 1st Color In Temp Variable (Blue)
		pPixel[0]		= pPixel[2];									// Move Red Value To Correct Position (1st)
		pPixel[2]		= temp;											// Move Temp Value To Correct Blue Position (3rd)

		// This Will Make Any Black Pixels, Completely Transparent		(You Can Hardcode The Value If You Wish)
		/*if ((pPixel[0]==0) && (pPixel[1]==0) && (pPixel[2]==0))			// Is Pixel Completely Black
			pPixel[3]	=   0;											// Set The Alpha Value To 0
		else		*/													// Otherwise
			pPixel[3]	= 255;											// Set The Alpha Value To 255
	}

	glGenTextures(1, &texid);											// Create The Texture

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texid);								// Bind To The Texture ID
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);		// (Modify This For The Type Of Filtering You Want)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);     // (Modify This For The Type Of Filtering You Want)
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);	// (Modify This If You Want Mipmaps)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, lWidthPixels, lHeightPixels, GL_RGBA, GL_UNSIGNED_BYTE, pBits);	

	DeleteObject(hbmpTemp);												// Delete The Object
	DeleteDC(hdcTemp);													// Delete The Device Context

	pPicture->Release();												// Decrements IPicture Reference Count

	return TRUE;														// Return True (All Good)
}