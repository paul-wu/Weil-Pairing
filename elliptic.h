#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<time.h>

#define lint long long

//define structure of element in finite fields
typedef struct fpoint{
	lint x,y;
}FPOINT;


//define curve
typedef struct curve{
	FPOINT * A, * B;
	lint p;
}CURVE;


//define point in curve
typedef struct point{
	FPOINT * x, * y;
}POINT;

void init();

lint modsquareroot(lint a, lint p);

bool testpoint(POINT * p, CURVE * c, lint p1);

POINT * randompoint(CURVE * c, lint p);

CURVE * curveassign(CURVE * a, CURVE * b); 

POINT * phi(POINT * a, lint p, POINT * result);

FPOINT * newfpoint(lint x, lint y);

POINT * newpoint(lint a, lint b, lint c, lint d);

CURVE * newcurve(lint A, lint B);

void freepoint(POINT * a);

lint ABS(lint a, lint p);

lint gcdEx(lint a, lint b, lint *x, lint *y);

lint inver(lint a, lint p);

lint randonsafeprime(lint n);

bool equl(FPOINT * a, FPOINT * b);

bool equln(FPOINT * a, FPOINT * b, lint p);

bool pequl(POINT * a, POINT * b);

FPOINT * fneg(FPOINT * a, lint p, FPOINT * result);

FPOINT * fadd(FPOINT * a, FPOINT * b, lint p, FPOINT * result);

FPOINT * fminus(FPOINT * a, FPOINT * b, lint p, FPOINT * result);

FPOINT * fmulti(FPOINT * a, FPOINT * b, lint p, FPOINT * result);

FPOINT * assign(FPOINT * a, FPOINT * b);

FPOINT * fpower(FPOINT * a, lint n, lint p, FPOINT * result);

FPOINT * inverse(FPOINT * a, lint p, FPOINT * result);

FPOINT * fnmulti(FPOINT * a, lint b, lint p, FPOINT * result);

POINT * passign(POINT * a, POINT * b);

bool testpoint(POINT * p, CURVE * c, lint p1);

void showpoint(POINT * p);

POINT * pneg(POINT * a, lint p, POINT * result);

POINT * add(POINT * p1, POINT * p2, CURVE * c, lint p, POINT * result);

POINT * minus(POINT * p1, POINT * p2, CURVE * c, lint p, POINT * result);

POINT * ppower(POINT * a, lint n, CURVE * c, lint p, POINT * result);

bool millerrabin(lint n, lint r);

lint findorder(POINT * po, CURVE * c, lint p);

bool evaluelinedivi(POINT * a, POINT * b, POINT * in, CURVE * c, lint p, FPOINT * result);

bool evaluelinedivi(POINT * a, POINT * b, POINT * in, CURVE * c, lint p, FPOINT * result);

bool miller(POINT * a, POINT * b, CURVE * c, lint p, lint m, FPOINT * f);

lint findorder(POINT * po, CURVE * c, lint p);

bool weilpairing(POINT * a, POINT * b, CURVE * c, lint p, lint n ,FPOINT * result);



