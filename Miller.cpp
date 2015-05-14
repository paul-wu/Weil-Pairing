#include"elliptic.h"


FPOINT * newfpoint(int x, int y)
{
	FPOINT * result = (FPOINT *)malloc(sizeof(fpoint));
	result->x = x;
	result->y = y;
	return result;
}

POINT * newpoint(int a, int b, int c, int d)
{
	POINT * result = (POINT *)malloc(sizeof(point));
	result->x = newfpoint(a,b);
	result->y = newfpoint(c,d);
	return result;
}

CURVE * newcurve(FPOINT * A, FPOINT * B)
{
	CURVE * result = (CURVE *)malloc(sizeof(curve));
	result->A = A;
	result->B = B;
	
	return result;
}

void freepoint(POINT * a)
{
	free(a->x);
	free(a->y);
}

//absolute value modulo p
int ABS(int a, int p)
{
	return (a>=0)?a%p:(p-(-a)%p)%p;
}

//extended euclid algorithm
int gcdEx(int a, int b, int *x, int *y) 
{
    if(b==0){
        *x = 1,*y = 0;
        return a;
    }
    else{
        int r = gcdEx(b, a%b, x, y);
        int t = *x;
        *x = *y;
        *y = t - a/b * *y;
        return r;
    }
}


//modulo inverse
int inver(int a, int p)
{
	int s,t;
	if(gcdEx(a,p,&s,&t)!=1)return 0;
	return ABS(s,p);
}

//field element equlity judgement
bool equl(FPOINT * a, FPOINT * b)
{
	if(a->x == b->x && a->y == b->y)return true;
	return false;
}

bool equln(FPOINT * a, FPOINT * b, int p)
{
	if(a->x == ABS(-b->x,p)&&a->y == ABS(-b->y,p))return true;
	return false;
}

bool pequl(POINT * a, POINT * b)
{
	return equl(a->x,b->x)&&equl(a->y,b->y);
}

FPOINT * fneg(FPOINT * a, int p, FPOINT * result)
{
	result->x = ABS(-a->x,p);
	result->y = ABS(-a->y,p);
	
	return result;
}

//field elements addition
FPOINT * fadd(FPOINT * a, FPOINT * b, int p, FPOINT * result)
{
	int x = ABS(a->x + b->x,p);
	int y = ABS(a->y + b->y,p);
	
	result->x = x;
	result->y = y;
	
	return result;
}

//field elements minus
FPOINT * fminus(FPOINT * a, FPOINT * b, int p, FPOINT * result)
{
	int x = ABS(a->x - b->x,p);
	int y = ABS(a->y - b->y,p);
	
	result->x = x;
	result->y = y;
	
	return result;
}

//field elements multiplication
FPOINT * fmulti(FPOINT * a, FPOINT * b, int p, FPOINT * result)
{
	int x = ABS(a->y*b->x + a->x*b->y,p);
 	int y = ABS(a->y*b->y - a->x*b->x,p);
 	
 	result->x = x;
 	result->y = y;
 	
	return result;
}

//field assignment
FPOINT * assign(FPOINT * a, FPOINT * b)
{
	a->x = b->x;
	a->y = b->y;
	return a;
}


//field element expontinal
FPOINT * fpower(FPOINT * a, int n, int p, FPOINT * result)
{
	n = n%(p*p - 1);
	
	FPOINT * DB = newfpoint(0,0);
	
	assign(DB,a);
	assign(result,ONE);

	while(n > 0){
		if(n&1)fmulti(DB,result,p,result);
		fmulti(DB,DB,p,DB);
		n >>= 1;	
	}
	
	free(DB);
	
	return result;
}

//field element inverse
FPOINT * inverse(FPOINT * a, int p, FPOINT * result)
{	
	int x = inver(ABS(-a->y*a->y*inver(a->x,p)-a->x,p),p);
	int y = ABS(-a->y*inver(a->x,p)*x,p);
	
	result->x = x;
	result->y = y;
	
	return result;
}

//multiply by number
FPOINT * fnmulti(FPOINT * a, int b, int p, FPOINT * result)
{
	result->x = ABS(a->x*b,p);
	result->y = ABS(a->y*b,p);
	return result;
}

POINT * passign(POINT * a, POINT * b)
{
	assign(a->x,b->x);
	assign(a->y,b->y);
	return a;
}

bool testpoint(POINT * p, CURVE * c, int p1)
{
	if(pequl(p,O))return true;
	
	FPOINT * x = newfpoint(0,0);
	FPOINT * y = newfpoint(0,0);
	
	assign(x,p->x);
	
	fadd(fadd(fpower(x,3,p1,x),fmulti(c->A,p->x,p1,y),p1,x),c->B,p1,x);
	
	fpower(p->y,2,p1,y);
	
	return equl(x,y);
}

void showpoint(POINT * p)
{
	printf("\n[(%d,%d),(%d,%d)]\n",p->x->x,p->x->y,p->y->x,p->y->y);
}

POINT * pneg(POINT * a, int p, POINT * result)
{
	assign(result->x,a->x);
	fneg(a->y,p,result->y);
	
	return result;
}

//curve point additon
POINT * add(POINT * p1, POINT * p2, CURVE * c, int p, POINT * result)
{	
	if(pequl(p1,O))return passign(result,p2);
	
	if(pequl(p2,O))return passign(result,p1);
	
	if(equl(p1->x,p2->x)&&equln(p1->y,p2->y,p)){
		return passign(result,O);	
	}
	
	FPOINT * x, * y, *lambda;
	
	x = newfpoint(0,0);
	y = newfpoint(0,0);
	lambda = (FPOINT *)malloc(sizeof(fpoint));
	
	if(equl(p1->x,p2->x)){
		fadd(fnmulti(fpower(p1->x,2,p,lambda),3,p,lambda),c->A,p,lambda);
		fmulti(lambda,inverse(fnmulti(p1->y,2,p,x),p,x),p,lambda);
	}else{
		fminus(p2->y,p1->y,p,lambda);
		fmulti(lambda,inverse(fminus(p2->x,p1->x,p,x),p,x),p,lambda);
	}
	
	fminus(fminus(fpower(lambda,2,p,x),p1->x,p,x),p2->x,p,x);
	fminus(fmulti(fminus(p1->x,x,p,y),lambda,p,y),p1->y,p,y);
	
	assign(result->x,x);
	assign(result->y,y); 
	
	free(lambda);
	free(x);
	free(y);
	
	return result;
}

POINT * minus(POINT * p1, POINT * p2, CURVE * c, int p, POINT * result)
{
	POINT * temp = newpoint(0,0,0,0);
	add(pneg(p2,p,temp),p1,c,p,result);
	
	freepoint(temp);
	
	return result;
}

//power of points addition
POINT * ppower(POINT * a, int n, CURVE * c, int p, POINT * result)
{
	POINT * DB;

	DB = newpoint(0,0,0,0);
	
	passign(DB,a);
	passign(result,O);
	
	while(n > 0){
		if(n&1)add(DB,result,c,p,result);
		add(DB,DB,c,p,DB);
		n >>= 1;
	}
	
	freepoint(DB);
	
	return result;
}

FPOINT * miller(POINT * a, POINT * b, CURVE * c, int p)
{
	FPOINT * result = (FPOINT *)malloc(sizeof(fpoint));
	
	//TODO
	
	return result;
}

int main()
{
	ONE = newfpoint(0,1);
	ZERO = newfpoint(0,0);
	O = newpoint(-1,-1,-1,-1);
	
	int p=5;
	FPOINT * test = newfpoint(0,3);
	FPOINT * test1 = newfpoint(0,7);
	
	CURVE * c = newcurve(newfpoint(0,0),newfpoint(0,1)); 
	
	POINT * P = newpoint(0,0,0,1);
	
	//add(P,P,c,p,P);
	
	ppower(P,100,c,p,P);
	
	//minus(P,P,c,p,P);

	//pneg(P,p,P);
	
	//passign(P,O);
	
	printf("%d\n",testpoint(P,c,p));
	
	showpoint(P);
	
	return 0;
}
