/*
Grammar as follows:

statement := 
	| expression EOF
	| VAR variable '=' expression EOF
	| DEL variable EOF
.

expression :=
	| term
	| term '+' expression
	| term '-' expression
	| variable '=' expression
.

term := 
	| atom
	| atom '*' term
	| atom '/' term
.

atom :=
	| KEY '(' tuple ')'
	| variable
	| constant
	| '(' expression ')'
.

tuple := 
	| expression ',' tuple
	| expression
.

variable :=
	| [not spacial char]*
.

constant := 
	| [0..9]*
.

EOF := 
	| '\0'
	| ';'

*/
#include"elliptic.h"
#include<string.h>


#define MAX_VAR 100000
#define MAX_STR 1000


lint global_p = 23;

CURVE * global_curve;

char * TYPE[5] = {"num","felement","point","curve","constant"};
char * KEY[10] = {"randomprime","modsqrt","modpower","select","mod","curve","remove","var"};

typedef struct variable{
	char * name;
	int type,tag;
	lint value;
	FPOINT * fvalue;
	POINT * pvalue;
	CURVE * c;
}VAR;

typedef struct valu{
	int type;
	lint value;
	FPOINT * fvalue;
	POINT * pvalue;
	CURVE * c;
}VALU;

typedef struct toke{
	int type;
	char * vaule,c;
}TOKEN;

char specialchar[32] = ".,\\{}[]()+-*/%$;@#!?`~<>:'|=^&\"";

int CURRENT_VAR_NUM = 0;

VAR * list[MAX_VAR] = { NULL };

TOKEN * tokenlist[MAX_VAR] = { NULL };

int tokenlen = 0;

void statement(int header);

VALU * expression(int * start);

VALU * term(int * start);

VALU * tuple(int * start, char * key);

VALU * bracket(int * start);

int MIN(int a,int b)
{
	return (a>b)?b:a;
}

int MAX(int a, int b)
{
	return (a>b)?a:b;
}

bool strcamp(char * a, char * b)
{
	int i = 0;
	if(a==NULL || b==NULL)return false;
	
	while(i < MAX_STR){
		if(a[i] != b[i])return false;
		if(a[i] == '\0' && b[i] == '\0')return true;
		i++;
	}
	return false;
}

char * substring(char * a, int start, int end)
{
	int i, len = strlen(a);
	
	end = MIN(end,len);
	
	if(end <= start)return NULL; 
	
	char * result = (char *)malloc((end-start)+2);
	
	for(i = start; i < MIN(len,end); i++){
		result[i-start] = a[i];
	}
	
	result[i-start] = '\0';
	
	return result;
}



VAR * findvariable(char * name)
{
	for(int i = 0;i < CURRENT_VAR_NUM; i++){
		if(list[i]->tag== 1 && strcamp(name,list[i]->name))return list[i];
	}
	return NULL;
}


bool checknum(char c)
{
	if(c >= '0' && c <= '9')return true;
	return false;
}

bool checkint(char * num)
{
	if(num == NULL)return false;
	int len = strlen(num);
	while(--len>=0){
		if(!checknum(num[len]))return false;
	}
	return true;
}

lint parseint(char * num)
{
	lint len = strlen(num);
	lint result = 0;
	
	if(len == 0 || len > 20)return 0;
	
	if(len < 0)return 0;
	
	int i = 0;
	
	result += num[i] - '0';
	
	while(++i < len){
		result *= 10;
		if(num[i] != '0')result += num[i] - '0';	
	}
	
	return result;
}

int findavailablevariable()
{
	int i;
	for(i = 0; i < CURRENT_VAR_NUM; i++){
		if(list[i] != NULL && list[i]->tag == 0)break;
	}
	return i;
}

char * copystring(char * a)
{
	int len = strlen(a);
	char * result = (char *)malloc(len+1);
	result[len] = '\0';
	while(--len >= 0)result[len] = a[len];
	
	return result;
}

VAR * newvariable()
{
	VAR * result = (VAR *)malloc(sizeof(variable));
	result->name = NULL;
	result->type = 0;
	result->value = 0;
	result->tag = 0;
	result->fvalue = newfpoint(0,0);
	result->pvalue = newpoint(0,0,0,0);
	result->c = newcurve(0,0);
	return result;
}

VALU * newvalue(int type,lint value,FPOINT * fvalue, POINT * pvalue, CURVE * c)
{
	VALU * result = (VALU *)malloc(sizeof(valu));
	
	result->fvalue = newfpoint(0,0);
	result->pvalue = newpoint(0,0,0,0);
	result->c = newcurve(0,0);
	
	result->type = type;
	result->value = value;
	
	if(fvalue)assign(result->fvalue,fvalue);
	if(pvalue)passign(result->pvalue,pvalue);
	if(c)curveassign(result->c,c);
	
	return result;
}

bool putvariable(int type, char * name,lint value, FPOINT * fvalue, POINT * pvalue, CURVE * c)
{	
	if(name == NULL || findvariable(name))return false;
	
	int i = findavailablevariable();

	if(i > MAX_VAR){
		printf("Maximum varibale size excced.\n");
		return false;
	}

	if(list[i] == NULL)list[i] = newvariable();
	
	VAR * cu = list[i];
	
	cu->tag = 1;
	cu->type = type;
	
	if(cu->name != NULL)free(cu->name);

	cu->name = copystring(name);
	
	switch(type){
		case 0:
			cu->value = value;
			break;
		case 1:
			if(fvalue)assign(cu->fvalue,fvalue);
			break;
		case 2:
			if(pvalue)passign(cu->pvalue,pvalue); 
			break;
		case 3:
			if(c)curveassign(cu->c,c);
		default:
			break;
	}
	
	CURRENT_VAR_NUM = MAX(CURRENT_VAR_NUM, i+1);
	return true;
}

bool deletvariable(char * name)
{
	VAR * le = findvariable(name);
	
	if(le == NULL)return false;
	
	le->tag = 0;
	
	return true;
}

int checkkey(char * name)
{
	int i;
	for(i = 0; i < 10; i++){
		if(strcamp(name,KEY[i]))break;
	}
	return i;
}

bool transparentchar(char c)
{
	if(c == ' ' || c == '\t' || c == '\r' || c == '\n')return true;
	return false;
}

bool isspacialchar(char c)
{	
	if(transparentchar(c))return true;
	for(int i = 0;i < 32;i++){
		if(c == specialchar[i])return true;
	} 
	return false;
}

int moveahead(char * input, int curren_head, int len)
{
	//int len = strlen(input);
	
	if(curren_head >= len)return len;
	
	while(curren_head < len && transparentchar(input[curren_head++]));
	
	return curren_head - 1;
}

char * nextliteral(char * input, int curren_head, int len ,int * pos)
{	
	//int len = strlen(input);
	
	int start = moveahead(input,curren_head,len);
	
	curren_head = start;
	
	while(curren_head <= len && !isspacialchar(input[curren_head++]));
	
	*pos = curren_head - 1;
	
	return substring(input,start,curren_head - 1);
}

int nexteof(char * input, int curren_head)
{
	while(input[curren_head] !=';' && input[curren_head++] !='\0');
	return curren_head;
}

int nextchar(char * input, char c, int current_head, int len)
{
	//int len = strlen(input);
	
	while(current_head < len && input[current_head++] != c);
	
	if(input[current_head - 1] == c)return current_head - 1;
	
	return len;
}

bool iszero(FPOINT * a)
{
	if(a->x != 0)return false;
	if(a->y != 0)return false;
	return true;
}

void printcurve(CURVE * c)
{
	printf("y^2 = x^3");
	
	if(!iszero(c->A)){
		if(c->A->x == 0){
			if(c->A->y > 0 && c->A->y != 1)printf(" + %lldx",c->A->y);
			else if(c->A->y>0 && c->A->y == 1)printf(" + x");
			else if(c->A->y != -1)printf(" - %lldx",c->A->y);
			else
				printf(" - x");
		}
		else
			printf(" + (%lld,%lld)*x",c->A->x,c->A->y);
	}
	if(!iszero(c->B)){
		if(c->B->x == 0){
			if(c->B->y > 0)printf(" + %lld",c->B->y);
			else
				printf(" - %lld",c->B->y);
		}
		else
			printf(" + (%lld,%lld)",c->B->x,c->B->y);
	}
	printf("\n");
}

void showvalue(VALU * v)
{
	if(v == NULL)return;
	switch(v->type){
		case 0:
			printf("%lld\n",v->value);
			break;
		case 1:
			printf("(%lld,%lld)\n",v->fvalue->x,v->fvalue->y);
			break;
		case 2:
			printf("[(%lld,%lld),(%lld,%lld)]\n",v->pvalue->x->x,v->pvalue->x->y,v->pvalue->y->x,v->pvalue->y->y);
			break;
		case 3:
			printcurve(v->c);
			break;
		default:
			printf("unknow type.\n");
	}
}


void showvar(VAR * v)
{
	if(v==NULL){
		printf("(null)\n");
		return;
	}
	printf("Name: %s\n",v->name);
	printf("Type: %s\n",TYPE[v->type]);
	printf("Value: ");
	if(v == NULL)return;
	switch(v->type){
		case 0:
			printf("%lld\n",v->value);
			break;
		case 1:
			printf("(%lld,%lld)\n",v->fvalue->x,v->fvalue->y);
			break;
		case 2:
			printf("[(%lld,%lld),(%lld,%lld)]\n",v->pvalue->x->x,v->pvalue->x->y,v->pvalue->y->x,v->pvalue->y->y);
			break;
		case 3:
			printcurve(v->c);
			break;
		default:
			printf("unknow type.\n");
	}
	printf("\n");
}

void freecurve(CURVE * c);

void freevalu(VALU * a)
{
	if(a == NULL)return;
	
	if(a->fvalue != NULL)free(a->fvalue);
	if(a->c != NULL)freecurve(a->c);
	if(a->pvalue != NULL)freepoint(a->pvalue);
	
	free(a);
}

void freecurve(CURVE * c)
{
	if(c->A)free(c->A);
	if(c->B)free(c->B);
	free(c);
}

bool assignvariable(VALU * val, char * name)
{
	if(strcamp(name,"global_p")){
		if(val->type == 0){
			if(millerrabin(val->value,10)){
				global_p = val->value; 
			}else
				printf("Warning! Can't assign value %lld to 'global_p', since it's not a prime.\n",val->value);
		}else
			printf("Warning! Can't assign type '%s' to constan 'global_p' which expects type 'num'.\n",TYPE[val->type]);
		return true;
	}
	
	if(strcamp(name,"global_curve")){
		if(val->type == 3){
			curveassign(global_curve,val->c);
		}else
			printf("Warning! Can't assign type '%s' to 'global_curve' which expects type 'curve'.\n");
			return true; 
	}
	
	VAR * current = findvariable(name);
	if(current == NULL){
		return putvariable(val->type,name,val->value,val->fvalue,val->pvalue,val->c);
	}
	else{
		current->type = val->type;
		current->value = val->value;
		if(val->fvalue)assign(current->fvalue,val->fvalue);
		if(val->pvalue)passign(current->pvalue, val->pvalue);
		if(val->c)curveassign(current->c,val->c); 
	}
	return true;
}

bool checkchar(TOKEN * t, char c)
{
	if(t->type == 0 && t->c == c)return true;
	return false;	
}

void showcons(char * name)
{
	if(name == NULL)return;
	if(strcamp(name,"global_p")){
		printf("Name: %s\n",name);
		printf("Type: constant\n");
		printf("Value: %lld\n",global_p);
	}
	if(strcamp(name, "global_curve")){
		printf("Name: %s\n",name);
		printf("Type: constant\n");
		printcurve(global_curve);
	}
}

void statement(int header)
{
	VALU * newvar;
	
	if(header >= tokenlen)return;
	
	if(	checkchar(tokenlist[header],';')){
		statement(header+1);
		return;
	}
	
	if(tokenlist[header]->type == 1 && strcamp(tokenlist[header]->vaule,"var")){
		while(++header < tokenlen){
			if(checkchar(tokenlist[header],';')){
				statement(header+1);
				return;
			}else if(tokenlist[header]->type == 0){
				printf("Syntax Error!\n");
				return;
			}if(checknum(tokenlist[header]->vaule[0])){
				printf("Variable can't start with number.\n");
				return;
			}
			putvariable(0,tokenlist[header]->vaule,0,NULL,NULL,NULL);
		}
		return;
	}
	
	if(tokenlist[header]->type == 1 && strcamp(tokenlist[header]->vaule,"del")){
		while(++header < tokenlen){
			if(checkchar(tokenlist[header],';')){
				statement(header+1);
				return;
			}else if(tokenlist[header]->type == 0){
				printf("Syntax Error!\n");
				return;
			}
			if(deletvariable(tokenlist[header]->vaule)){
				printf("'%s' has been deleted.\n",tokenlist[header]->vaule);
			}else
				printf("Varibale '%s' do not exist.\n", tokenlist[header]->vaule);
		}
		return;
	}
	
	if(tokenlist[header]->type == 1 && strcamp(tokenlist[header]->vaule,"check")){
		while(++header < tokenlen){
			if(tokenlist[header]->type == 1){
				if(strcamp(tokenlist[header]->vaule,"global_p") || strcamp(tokenlist[header]->vaule,"global_curve")){
					showcons(tokenlist[header]->vaule);
					return;
				}
				VAR * get = findvariable(tokenlist[header]->vaule);
				if(get == NULL){
					printf("Can't find variable '%s'.\n",tokenlist[header]->vaule);
					return;
				}
				showvar(get);
			}else
				break;
		}
		if(header < tokenlen && checkchar(tokenlist[header],';')){
			statement(header+1);
			return;
		}
		if(header < tokenlen){
			printf("Syntax Error!.\n");
			return;
		}
	}
	
	newvar = expression(&header);

	if(newvar == NULL)return;
	
	showvalue(newvar);

	freevalu(newvar);
	
	statement(header);
}

bool addv(VALU * a, VALU * b, VALU * result)
{
	if(a->type == 0 && b->type == 0){
		result->type = 0; result->value = a->value + b->value;
		return true;
	}
	if(a->type == 1 && b->type == 1){
		result->type = 1; fadd(a->fvalue,b->fvalue,global_p,result->fvalue);
		return true;
	}
	if(a->type == 2 && b->type == 2 && testpoint(a->pvalue,global_curve,global_p) && testpoint(b->pvalue,global_curve,global_p)){
		result->type = 2; add(a->pvalue,b->pvalue,global_curve,global_p,result->pvalue);
		return true;
	}
	if(a->type == 0 && b->type == 1){
		result->type = 1; result->fvalue->x = b->fvalue->x; result->fvalue->y = ABS(b->fvalue->y+a->value,global_p);
		return true;
	}
	if(a->type == 1 && b->type == 0)return addv(b,a,result);
	return false;
}

bool minuv(VALU * a, VALU * b, VALU * result)
{
	if(a->type == 0 && b->type == 0){
		result->type = 0; result->value = a->value - b->value;
		return true;
	}
	if(a->type == 1 && b->type == 1){
		result->type = 1; fminus(a->fvalue,b->fvalue,global_p,result->fvalue);
		return true;
	}
	if(a->type == 2 && b->type == 2){
		result->type = 2; minus(a->pvalue,b->pvalue,global_curve,global_p,result->pvalue);
		return true;
	}
	if(a->type == 0 && b->type == 1){
		result->type = 1; result->fvalue->x = ABS(-b->fvalue->x,global_p); result->fvalue->y = ABS(a->value-b->fvalue->y,global_p);
		return true;
	}
	if(a->type == 1 && b->type == 0){
		result->type = 1; result->fvalue->x = b->fvalue->x; result->fvalue->y = ABS(b->fvalue->y-a->value,global_p);
		return true;
	}
	return false;
}

bool multiv(VALU * a,VALU * b, VALU * result)
{
	if(a->type == 0 && b->type == 0){
		result->type = 0;result->value = a->value * b->value;
		return true;
	}
	if(a->type == 1 && b->type == 1){
		result->type = 1; fmulti(a->fvalue,b->fvalue,global_p,result->fvalue);
		return true;
	}
	if(a->type == 0 && b->type == 1){
		result->type = 1; fnmulti(b->fvalue,a->value,global_p,result->fvalue);
		return true;
	}
	if(a->type == 1 && b->type == 0)return multiv(b,a,result);
	if(a->type == 0 && b->type == 2){
		result->type = 2; ppower(b->pvalue,a->value,global_curve,global_p,result->pvalue);
		return true;
	}
	if(a->type == 2 && b->type == 0)return multiv(b,a,result);
	return false;
	
}

bool diviv(VALU * a,VALU * b, VALU * result)
{
	FPOINT * temp = newfpoint(0,0);
	
	if(a->type == 0 && b->type == 0){
		result->type = 0; a->value = a->value / b->value;
		return true;
	}
	if(a->type == 1 && b->type == 1){
		result->type = 1; fmulti(a->fvalue,inverse(b->fvalue,global_p,temp),global_p,result->fvalue);
		return true;
	}
	return false;
}

lint POW(lint a,lint b)
{
	lint t = a;
	while(--b>0){
		t *= a;
	}
	return t;
}

bool powv(VALU * a, VALU * b,VALU *result)
{
	if(b->type != 0)return false;
	
	if(a->type == 0){
		result->type = 0; result->value = POW(a->value,b->value);
		return true;
	}
	if(a->type == 1){
		result->type = 1;if(result->fvalue && b->fvalue)fpower(a->fvalue,b->value,global_p,result->fvalue);
		return true;
	}
	return false;
}

VALU * expression(int * start)
{
	if(*start >= tokenlen || checkchar(tokenlist[*start],';')){
		*start += 1;
		return NULL;
	}
	
	VALU * result = NULL;
	
	//when the first token be  a literal
	if(tokenlist[*start]->type == 1){
		char * name = tokenlist[*start]->vaule;
		if(*start + 1 < tokenlen && checkchar(tokenlist[*start+1],'=')){ // assign expression
			if(checknum(name[0])){
				printf("Can't assign value to '%s', it's not a valid variable name.\n",name);
				return NULL;
			}
			*start += 2;
			result = expression(start);
			if(result != NULL){
				assignvariable(result,name);
			}
			return result;
		}
	}
	
	result = term(start);
	
	if(result == NULL)return NULL;
	
	if(*start >= tokenlen || checkchar(tokenlist[*start],';') || checkchar(tokenlist[*start],')') || checkchar(tokenlist[*start],',') || checkchar(tokenlist[*start],']')){ // end of expression
		*start += 1;
		return result;
	}
L3:	if(checkchar(tokenlist[*start],'+')){ // if add
		*start += 1;
		VALU * temp = expression(start);
		
		if(temp == NULL || result == NULL){
			if(result)freevalu(result);
			if(temp)freevalu(temp); 
			result = NULL;
			return NULL;
		}
		
		int ty = result->type;
		
		if(!addv(result,temp,result)){
			if(ty == temp->type && ty == 2)printf("One of/both the points are not in the current curve.\n");
			else printf("Type '%s' and Type '%s' can not be added.\n",TYPE[temp->type],TYPE[ty]);
			freevalu(result);
			result = NULL;
		}
		freevalu(temp);
		return result;
	}
L4:	if(checkchar(tokenlist[*start],'-')){ // if minus
		*start += 1;
		VALU * temp = term(start);
		
		if(temp == NULL || result == NULL){
			if(result)freevalu(result);
			if(temp)freevalu(temp);
			result = NULL;
			return NULL;
		}
		
		int ty = result->type;
		
		if(!minuv(result,temp,result)){
			printf("Type '%s' can not minus Type '%s'.\n",TYPE[temp->type],TYPE[ty]);
			freevalu(result);
			result = NULL;
		}
		if(*start >= tokenlen || checkchar(tokenlist[*start],';') || checkchar(tokenlist[*start],')') || checkchar(tokenlist[*start],',') || checkchar(tokenlist[*start],']')){ // end of expression
			*start += 1;
			freevalu(temp);
			return result;
		}
		if(checkchar(tokenlist[*start],'+'))goto L3;
		if(checkchar(tokenlist[*start],'-'))goto L4;
	}
	
	printf("Syntax Error! Invalid expression.\n");
	
	return NULL;
}

bool constructpointvalue(VALU * a, VALU * b,VALU * result)
{
	if(a->type == 0 && b->type == 0){
		result->type = 2; result->pvalue->x->x = 0; result->pvalue->x->y = a->value;
		result->pvalue->y->x = 0; result->pvalue->y->y = b->value;
		return true;
	}
	if(a->type == 1 && b->type == 1){
		result->type = 2; assign(result->pvalue->x,a->fvalue);
		assign(result->pvalue->y,b->fvalue);
		return true;
	}
	if(a->type == 0 && b->type == 1){
		result->type = 2; assign(result->pvalue->y,b->fvalue);
		result->pvalue->x->x = 0; result->pvalue->x->y = a->value;
		return true;
	}
	if(a->type == 1 && b->type == 0){
		result->type = 2; assign(result->pvalue->x,a->fvalue);
		result->pvalue->y->x = 0; result->pvalue->y->y = b->value;
		return true;
	}
	return false;
}

VALU * atom(int *start)
{
	if(*start >= tokenlen || checkchar(tokenlist[*start],';'))return NULL;
	
	VALU * result;
	
	if(tokenlist[*start]->type == 0){
		if(tokenlist[*start]->c == '('){
			if(*start + 4 < tokenlen && checkchar(tokenlist[*start + 2],',') && checkchar(tokenlist[*start+4],')')){
				char * name1 = tokenlist[*start+1]->vaule, *name2 = tokenlist[*start + 3]->vaule;
				
				if(name1 == NULL || name2 == NULL){
					printf("Syntax Error! Invalid charactor.\n");
					return NULL;
				}
				
				if(checkint(name1) && checkint(name2)){
					FPOINT * temp = newfpoint(ABS(parseint(name1),global_p),ABS(parseint(name2),global_p));
					result = newvalue(1,0,temp,NULL,NULL);
					*start += 5;
					free(temp);
				}else{
					printf("Syntax Error! Can't parse '(%s,%s)' to be an field element.\n",name1,name2);
					return NULL;
				}
			}else{
				*start += 1;
				result = expression(start);
			}
		}else if(tokenlist[*start]->c == '['){
			*start += 1;
			result = expression(start);
			if(result == NULL)return NULL;
			if(!checkchar(tokenlist[(*start) - 1],',')){
				printf("Syntax Error! Expect two parameter for points.\n");
				return NULL;
			}
			
			int ty = result->type;
			
			VALU * temp = expression(start);
			if(temp == NULL)return NULL;
			if(!constructpointvalue(result,temp,result)){
				printf("Can't paramize type '%s' and '%s' to be a point.\n",TYPE[ty],TYPE[temp->type]);
				if(temp)freevalu(temp);
				if(result)freevalu(result);
				return NULL;
			}
			if(!testpoint(result->pvalue,global_curve,global_p))printf("Warning! The point is not on the current cure.\n");
			freevalu(temp);
		}else{
			printf("Syntax Error! Unexpected charactor '%c'.\n",tokenlist[*start]->c);
			return NULL;
		}
	}else{
		char * name = tokenlist[*start]->vaule;
		*start += 1;
		
		if(*start < tokenlen && checkchar(tokenlist[*start],'(')){
			*start += 1;	
			result = tuple(start,name);
		}else{
			if(checkint(name)){
				lint t = parseint(name);
				result = newvalue(0,t,NULL,NULL,NULL);
			}else if(strcamp(name, "global_p")){
				result = newvalue(0,global_p,NULL,NULL,NULL);
			}else if(strcamp(name,"global_curve")){
				result = newvalue(3,0,NULL,NULL,global_curve);
			}else{
				VAR * v = findvariable(name);
				
				if(!v){
					printf("'%s' has not been defined.\n",name);
					return NULL;
				}
				result = newvalue(v->type,v->value,v->fvalue,v->pvalue,v->c);
			}
		}
	}
	return result;
} 

VALU * term(int * start)
{
	if(*start >= tokenlen || checkchar(tokenlist[*start],';'))return NULL;

	VALU * result = NULL;
	
	result = atom(start);
	
	if(result == NULL)return result;
	
	if(*start < tokenlen && checkchar(tokenlist[*start],'^')){
		*start += 1;
		VALU * temp = atom(start);
		
		int tp = result->type; 
		
		if(temp == NULL || result == NULL){
			if(result)freevalu(result);
			if(temp)freevalu(temp);
			return NULL;
		}
		
		if(!powv(result,temp,result)){
			printf("Type '%s' can not be the power of type '%s'.\n", TYPE[temp->type],TYPE[tp]);
			if(result)freevalu(result);
			if(temp)freevalu(temp);
			return NULL;
		}
		if(temp)freevalu(temp);
	}

	if(result == NULL)return NULL;
	
	int ty = result->type;
	
	if(*start >= tokenlen || checkchar(tokenlist[*start],';') || checkchar(tokenlist[*start],')') || checkchar(tokenlist[*start],',') || checkchar(tokenlist[*start],'+') || checkchar(tokenlist[*start],'-') || checkchar(tokenlist[*start],']')){ // end of expression
		return result;
	}
L1:		
	if(checkchar(tokenlist[*start],'*')){
		*start += 1;
		
		VALU * temp = term(start);
				
		if(temp == NULL || result == NULL){
			if(result)freevalu(result);
			if(temp)freevalu(temp);
			return NULL;
		} 
				
		if(!multiv(result,temp,result)){
			printf("Type '%s' and Type '%s' can not be multiplied.\n",TYPE[temp->type],TYPE[ty]);
			freevalu(result);
			result = NULL;
		}
		freevalu(temp);
		return result;
	}
L2:
	if(checkchar(tokenlist[*start],'/')){
		*start += 1;
		
		VALU * temp = atom(start);
		
		if(temp == NULL || result == NULL){
			if(result)freevalu(result);
			if(temp)freevalu(temp);
			return NULL;
		}
		if(!diviv(result,temp,result)){
			printf("Type '%s' can not divide Type '%s'.\n",TYPE[temp->type],TYPE[ty]);
			freevalu(result);
			return NULL;
		}
		if(*start >= tokenlen || checkchar(tokenlist[*start],';') || checkchar(tokenlist[*start],')') || checkchar(tokenlist[*start],',') || checkchar(tokenlist[*start],'+') || checkchar(tokenlist[*start],'-') || checkchar(tokenlist[*start],']')){ // end of expression
			freevalu(temp);
			return result; 
		}
		if(checkchar(tokenlist[*start],'*'))goto L1;
		if(checkchar(tokenlist[*start],'/'))goto L2;
	}	
	printf("Syntax Error! Invalid term.\n");
	return NULL;
}

bool sqrtv(VALU * a, VALU * result)
{
	if(a->type == 0){
		result->value = (lint)sqrt((double)a->value);
		return true;
	}
	if(a->type == 1 && a->fvalue->x == 0){
		result->fvalue->x = 0; result->fvalue->y = modsquareroot(a->fvalue->y,global_p);
		return true;
	}
	return false;
}

VALU * duplicate(int *start, char * name)
{
	VALU * result;
	result = expression(start);
	if(result == NULL){
		printf("Too few parameter for function '%s()', expected three parameters.\n",name);
		return NULL;
	}
	VALU * temp = expression(start);
	if(temp == NULL){
		printf("Too few parameter for function '%s()', expected two more parameter.\n",name);
		return NULL;
	}
	VALU * temp1 = expression(start);
	if(temp1 == NULL){
		printf("Too few parameter for function '%s()', expected one more parameter.\n",name);
		return NULL;
	}
	if(result->type == 2 && temp->type == 2 && temp1->type == 0){
		if(!testpoint(result->pvalue,global_curve,global_p)|| !testpoint(temp->pvalue,global_curve,global_p)){
			printf("One / both points are not on the curve.\n");
			return NULL;
		}
		FPOINT * f = newfpoint(0,0);
		if(strcamp(name,"miller") && !miller(result->pvalue,temp->pvalue,global_curve,global_p,temp1->value,f)){
			printf("Error can't compute 'miller()' for these two points.\n");
			return NULL;
		}
		if(strcamp(name,"pair") && !weilpairing(result->pvalue,temp->pvalue,global_curve,global_p,temp1->value,f)){
			printf("Error can't compute 'pair()' for these two points.\n");
			return NULL;
		}
		freevalu(temp1);freevalu(temp);freevalu(result);
		result = newvalue(1,0,f,NULL,NULL);
		if(f)free(f);
		return result;
	}
	printf("Parameter type error for function '%s()'.\n",name);
	return NULL;
}

bool gencurve(VALU * a,VALU * b,VALU * result)
{
	if(a->type == 0 && b->type == 0){
		result->c->A->x = 0;result->c->A->y = a->value;
		result->c->B->x = 0;result->c->B->y = b->value;
		result->type = 3;
		return true;
	}
	if(a->type == 1 && b->type == 1){
		assign(result->c->A,a->fvalue);assign(result->c->B,b->fvalue);
		result->type = 3;
		return true;
	}
	if(a->type == 0 && b->type == 1){
		result->c->A->x = 0;result->c->A->y = a->value;
		assign(result->c->B,b->fvalue);
		result->type = 3;
		return true;
	}
	if(a->type == 1 && b->type == 0){
		assign(result->c->A,a->fvalue);
		result->c->B->x = 0; result->c->B->y = b->value;
		result->type = 3;
		return true;
	}
	return false;
}

VALU * tuple(int *start, char * key)
{
	VALU * result;
	
	if(*start >= tokenlen || checkchar(tokenlist[*start],';'))return NULL;
	
	if(strcamp(key,"Randomprime")){
		
		result = expression(start);
		
		if(result == NULL)return NULL;
		
		if(result->type == 0){
			result->value = randonsafeprime(result->value);
		}else{
			printf("Wrong parameter type for Randomprime().\n");
			return NULL;
		}
		return result;
	}
	if(strcamp(key,"PrimeQ")){
		result = expression(start);
		VALU * temp;
		int t = *start - 1;
		if(checkchar(tokenlist[t],',')){
			temp = expression(start);
			if(temp == NULL || result == NULL)return NULL;
			
			if(result->type == 0 && temp->type == 0){
				result->value = (int)millerrabin(result->value,temp->value);
				return result;
			}
			printf("Wrong parameter type for PrimeQ().\n");
			return NULL;
		}
		
		if(result == NULL)return NULL;
		
		if(result->type == 0){
			result->value = (int)millerrabin(result->value,10);
			return result;
		}
		printf("Wrong parameter type for PrimeQ().\n");
		return NULL;
	}
	if(strcamp(key,"sqrt")){
		result = expression(start);
		
		if(result == NULL)return NULL;
		
		if(result->type == 0 || result->type == 1){
			if(!sqrtv(result,result)){
				printf("Invalid type or value for function 'sqrt'.\n");
				return NULL;
			}
		}else{
			printf("Wrong parameter type for sqrt().\n");
			return NULL;
		}
		return result;
	}
	if(strcamp(key,"Randompoint")){
		*start += 1;
		result = newvalue(2,0,NULL,randompoint(global_curve,global_p),NULL);
		return result;
	}
	if(strcamp(key,"ord")){
		result = expression(start);
		if(result == NULL)return NULL;
		if(result->type != 2){
			printf("Invalid paramenter type for 'ord()'.\n");
			return NULL;
		}
		VALU * temp = result;
		if(!testpoint(temp->pvalue,global_curve,global_p)){
			printf("Can't find the order for a point not in the curve.\n");
			return NULL;
		}
		result = newvalue(0,findorder(temp->pvalue,global_curve,global_p),NULL,NULL,NULL);
		free(temp);
		return result;
	}
	if(strcamp(key,"mill") || strcamp(key,"miller")){
		return duplicate(start,key);
	}
	
	if(strcamp(key,"phi")){
		result = expression(start);
		if(result == NULL){
			printf("No parameter for function 'phi()'.\n");
			return NULL;
		}
		if(result->type != 2){
			printf("Parameter type error for function 'phi'.\n");
			return NULL;
		}
		phi(result->pvalue,global_p,result->pvalue);
		
		return result;
	}
	if(strcamp(key,"curve") || strcamp(key,"cu")){
		result = expression(start);
		if(result == NULL){
			printf("You need two parameter to generate a curve.\n");
			return NULL;
		}
		VALU * temp = expression(start);
		if(temp == NULL){
			return NULL;
		}
		if(temp->type <= 1 && result->type <= 1){
			gencurve(result,temp,result);
			freevalu(temp);
			return result;
		}
		printf("Parameter type error for function 'curve'");
	} 
	if(strcamp(key,"pair")){
		return duplicate(start,key);
	}
	if(strcamp(key,"gcd")){
		result = expression(start);
		if(result == NULL)return NULL;
		
		VALU * temp = expression(start);
		if(start == NULL)return NULL;
		lint a,b;
		if(result->type == 0, temp->type == 0){
			result->value = gcdEx(result->value,temp->value,&a,&b);
			return result;
		} 
		printf("Type error for 'gcd()'.\n");
		return NULL;
	}
	
	return NULL;
}

void Sleep(int n)
{
	while(n > 0){
		millerrabin(n,10);
		n--;
	}
}

void slowshow(char * in)
{
	int len = strlen(in);
	int i = 0;
	while(i < len){
		printf("%c",in[i]);
		Sleep(10000);
		i++;
	}
}

void lex(char * input);

void demo()
{
	system("cls");
	char dem[2000] = "We first build two empty variables named 'n' and 'm'\n;var n m;\
	 \nthen assign vlaues to them\n;n = 120;\n;m = 200;\n\
	 \nCompute m+n\n;m + n;\n\
	 \nGenerate a random prime with lenth m and put it to vairable a\n;a = Randomprime(m);\n\
	 \nSet global prime\n;global_p = a;\n\
	 \nEvaluabte an math expression\n;a^2 + m*(n - 12*n)/a +(12*3)*(12+45+34);\n\
	 \nGenerate an random point and put it to variable p\n;p = Randompoint();\n\
	 \nAnother point q\n;q = Randompoint();\n\
	 \nCompute point addtion p + q\n;p + q;\n\
	 \nCompute their order\n;ord(p);\n;ord(q);\n\
	 \nUse function phi\n;p1 = phi(p);\n\
	 \nIt's order is\n;r = ord(p1);\n\
	 \nWe can see that they are the same.\n\
	 \nCompute weil paring\n;f = pair(p,p1,r);\n\
	 \nVarify bilinearity\n;f1 = pair(3*p,p1,r);\n\
	 \nand f^3 = f1\n;f^3 - f1;\n\
	 \n;f2 = pair(3*p,5*p1,r);\n\
	 \n;f^15 - f2;\n\
	 \nWe now demostrate how pairing can be used in cryptograph\n\
	 \nSuppose Alice, Bob Chalrse wants to share a secret key 'K', they firstly choose three random number sepreately\n\
	 \n;A = Randomprime(100);\n\
	 \n;B = Randomprime(100);\n\
	 \n;C = Randomprime(100);\n\
	 \nThey post the following public keys\n\
	 \n;Ap = A*p;\n\
	 \n;Bp = B*p;\n\
	 \n;Cp = C*p;\n\
	 \nOnece recieved the keys, they calculate the follow value\n\
	 \n;Ka = pair(Bp,phi(Cp),r);\n\
	 \n;Kb = pair(Ap,phi(Cp),r);\n\
	 \n;Kc = pair(Ap,phi(Bp),r);\
	 \nNow, we can check they have shared the key pair(p,phi(p),r)^ABC\n\
	 \n;Ka^A;\n\
	 \n;Kb^B;\n\
	 \n;Kc^C;\n\
	 \nThanks for watching!";
	int len = strlen(dem);
	int i = 0, j = 0;
	while(i < len){
		if(dem[i] != ';')printf("%c",dem[i]);
		else{
			i++;
			j = nextchar(dem,';',i,len);
			char * input = substring(dem,i,j);
			if(!input){
				i++;continue;
			}
			tokenlen = 0;
			lex(input);
			printf("> "); 
			slowshow(input);
			Sleep(500000);
			printf("\n");
			statement(0);
			free(input);
			i = j+1;continue;
		}
		Sleep(20000);
		i++;
	}
	system("pause");
}

void init_system()
{
	system("cls");
	printf("------------------------------------------------------------\n");
	printf("    Welcome to use weil pairing demostration interpreter\n");
	printf("------------------------------------------------------------\n");
	printf("Version 2.7\n");
	printf("Copyright (c) 2015. Wu Changlong<changlong1993@gmail.com>\n");
	printf("\n");
	printf("Hints:\n");
	printf("1. type 'quit' to exit.\n");
	printf("2. type 'clear' to clear the screen.\n");
	printf("3. type 'reset' to set all the environment.\n");
	printf("4. type 'help' to get the detailed help.\n");
	printf("\n");
	
	global_curve = newcurve(0,1);
	global_p = 23;
	
}
void help()
{
	system("cls");
	printf("-------------------------------------------------------------------\n");
	printf("    This is the weil pairing demostration interpreter help system\n");
	printf("-------------------------------------------------------------------\n");
	printf("\n");
	
	printf("Function list:\n");
	printf("1. Randomprime(n) to generate a prime around n\n");
	printf("2. PrimeQ(n) to test whether n is a prime\n");
	printf("3. sqrt(n) square root of varibale n\n");
	printf("3. Randompoint() to generate a random point on current curve n\n");
	printf("4. Mill(P,Q,m) to comput miller's algorithm\n");
	printf("5. phi(P) to map point P = (x , y) to the point Q = (\zeta x,y), where \zeta is 3-primitive root.\n");
	printf("6. Pair(P,Q,n) to compute weil pairing for P,Q with order n\n");
	
	
	printf("\n\nOperation:\n");
	printf("1. Our language compatible with the common mathematical expression completely.\n");
	printf("2. To buid new variables, you can use 'var name1 name2 name3 ...', or just 'a = value'.\n");
	printf("3. Use 'del name' to delete the varaible you don't need.\n");
	printf("4. We have an automatic type inference system, so all the types are generated automaticaly.\n");
	printf("5. You can use 'check name' to see the total type infomation of the vairable 'name'\n");
	printf("6. The arithmetic operations is designed to be duplicated: \n");
	printf("e.g. the expression 'p+q', means natrual number addition when the type of p & q are of type 'num', ");
	printf("but point addition when the types are 'point'. So, feel free to use the operator as you want.\n");
	
	printf("\n\nDo you want to see a dynamic demostration?(Y/N): ");
	char * f = (char *)malloc(1000);
	gets(f);
	if(f && (f[0] == 'y' || f[0] == 'Y' || strcamp(f,"yes") || strcamp(f,"YES")))demo();
	free(f);
	init_system();
}
TOKEN * newtoken(int type, char * value, char c)
{
	TOKEN * result = (TOKEN *)malloc(sizeof(toke));
	result->type = type;
	result->vaule = value;
	result->c = c;
	return result; 
}

void addtoken(int type, char * value, char c)
{
	if(tokenlist[tokenlen] == NULL)tokenlist[tokenlen] = newtoken(type,value,c);
	else{
		tokenlist[tokenlen]->type = type;
		tokenlist[tokenlen]->c = c;
		if(tokenlist[tokenlen] != NULL)free(tokenlist[tokenlen]->vaule);
		tokenlist[tokenlen]->vaule = value; 
	}
	tokenlen++;
}

void lex(char * input)
{
	int header = 0, len = strlen(input);
	
	char * t;
	
	while(header < len){
		header = moveahead(input,header,len);
		if(isspacialchar(input[header])){
			addtoken(0,NULL,input[header]);
			header++; continue;
		}
		t = nextliteral(input,header,len,&header);
		addtoken(1,t,'0');
	}
	if(tokenlen > 0 && tokenlist[tokenlen-1]->type == 0 && tokenlist[tokenlen - 1]->c == ' ')tokenlen--;
}

void showtoken()
{
	for(int i = 0;i < tokenlen; i++){
		if(tokenlist[i]->type == 0)printf(" '%c' ",tokenlist[i]->c);
		else
			printf(" '%s' ",tokenlist[i]->vaule);
	}
	printf("\n");
}

int main()
{
	char * worktap = (char *)malloc(MAX_STR);
	int pos;
	init();
	init_system();
	while(true){
		printf("> ");
		
		tokenlen = 0;
		
		while(true){
			
			gets(worktap);
			
			if(strlen(worktap) == 0){
				break;
			}
			
			lex(worktap);
			
			if(tokenlist[tokenlen-1]->type == 0 && tokenlist[tokenlen-1]->c == ';')continue;
			break;
		}
		
		if(tokenlen < 1)continue;
		
		if(tokenlist[0]->type == 1){
			if(strcamp(tokenlist[0]->vaule,"quit")||strcamp(tokenlist[0]->vaule,"exit"))break;
			if(strcamp(tokenlist[0]->vaule,"help")){
				help();
				continue;
			}else if(strcamp(tokenlist[0]->vaule,"clear")){
				system("cls");
				continue;
			}else if(strcamp(tokenlist[0]->vaule,"reset")){
				init_system();
				continue;
			}
		}
		statement(0);
	}
	
	free(worktap);
	
	return 0;
}

