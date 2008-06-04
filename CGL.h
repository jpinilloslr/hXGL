#include <glaux.h>
#include <types.hpp>
#include <windows.h>

class CGL
{
    private:
       HDC dc;
       HGLRC rc;	
	public:
	   CGL();
	   ~CGL();
	   void InitOpenGL(HWND hWnd);
	   void Resize(HWND hWnd);
	   void StopOpenGL();
	   void StartSceneDraw();
	   void StopSceneDraw();
	   void Light(int Light, float X,float Y, float Z, bool Positional);
};