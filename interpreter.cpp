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

char * TYPE[3] = {"num","felement","point"};
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
			printf("Curve: y*y = x*x*x + %lld*x + %lld\n",v->c->A,v->c->B);
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
			printf("Curve: y*y = x*x*x + %lld*x + %lld\n",v->c->A,v->c->B);
			break;
		default:
			printf("unknow type.\n");
	}
	printf("\n");
}

void freevalu(VALU * a)
{
	if(a == NULL)return;
	
	if(a->fvalue != NULL)free(a->fvalue);
	if(a->c != NULL)free(a->c);
	//if(a->pvalue != NULL)freepoint(a->pvalue);
	
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
	if(a->type == 1 || b->type == 1){
		result->type = 1; fadd(a->fvalue,b->fvalue,global_p,result->fvalue);
		return true;
	}
	if(a->type == 2 && b->type == 2){
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
	if(a->type == 1 || b->type == 1){
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
	
	if(*start >= tokenlen || checkchar(tokenlist[*start],';') || checkchar(tokenlist[*start],')') || checkchar(tokenlist[*start],',')){ // end of expression
		*start += 1;
		return result;
	}
	if(checkchar(tokenlist[*start],'+')){ // if add
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
			printf("Type '%s' and Type '%s' can not be added.\n",TYPE[temp->type],TYPE[ty]);
			freevalu(result);
			result = NULL;
		}
		freevalu(temp);
		return result;
	}
	if(checkchar(tokenlist[*start],'-')){ // if minus
		*start += 1;
		VALU * temp = expression(start);
		
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
		freevalu(temp);
		return result;
	}
	
	printf("Syntax Error! Invalid expression.\n");
	
	return NULL;
}

VALU * term(int * start)
{
	if(*start >= tokenlen || checkchar(tokenlist[*start],';'))return NULL;

	VALU * result = NULL;
	
	if(tokenlist[*start]->type == 0){
		if(tokenlist[*start]->c == '('){
			if(*start + 4 < tokenlen && checkchar(tokenlist[*start + 2],',') && checkchar(tokenlist[*start+4],')')){
				char * name1 = tokenlist[*start+1]->vaule, *name2 = tokenlist[*start + 3]->vaule;
				
				if(checkint(name1) && checkint(name2)){
					FPOINT * temp = newfpoint(ABS(parseint(name1),global_p),ABS(parseint(name2),global_p));
					result = newvalue(1,0,temp,NULL,NULL);
					*start += 5;
				}else{
					printf("Syntax Error! Can't parse '(%s,%s)' to be an field element.\n",name1,name2);
					return NULL;
				}
			}else{
				*start += 1;
				result = expression(start);
			}
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
	
	if(result == NULL)return NULL;
	
	int ty = result->type;
	
	if(*start >= tokenlen || checkchar(tokenlist[*start],';') || checkchar(tokenlist[*start],')') || checkchar(tokenlist[*start],',') || checkchar(tokenlist[*start],'+') || checkchar(tokenlist[*start],'-')){ // end of expression
		return result;
	}	
	if(checkchar(tokenlist[*start],'*')){
		*start += 1;
		
		VALU * temp = term(start);
				
		if(temp == NULL || result == NULL){
			if(result)freevalu(result);
			if(temp)freevalu(temp);
			result = NULL;
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
	if(checkchar(tokenlist[*start],'/')){
		*start += 1;
		
		VALU * temp = term(start);
		
		if(temp == NULL || result == NULL){
			if(result)freevalu(result);
			if(temp)freevalu(temp);
			result = NULL;
			return NULL;
		}
		if(!diviv(result,temp,result)){
			printf("Type '%s' can not divide Type '%s'.\n",TYPE[temp->type],TYPE[ty]);
			freevalu(result);
			result = NULL;
		}
		freevalu(temp);
		return result; 
	}
	printf("Syntax Error! Invalid term.\n");
	return NULL;
}

VALU * tuple(int *start, char * key)
{
	//TODO
	
	return NULL;
}

void help()
{
	//TODO
	
	printf("No help currently avialable.\n");
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
	
	printf("------------------------------------------------------------\n");
	printf("    Welcome to use Weil pairing demostration interpreter\n");
	printf("------------------------------------------------------------\n");
	printf("Version 2.7\n");
	printf("Copyright (c) 2015. Wu Changlong<changlong1993@gmail.com>\n");
	printf("\n");
	printf("Hints:\n");
	printf("1. type 'quit' to exit.\n");
	printf("2. type 'clear' to clear the screen.\n");
	printf("3. type 'reset' to set all the environment.\n");
	printf("4. type 'help' to get the detailed help.\n\n");
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

