#ifndef CCameraH
#define CCameraH

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <glaux.h>
#include <math.h>
#include <CFont.h>

#define GET_KEYDOWN(key)  ((int) ((GetAsyncKeyState(key) & (1 << 15)) >> 15))

#define WLK_FOREWARD  0
#define WLK_BACKWARD  1
#define WLK_RIGHT     2
#define WLK_LEFT      3

class CCamera
{
   private:
     GLfloat camX, lastX;
     GLfloat camY, lastY;
     GLfloat camZ, lastZ;
     GLfloat camYaw;
     GLfloat camPitch;
	 GLfloat camRoll;
	 CFont   *Font;
	 
	 void ClampCamera();
	 void Walk(int Direction, float fVel);
   public:
      CCamera();
	  ~CCamera();
	  void Control(float VelMov, float VelCam, bool bActive);
	  void Control(float pX, float pY, float pZ, float fVel);
	  void UpdatePosition();
	  void RestorePosition();
	  void RestorePosition(char c);
	  void ShowModelViewMatrix();
	  
	  void SetPositionX(GLfloat X)    {  camX = X;  }
	  void SetPositionY(GLfloat Y)    {  camY = Y;  }
	  void SetPositionZ(GLfloat Z)    {  camZ = Z;  }
	  
	  void SetYaw(float fYaw)         {  camYaw = fYaw;     }
	  void SetPitch(float fPitch)     {  camPitch = fPitch; }
	  
	  float GetPositionX()            {  return camX;  }
	  float GetPositionY()            {  return camY;  }
	  float GetPositionZ()            {  return camZ;  }
	  
	  float GetYaw()                  {  return camYaw;   }
	  float GetPitch()                {  return camPitch; }		 
	  
	  bool PositionChanged();
};

#endif