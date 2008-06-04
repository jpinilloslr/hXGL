#include "CGL.h"

void CGL::Light(int Light, float X,float Y, float Z, bool Positional)
{
    float position[] = {0, 0, 0, 0};
    position[0] = X;
    position[1] = Y;
    position[2] = Z;
    position[3] = Positional;

    float ambiente[] = {0.1, 0.1, 0.1, 1.0};
    float difusa[] = {1.0, 1.0, 1.0, 1.0};
    float especular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat MatShine[] = {50.0};

    glLightfv (Light, GL_POSITION, position);
    glLightfv (Light, GL_AMBIENT, ambiente);
    glLightfv (Light, GL_DIFFUSE, difusa);
    glLightfv (Light, GL_SPECULAR, especular);


    glEnable(Light);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, MatShine);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
}

CGL::CGL()
{
}

CGL::~CGL()
{
}

void CGL::InitOpenGL(HWND hWnd)
{
      PIXELFORMATDESCRIPTOR pfd;
      int pf;

      dc = GetDC(hWnd);

      pfd.nSize=sizeof(pfd);
      pfd.nVersion=1;
      pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | 0;
      pfd.iPixelType=PFD_TYPE_RGBA;
      pfd.cColorBits=32;

      pf = ChoosePixelFormat(dc,&pfd);
      SetPixelFormat(dc,pf,&pfd);

      (void *)rc = wglCreateContext(dc);
      wglMakeCurrent(dc, rc);

      SetWindowPos(hWnd, NULL, 0, 0, 640, 480, 0);

      glClearColor(0.2,0.2,0.5,1.0);
      glShadeModel(GL_SMOOTH);
      glClearDepth(1.0);
      glDepthFunc(GL_LESS);
      glEnable(GL_DEPTH_TEST);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_COLOR_MATERIAL);
	  
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER, 0.4);
	  glEnable(GL_CULL_FACE);
	  
}

void CGL::Resize(HWND hWnd)
{
    TRect WinRect;
    int Width, Height;
    GetWindowRect(hWnd, &WinRect);

    Width = WinRect.Right - WinRect.Left;
    Height = WinRect.Bottom - WinRect.Top;
	
    if (Height == 0) 
	{
        Height+=1;
        SetWindowPos(hWnd, NULL, WinRect.Left, WinRect.Top, Width,Height,0);
    }

    //redefine algunos valores segun el nuevo
    //tamaño de ventana
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 65, (GLfloat)Width/(GLfloat)Height, 0.1f, 5000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CGL::StopOpenGL()
{
    wglMakeCurrent(0,0);
    wglDeleteContext(rc);
}

void CGL::StartSceneDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
}

void CGL::StopSceneDraw()
{
    glFlush();
    SwapBuffers(dc);
}