#include"elliptic.h"

FPOINT * newfpoint(lint x, lint y)
{
	FPOINT * result = (FPOINT *)malloc(sizeof(fpoint));
	result->x = x;
	result->y = y;
	return result;
}

POINT * newpoint(lint a, lint b, lint c, lint d)
{
	POINT * result = (POINT *)malloc(sizeof(point));
	result->x = newfpoint(a,b);
	result->y = newfpoint(c,d);
	return result;
}

CURVE * newcurve(lint A, lint B)
{
	CURVE * result = (CURVE *)malloc(sizeof(curve));
	result->A = newfpoint(0,A);
	result->B = newfpoint(0,B);
	
	return result;
}

void freepoint(POINT * a)
{
	free(a->x);
	free(a->y);
}

//absolute value modulo p
lint ABS(lint a, lint p)
{
	return (a>=0)?a%p:(p-(-a)%p)%p;
}

//extended euclid algorithm
lint gcdEx(lint a, lint b, lint *x, lint *y) 
{
    if(b==0){
        *x = 1,*y = 0;
        return a;
    }
    else{
        lint r = gcdEx(b, a%b, x, y);
        lint t = *x;
        *x = *y;
        *y = t - a/b * *y;
        return r;
    }
}

lint powermod(lint a, lint n, lint p)
{
	lint result, DB;
	
	result = 1;
	DB = a;
	
	while(n>0){
		if(n&1)result = (result*DB)%p;
		DB = (DB*DB)%p;
		n >>= 1;
	}
	
	return ABS(result,p);
}

//modulo inverse
lint inver(lint a, lint p)
{
	lint s,t;
	if(gcdEx(a,p,&s,&t)!=1)return 0;
	return ABS(s,p);
}

lint randomnonq(lint p)
{
	lint i, k = (p-1)>>1;
	for(i = 1;i < p;i++){
		if(powermod(i,k,p) != 1)break;
	}
	return i;
}

lint modsquareroot(lint a, lint p)
{
	if(powermod(a,(p-1)>>1,p) != 1)return -1;
	
	lint r = (p-1)>>1;
	lint b = randomnonq(p);
	lint x = r, y = 0;

	while(!(x&1)){
		x >>= 1; y >>= 1;
		if(ABS(powermod(a,x,p)*powermod(b,y,p),p) != 1)y += r;
	}
	
	return ABS(powermod(a,(x+1)>>1,p)*powermod(b,y>>1,p),p);
}

bool millerrabin(lint n, lint r)
{
	if(n <= 1)return false;
	
	lint s = 0, t = n - 1;
	
	while(!(t&1)){
		s++; t >>= 1;
	}
	while(r--){
		lint b = rand()%(n-1) + 2;
		lint r0 = powermod(b,t,n);
		lint s0 = s - 1;
		
		lint c = powermod(b,n-1,n); 
		
		if(r0 == 1 || r0 == n - 1)continue;
		if(s0 < 1)return false;
		while(s0--){
			r0 = powermod(r0,2,n);
			if(r0 == n-1)break;
			if(s0 == 0)return false;
		}
	}
	
	return true;
} 

lint largerandom(lint n)
{
	lint a = rand()%10000, b = rand()%10000;
	return (a+b*10000)%n;
}

lint randomgoodprime(lint n)
{
	lint p = 4;
	while(!millerrabin(p,10))p = 12*largerandom(n)+11;
	return p;
}

lint randonsafeprime(lint n)
{
	lint p;
	while(p = randomgoodprime(n)){
		if(millerrabin((p+1)/12,2))break;
	}
	return p;
}

//field element equlity judgement
bool equl(FPOINT * a, FPOINT * b)
{
	if(a->x == b->x && a->y == b->y)return true;
	return false;
}

bool equln(FPOINT * a, FPOINT * b, lint p)
{
	if(a->x == ABS(-b->x,p)&&a->y == ABS(-b->y,p))return true;
	return false;
}

bool pequl(POINT * a, POINT * b)
{
	return equl(a->x,b->x)&&equl(a->y,b->y);
}

void showelement(FPOINT * p)
{
	printf("(%lld,%lld)\n",p->x,p->y);
}

FPOINT * fneg(FPOINT * a, lint p, FPOINT * result)
{
	result->x = ABS(-a->x,p);
	result->y = ABS(-a->y,p);
	
	return result;
}

//field elements addition
FPOINT * fadd(FPOINT * a, FPOINT * b, lint p, FPOINT * result)
{
	lint x = ABS(a->x + b->x,p);
	lint y = ABS(a->y + b->y,p);
	
	result->x = x;
	result->y = y;
	
	return result;
}

//field elements minus
FPOINT * fminus(FPOINT * a, FPOINT * b, lint p, FPOINT * result)
{
	lint x = ABS(a->x - b->x,p);
	lint y = ABS(a->y - b->y,p);
	
	result->x = x;
	result->y = y;
	
	return result;
}

//field elements multiplication
FPOINT * fmulti(FPOINT * a, FPOINT * b, lint p, FPOINT * result)
{
	lint x = ABS(a->y*b->x + a->x*b->y,p);
 	lint y = ABS(a->y*b->y - a->x*b->x,p);
 	
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
FPOINT * fpower(FPOINT * a, lint n, lint p, FPOINT * result)
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
FPOINT * inverse(FPOINT * a, lint p, FPOINT * result)
{	
	if(a->x == 0){
		result->x = 0;
		result->y = inver(a->y,p);
		return result;
	}
	
	lint x = inver(ABS(-a->y*a->y*inver(a->x,p)-a->x,p),p);
	lint y = ABS(-a->y*inver(a->x,p)*x,p);
	
	result->x = x;
	result->y = y;
	
	return result;
}

//multiply by number
FPOINT * fnmulti(FPOINT * a, lint b, lint p, FPOINT * result)
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

bool testpoint(POINT * p, CURVE * c, lint p1)
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
	printf("\n[(%lld,%lld),(%lld,%lld)]\n",p->x->x,p->x->y,p->y->x,p->y->y);
}

POINT * pneg(POINT * a, lint p, POINT * result)
{
	if(pequl(a,O)){
		passign(result,O);
		return result;
	}
	assign(result->x,a->x);
	fneg(a->y,p,result->y);
	
	return result;
}

//curve polint additon
POINT * add(POINT * p1, POINT * p2, CURVE * c, lint p, POINT * result)
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

POINT * minus(POINT * p1, POINT * p2, CURVE * c, lint p, POINT * result)
{
	POINT * temp = newpoint(0,0,0,0);
	add(pneg(p2,p,temp),p1,c,p,result);
	
	freepoint(temp);
	
	return result;
}

//power of points addition
POINT * ppower(POINT * a, lint n, CURVE * c, lint p, POINT * result)
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

POINT * randompoint(CURVE * c, lint p)
{	
	POINT * result = newpoint(0,0,0,0);
	
	lint x, y;
	
	if((p-1)%3 && equl(c->A,ZERO)){
		y = largerandom(p);
		lint r = inver(3,p-1);
		
		x = powermod(y*y-1, r, p);
	}else{
		while(1){
			x = largerandom(p);
			y = ABS(powermod(x,3,p) + c->A->y*x + c->B->y,p);
			
			if((y=modsquareroot(y,p)) != -1)break;
		}
	}
	
	result->x = newfpoint(0,x);
	result->y = newfpoint(0,y);
	
	return result;
}

FPOINT * primitroot(lint p)
{
	lint x, y;
	
	y = inver(2,p);
	
	if((x = modsquareroot(-3,p)) != -1){
		y = ABS(x*y - y,p);
		return newfpoint(0,y);
	}
	
	x = modsquareroot(ABS(y*y - y + 1,p),p);
	
	return newfpoint(x,ABS(-y,p)); 
}

POINT * phi(POINT * a, lint p, POINT * result)
{	
	if(pequl(a,O)){
		passign(result,a);
		return result;
	}
	
	FPOINT * temp = primitroot(p);
	
	
	fmulti(a->x,temp,p,result->x);
	assign(result->y, a->y);
	
	free(temp);
	
	return result;
}

FPOINT * evalueline(POINT * a, POINT * b, POINT * in, lint p, CURVE * c, FPOINT * result)
{
	FPOINT * temp1, *temp2;
	
	temp1 = newfpoint(0,0); temp2 = newfpoint(0,0);
	
	if(pequl(a,b)){
		fadd(fnmulti(fpower(a->x,2,p,temp1),3,p,temp1),c->A,p,temp1);
		fminus(in->x,a->x,p,temp2);
		fmulti(temp1,temp2,p,result);
		fmulti(fnmulti(a->y,2,p,temp1),fminus(in->y,a->y,p,temp2),p,temp1);
		fminus(result,temp1,p,result);
		
	}else{
		fmulti(fminus(in->x,b->x,p,temp1),fminus(a->y,b->y,p,temp2),p,result);
		fmulti(fminus(a->x,b->x,p,temp1),fminus(in->y,b->y,p,temp2),p,temp1);
		fminus(result,temp1,p,result);
	}
	
	free(temp1); free(temp2);
	
	return result;
}

bool evaluelinedivi(POINT * a, POINT * b, POINT * in, CURVE * c, lint p, FPOINT * result)
{
	FPOINT * temp = newfpoint(0,0);
	POINT * tp = newpoint(0,0,0,0), * tp1 = newpoint(0,0,0,0);
	
	assign(result,ONE);
	add(a,b,c,p,tp);
	
	fmulti(result,evalueline(a,b,in,p,c,temp),p,result);
	if(equl(result,ZERO)){
		free(temp); freepoint(tp); freepoint(tp1);
		return false;
	}
	
	evalueline(tp,pneg(tp,p,tp1),in,p,c,temp);
	if(equl(temp,ZERO)){
		free(temp); freepoint(tp); freepoint(tp1);
		return false;
	}
	
	fmulti(inverse(temp,p,temp),result,p,result);
	free(temp); freepoint(tp); freepoint(tp1);
	
	return true;	
} 

bool miller(POINT * a, POINT * b, CURVE * c, lint m, lint p, FPOINT * f)
{
	FPOINT * temp = newfpoint(0,0);
	POINT * t = newpoint(0,0,0,0);
	
	assign(f,ONE);
	passign(t,a);
	
	lint i = 0, array[(int)logb((double)m)+1];
	
	while(m){
		if(m&1)array[i] = 1;
		else
			array[i] = 0;
		m >>= 1;
		i++;
	}
	
	for(lint j = i - 1;j > 1; j--){
		fmulti(f,f,p,f); 
		if(!evaluelinedivi(t,t,b,c,p,temp)){
			free(temp); freepoint(t);
			return false; 
		}
		fmulti(f,temp,p,f);
		add(t,t,c,p,t); // double point t
		if(array[i] == 1){
			if(!evaluelinedivi(t,a,b,c,p,temp)){
				free(temp); freepoint(t);
				return false; 
			}
			fmulti(f,temp,p,f);
			add(t,a,c,p,t);
		}
	}
	
	free(temp); freepoint(t);
	
	return true;
}

//the following function works only for supersingular elliptic curve
lint findorder(POINT * po, CURVE * c, lint p)
{
	POINT * t = newpoint(0,0,0,0);
	lint m = (p+1)/12;
	lint list[6] = {1,2,3,4,6,12};
	
	for(int i = 0;i < 12;i++){
		passign(t, po);
		if(i<6 && pequl(ppower(t,list[i],c,p,t),O)){
			freepoint(t);
			return list[i];
		}else if(pequl(ppower(t,m*list[i%6],c,p,t),O)){
			freepoint(t);
			return m*list[i%6];
		}
	}
	
	freepoint(t);
	
	return p+1;
}

bool weilpairing(POINT * a, POINT * b, CURVE * c, lint p, FPOINT * result)
{
	lint m = findorder(a,c,p);
	lint n = findorder(b,c,p);
	
	FPOINT * t1, * t2, * t3, *t4;
	t1 = newfpoint(0,0); t2 = newfpoint(0,0); t3 = newfpoint(0,0); t4 = newfpoint(0,0);
	
	
	if(n%m == 0)m = n;
	else if(m%n == 0)n = m;
	else
 		return false;
	POINT * S = newpoint(0,0,0,0), *temp = newpoint(0,0,0,0), *temp1 = newpoint(0,0,0,0), *temp2 = newpoint(0,0,0,0);
	
	while(true){
		freepoint(S);
		S = randompoint(c,p);
		
		if(!miller(a,add(S,b,c,p,temp),c,n,p,t1))continue;
		if(!miller(a,S,c,n,p,t2))continue;
		if(!miller(b,minus(a,S,c,p,temp),c,n,p,t3))continue;
		if(!miller(b,pneg(S,p,temp),c,m,p,t4))continue;
		
		assign(result,t1); fmulti(result,t4,p,result);
		fmulti(result,inverse(t2,p,t3),p,result);
		fmulti(result,inverse(t3,p,result),p,result);
		
		if(!evaluelinedivi(minus(a,S,c,p,temp),pneg(S,p,temp1),add(S,b,c,p,temp2),c,p,t1))continue;
		if(!evaluelinedivi(minus(a,S,c,p,temp),pneg(S,p,temp1),S,c,p,t2))continue;
		if(!evaluelinedivi(add(S,b,c,p,temp),S,minus(a,S,c,p,temp2),c,p,t3))continue;
		if(!evaluelinedivi(add(S,b,c,p,temp),S,pneg(S,p,temp2),c,p,t4))continue;

		fpower(t1,n,p,t1); fpower(t2,n,p,t2); fpower(t3,n,p,t3); fpower(t4,n,p,t4); 
		inverse(result,p,result);
		
		assign(result,t1); fmulti(result,t4,p,result);
		fmulti(result,inverse(t2,p,t3),p,result);
		fmulti(result,inverse(t3,p,result),p,result);		
		
		break;
	}
	
	free(t1); free(t2); free(t3); free(t4); 
	
	freepoint(S); freepoint(temp); freepoint(temp1); freepoint(temp2);
	
	return true;
}

void init()
{
	ONE = newfpoint(0,1);
	ZERO = newfpoint(0,0);
	O = newpoint(-1,-1,-1,-1);
	srand((int)time(0));
}

int main()
{
	init();
	lint p=46523;
	FPOINT * test = newfpoint(0,14);
	FPOINT * test1;
	
	CURVE * c = newcurve(0,1); 
	
	POINT * P1, * P2, * temp = newpoint(0,0,0,0);
	
	//add(P,P,c,p,P);
	
	P1 = newpoint(0,654,0,21925);
	P2 = newpoint(0,12416,0,39871);
	
	showelement(primitroot(p));
	
	//phi(P2,p,P2);
	//phi(P1,p,P1);
	
	
	showpoint(P1);
	showpoint(P2);
	
	ppower(P1,2,c,p,P2);
	
	if(weilpairing(P1,P2,c,p,test))showelement(test);
	else
		printf("fail!\n");
	
	printf("%d\n",testpoint(P1,c,p));
	printf("%d\n",testpoint(P2,c,p));
	
	//pneg(P,p,P);
	
	//passign(P,O);
	
	return 0;
}
