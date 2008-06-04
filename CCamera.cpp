#include "CCamera.h"

CCamera::CCamera()
{
   camX = 0.0f;
   camY = 0.0f;
   camZ = 0.0f;
   camYaw = 180.0f;
   camPitch = 0.0f;
   
   Font = new CFont();
   //Font->LoadTexture("\\Data\\Font.jpg");
   Font->Size = 0.7f;
}

CCamera::~CCamera()
{
    delete Font;
}

void CCamera::ClampCamera()
{
    if(camPitch > 90.0f)
    {
  	  camPitch = 90.0f;
	}  
    else 
    if(camPitch < -90.0f)
	{
	   camPitch = -90.0f;
	}
	
    while(camYaw < 0.0f)
    {
	   camYaw += 360.0f;
	}   
	
    while(camYaw >= 360.0f)
	{
	   camYaw -= 360.0f;
	}   
}

void CCamera::Control(float pX, float pY, float pZ, float fVel)
{
	/*    Controles:
	**    Arriba - Abajo
	**    Derecha - Inzquierda
	**    Z - X
	*/

	if ((GetKeyState('S') != 0) && ((GetKeyState('S')) != 1)) 
	{
		camYaw=camYaw+fVel;
	}

	if ((GetKeyState('W') != 0) && ((GetKeyState('W')) != 1)) 
	{
		camYaw=camYaw-fVel;
	}

	if ((GetKeyState('X') != 0) && ((GetKeyState('X')) != 1)) 
	{
		camRoll=camRoll+fVel;
	}

	if ((GetKeyState('Z') != 0) && ((GetKeyState('Z')) != 1)) 
	{
		camRoll=camRoll-fVel;
	}

	if ((GetKeyState('D') != 0) && ((GetKeyState('D')) != 1)) 
	{
		 camPitch= camPitch+fVel;
	}

	if ((GetKeyState('A') != 0) && ((GetKeyState('A')) != 1)) 
	{
		 camPitch= camPitch-fVel;
	}

	glTranslatef(pX, pY, pZ);	
    glRotatef(camYaw, 1.0, 0.0, 0.0);
    glRotatef(camPitch, 0.0, 1.0, 0.0);
    glRotatef(camRoll, 0.0, 0.0, 1.0);
}

void CCamera::Control(float VelMov, float VelCam, bool bActive)
{
    POINT CurPos;

    if (bActive)
    {
		GetCursorPos(&CurPos);
        int MidX, MidY;
        float Sen = 0;
        MidX = 640/2;
        MidY = 480/2;
        
		ShowCursor(false);
      
	    //Control del mouse
		if (CurPos.x > MidX+Sen)
        {
            camYaw += ((VelCam * (MidX - CurPos.x))/2.0f);
            ClampCamera();
        }
		
        if (CurPos.x < MidX-Sen)
        {
            camYaw -= ((VelCam * (CurPos.x - MidX))/2.0f);
            ClampCamera();
        }
		
        if ((CurPos.y > MidY+Sen))
        {
            camPitch -= ((VelCam * (CurPos.y - MidY))/2.0f);
            ClampCamera();
        }
		
        if ((CurPos.y < MidY-Sen) )
        {
            camPitch += ((VelCam * (MidY - CurPos.y))/2.0f);
            ClampCamera();
        }
		
        SetCursorPos(MidX,MidY);
		
        glRotatef(-camPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(-camYaw, 0.0f, 1.0f, 0.0f);		
		
	    //Control del teclado
	    if ((GET_KEYDOWN(VK_UP)) || (GET_KEYDOWN('W')))
        {
            if ((camPitch != 90) && (camPitch != -90))
			{
                Walk(WLK_FOREWARD, VelMov);
			}	
        }
		
        if ((GET_KEYDOWN(VK_DOWN)) || (GET_KEYDOWN('S')))
        {
            if ((camPitch != 90) && (camPitch != -90))
			{
                Walk(WLK_BACKWARD, VelMov);
			}	
        }
		
        if ((GET_KEYDOWN(VK_LEFT)) || (GET_KEYDOWN('A')))
        {
            Walk(WLK_LEFT, VelMov);
        }
		
        if ((GET_KEYDOWN(VK_RIGHT)) || (GET_KEYDOWN('D')))
        {
            Walk(WLK_RIGHT, VelMov);
        }		
    }
}

void CCamera::UpdatePosition()
{
	lastX = camX;
	lastY = camY;
	lastZ = camZ; 
	
	glTranslatef(-camX, -camY, -camZ);
}

void CCamera::RestorePosition()
{
	camX = lastX;
	camY = lastY;
	camZ = lastZ;
	
	glTranslatef(-camX, -camY, -camZ);
}

void CCamera::RestorePosition(char c)
{
	if(c == 'X')
		camX = lastX;
	if(c == 'Y')
		camY = lastY;
	if(c == 'Z')
		camZ = lastZ;

	lastX = camX;
	lastY = camY;
	lastZ = camZ; 		

	glTranslatef(-camX, -camY, -camZ);
}

void CCamera::ShowModelViewMatrix()
{
    char szText[256];
    float m[16];
    
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
  
    sprintf(szText, "%f  %f  %f  %f\n"
				    "%f  %f  %f  %f\n"
				    "%f  %f  %f  %f\n"
				    "%f  %f  %f  %f\n", 
				    m[0], m[1], m[2], m[3], 
				    m[4], m[5], m[6], m[7], 
				    m[8], m[9], m[10], m[11], 
				    m[12], m[13], m[14], m[15]);
   
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    Font->DrawText("Model View Matrix", -0.08f, -0.05f);	
    glColor3f(0.0f, 1.0f, 0.0f);
    Font->DrawText(szText, -0.08f, -0.053f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

void CCamera::Walk(int Direction, float fVel)
{
    float m[16], v_look[3], v_right[3];
   
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
   
    v_look[0] = m[2];
    v_look[1] = m[6];
    v_look[2] = m[10];
   
    v_right[0] = m[0];
    v_right[1] = m[4];
    v_right[2] = m[8];
 
    switch (Direction)
    {
        case WLK_FOREWARD:
		{
            v_look[0] *= fVel;
            v_look[1] *= fVel;
            v_look[2] *= fVel;
			
            camX -= v_look[0];
            camY -= v_look[1];
            camZ -= v_look[2];
			break;
		}
		
        case WLK_BACKWARD:
		{
            v_look[0] *= fVel;
            v_look[1] *= fVel;
            v_look[2] *= fVel;
			
            camX += v_look[0];
            camY += v_look[1];
            camZ += v_look[2];
			break;
		}		
		
        case WLK_RIGHT:
		{
            v_right[0] *= fVel;
            v_right[1] *= fVel;
            v_right[2] *= fVel;
			
            camX += v_right[0];
            camY += v_right[1];
            camZ += v_right[2];
			break;
		}		
		
        case WLK_LEFT:
		{
            v_right[0] *= fVel;
            v_right[1] *= fVel;
            v_right[2] *= fVel;
			
            camX -= v_right[0];
            camY -= v_right[1];
            camZ -= v_right[2];
			break;
		}			
    }
}

bool CCamera::PositionChanged()
{
    bool b;
	b = ((camX != lastX) || (camY != lastY) || (camZ != lastZ));
	
	lastX = camX;
	lastY = camY;
	lastZ = camZ;
	
	return b;
}
