#ifndef glUtilH
#define glUtilH

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glaux.h>

typedef float c_vec3[3];

typedef struct
{
   c_vec3 v1;
   c_vec3 v2;
   c_vec3 v3;
} CTriangle;

typedef struct
{
   c_vec3 v1;
   c_vec3 v2;
} CLine;


void Normalize(c_vec3 &vec);
float InnerProduct(c_vec3 v, c_vec3 q);
void  CrossProduct(c_vec3 &result, c_vec3 a, c_vec3 b);
void StartBillboarding(c_vec3 vCameraPos, c_vec3 vObjPos);
void StopBillboarding();
int RayIntersectsTriangle(float *p, float *d, float *v0, float *v1, float *v2, float &t);

#endif
