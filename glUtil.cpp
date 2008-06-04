#include "glUtil.h"

void CrossProduct(c_vec3 &result, c_vec3 a, c_vec3 b)
{
	result[0] = a[1] * b[2] - b[1] * a[2]; 
	result[1] = a[2] * b[0] - b[2] * a[0]; 
	result[2] = a[0] * b[1] - b[0] * a[1];	
}

float InnerProduct(c_vec3 v, c_vec3 q)
{
	return ((v)[0] * (q)[0] + (v)[1] * (q)[1] + (v)[2] * (q)[2]);
}

void Normalize(c_vec3 &vec)
{
    float len = (float)sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
	
	if (len) 
	{
		vec[0] /= len;;
		vec[1] /= len;
		vec[2] /= len;
	}	
}

void StartBillboarding(c_vec3 vCameraPos, c_vec3 vObjPos)
{
    float m[16];
	c_vec3 vLook, vRight, vUp;
	
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	
	vLook[0] = vCameraPos[0] - vObjPos[0];
	vLook[1] = vCameraPos[1] - vObjPos[1];
	vLook[2] = vCameraPos[2] - vObjPos[2];
	
	Normalize(vLook);
	
	vUp[0] = m[1];
	vUp[1] = m[5];
	vUp[2] = m[9];
	
	CrossProduct(vRight, vUp, vLook);
	CrossProduct(vUp, vLook, vRight);

	m[0] = vRight[0]; m[4] = vUp[0]; m[8] = vLook[0];  m[12] = vObjPos[0];
	m[1] = vRight[1]; m[5] = vUp[1]; m[9] = vLook[1];  m[13] = vObjPos[1];
	m[2] = vRight[2]; m[6] = vUp[2]; m[10] = vLook[2]; m[14] = vObjPos[2];
	m[3] = 0.0f;      m[7] = 0.0f;   m[11] = 0.0f;     m[15] = 1.0f;
	
	glPushMatrix();
	glMultMatrixf(m);
}

void StopBillboarding()
{
    glPopMatrix();
}

/*
** Retorna true si el rayo intersecta el triangulo
** p: punto origen del rayo
** d: vector del rayo
** v0, v1, v2: vertices del triangulo
** t: en esta variable se recoge la distancia en la la linea intersecto
*/
int RayIntersectsTriangle(float *p, float *d, float *v0, float *v1, float *v2, float &t) 
{
	float e1[3],e2[3],h[3],s[3],q[3];
	float a,f,u,v;
	
	(e1)[0] = (v1)[0] - (v0)[0];
	(e1)[1] = (v1)[1] - (v0)[1];
	(e1)[2] = (v1)[2] - (v0)[2];	
	
	(e2)[0] = (v2)[0] - (v0)[0];
	(e2)[1] = (v2)[1] - (v0)[1];
	(e2)[2] = (v2)[2] - (v0)[2];	
	
	CrossProduct(h,d,e2);
	a = InnerProduct(e1,h);
	
	if (a > -0.00001 && a < 0.00001)
		return(false);
	
	f = 1/a;
	
	(s)[0] = (p)[0] - (v0)[0];
	(s)[1] = (p)[1] - (v0)[1];
	(s)[2] = (p)[2] - (v0)[2];		
	
	u = f * (InnerProduct(s,h));
	
	if (u < 0.0 || u > 1.0)
		return(false);
	
	CrossProduct(q,s,e1);
	v = f * InnerProduct(d,q);
	if (v < 0.0 || u + v > 1.0)
		return(false);
	// at this stage we can compute t to find out where 
	// the intersection point is on the line
	t = f * InnerProduct(e2,q);
	if (t > 0.00001) // ray intersection
		return(true);
	else // this means that there is a line intersection  
		 // but not a ray intersection
		 return (false);
}