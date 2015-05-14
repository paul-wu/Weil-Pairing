#include<stdio.h>
#include<malloc.h>
#include<math.h>

//define structure of element in finite fields
typedef struct fpoint{
	int x,y;
}FPOINT;


//define curve
typedef struct curve{
	FPOINT * A, * B;
}CURVE;


//define point in curve
typedef struct point{
	FPOINT * x, * y;
}POINT;

FPOINT * ZERO, * ONE;//zero element in field
POINT * O;//infinte point

FPOINT * newfpoint(int x, int y);

POINT * newpoint(int a, int b, int c, int d);

CURVE * newcurve(FPOINT * A, FPOINT * B);

void freepoint(POINT * a);

int ABS(int a, int p);

int gcdEx(int a, int b, int *x, int *y);

int inver(int a, int p);

bool equl(FPOINT * a, FPOINT * b);

bool equln(FPOINT * a, FPOINT * b, int p);

bool pequl(POINT * a, POINT * b);

FPOINT * fneg(FPOINT * a, int p, FPOINT * result);

FPOINT * fadd(FPOINT * a, FPOINT * b, int p, FPOINT * result);

FPOINT * fminus(FPOINT * a, FPOINT * b, int p, FPOINT * result);

FPOINT * fmulti(FPOINT * a, FPOINT * b, int p, FPOINT * result);

FPOINT * assign(FPOINT * a, FPOINT * b);

FPOINT * fpower(FPOINT * a, int n, int p, FPOINT * result);

FPOINT * inverse(FPOINT * a, int p, FPOINT * result);

FPOINT * fnmulti(FPOINT * a, int b, int p, FPOINT * result);

POINT * passign(POINT * a, POINT * b);

bool testpoint(POINT * p, CURVE * c, int p1);

void showpoint(POINT * p);

POINT * pneg(POINT * a, int p, POINT * result);

POINT * add(POINT * p1, POINT * p2, CURVE * c, int p, POINT * result);

POINT * minus(POINT * p1, POINT * p2, CURVE * c, int p, POINT * result);

POINT * ppower(POINT * a, int n, CURVE * c, int p, POINT * result);



