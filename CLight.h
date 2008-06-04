typedef struct
{
  float X;
  float Y;
  float Z;
} CPosition;

typedef struct
{
  float Red;
  float Green;
  float Blue;
} CColor;

typedef struct
{
  CPosition  Position;
  CColor     Color;
  float      Intensity;
} CLight;