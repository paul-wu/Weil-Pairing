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


lint global_p;

char * TYPE[3] = {"num","felement","point"};
char * KEY[10] = {"randomprime","modsqrt","modpower","select","mod","curve","remove","var"};

typedef struct variable{
	char * name;
	int type,value,tag;
	FPOINT * fvalue;
	POINT * pvalue;
	CURVE * c;
}VAR;

typedef struct valu{
	int type;
	int value;
	FPOINT * fvalue;
	POINT * pvalue;
	CURVE * c;
}VALU;

char specialchar[30] = ".,\\{}[]()+-*/%$@#!?`~<>:|=^&\"";

int CURRENT_VAR_NUM = 0;

VAR * list[MAX_VAR];

void statement(char * input, int header);

VAR * expression(char * input, int start, int end);

VAR * term(char * input, int start, int end);

VAR * tuple(char * input, int start, int end);

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
	
	while(--len >= 0 && num[len] == '0');
	
	if(len < 0)return 0;
	
	result += num[len] - '0';
	
	while(--len >= 0){
		result *= 10;
		if(num[len] != '0')result += num[len] - '0';	
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

bool putvariable(int type, char * name,lint value, FPOINT * fvalue, POINT * pvalue, CURVE * c)
{
	int i = findavailablevariable();
	
	if(i > MAX_VAR)return false;
	
	VAR * cu = list[i];
	
	if(cu == NULL)cu = (VAR *)malloc(sizeof(variable));
	
	cu->tag = 1;
	cu->type = type;
	
	if(cu->name != NULL)free(cu->name);
	if(cu->fvalue != NULL)free(cu->fvalue);
	if(cu->pvalue != NULL)free(cu->pvalue);
	if(cu->c != NULL)free(cu->c);
	
	cu->name = copystring(name);
	
	switch(type){
		case 0:
			cu->value = value;
			cu->fvalue = NULL; cu->pvalue = NULL; cu->c = NULL;
			break;
		case 1:
			cu->fvalue = fvalue;cu->pvalue = NULL;cu->c = NULL;
			break;
		case 2:
			cu->fvalue = NULL;cu->pvalue = pvalue;cu->c = NULL;
			break;
		case 3:
			cu->fvalue = NULL;cu->pvalue = pvalue;cu->c = c;
		default:
			break;
	}
	
	CURRENT_VAR_NUM = i;
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
	for(int i = 0;i < 30;i++){
		if(c == specialchar[i])return true;
	} 
	return false;
}

int moveahead(char * input, int curren_head)
{
	int len = strlen(input);
	
	while(curren_head < len && transparentchar(input[curren_head++]));
	
	return curren_head - 1;
}

char * nextliteral(char * input, int curren_head, int * pos)
{	
	int len = strlen(input);
	
	int start = moveahead(input,curren_head);
	
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

int nextchar(char * input, char c, int current_head)
{
	int len = strlen(input);
	
	while(current_head < len && input[current_head++] != c);
	
	if(input[current_head - 1] == c)return current_head - 1;
	
	return len;
}

void showvalue(VAR * v)
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

void statement(char * input, int header)
{
	int len = strlen(input);
	VAR * newvar;
	
	if(len == 0){
		return;
	}
	
	header = moveahead(input,header);
	if(header >= len){
		return;
	}
	
	if(checknum(input[header])){
		int next = nextchar(input,';',header);
		
		newvar = expression(input,header,next);
		
		if(newvar == NULL){
			statement(input,next+1);
			return;
		}
		showvalue(newvar);
		return;
	}
	
	if(input[header] == ';'){
		statement(input,header+1);
		return;
	}
	
	int next;
	char * key = nextliteral(input,header,&next);
	
	if(strcamp(key,"var")){
		
		if(key != NULL)free(key);
		key = nextliteral(input,next,&next);
		next = moveahead(input,next);
		
		if(key == NULL){
			printf("You should specify a name to the variable.\n");
			return;
		}
		
		if(checknum(key[0])){
			printf("Variable should not start with an number.\n");
			return;
		}
		
		if(input[next] == '='){
			if(findvariable(key) != NULL){
				printf("Variable name '%s' has been use before, please specify a new one.\n",key);
				if(key!=NULL)free(key);
				return;
			}
			int end = nextchar(input,';',next);
			newvar = expression(input,next,end);
			
			if(newvar == NULL){
				printf("Can't build new variable beacause of nuknown expression value.\n");
				free(key);
				if(key!=NULL)free(key);
				return;
			}
			putvariable(newvar->type,key,newvar->value,newvar->fvalue,newvar->pvalue,newvar->c);
			free(key);
			statement(input,end+1);
			if(key!=NULL)free(key);
			return;
		}
		printf("Error! You must specify the initial value when build a new variable.\n");
		if(key!=NULL)free(key);
		return;
	}
	
	if(strcamp(key,"del")){
		if(key != NULL)free(key);
		key = nextliteral(input,next,&next);
		next = moveahead(input,next);
		
		if(key == NULL){
			printf("No variable been deleted.\n");
			return;
		}
		
		if(deletvariable(key)){
			printf("%s has been deleted.\n",key);
			int end = nextchar(input,';',next);
			
			statement(input,end);
			
			if(key!=NULL)free(key);
			return;
		}
		printf("%s has not been defined yet.\n",key);
		if(key!=NULL)free(key);
		return;
	}
	
}

VAR * expression(char * input, int start,int end)
{
	//TODO
	
	return NULL;
}

void help()
{
	//TODO
	
	printf("No help currently.\n");
}

void init_system()
{
	system("cls");
	printf("------------------------------------------------------------\n");
	printf("    Welcome to use weil pairing demostration interpreter\n");
	printf("------------------------------------------------------------\n");
	printf("Version 2.7\n");
	printf("Copyrigth (c) 2015. Wu Changlong<changlong1993@gmail.com>\n");
	printf("\n");
	printf("Hints:\n");
	printf("1. type 'quit' to exit.\n");
	printf("2. type 'clear' to clear the screen.\n");
	printf("3. type 'reset' to set all the environment.\n");
	printf("4. type 'help' to get the detailed help.\n");
	printf("\n");
}

int main()
{
	char * worktap = (char *)malloc(MAX_STR);
	char * token = NULL;
	int pos;
	
	printf("------------------------------------------------------------\n");
	printf("    Welcome to use weil pairing demostration interpreter\n");
	printf("------------------------------------------------------------\n");
	printf("Version 2.7\n");
	printf("Copyrigth (c) 2015. Wu Changlong<changlong1993@gmail.com>\n");
	printf("\n");
	printf("Hints:\n");
	printf("1. type 'quit' to exit.\n");
	printf("2. type 'clear' to clear the screen.\n");
	printf("3. type 'reset' to set all the environment.\n");
	printf("4. type 'help' to get the detailed help.\n\n");
	while(true){
		printf("> ");
		gets(worktap);
		
		if(token != NULL)free(token);
		token = nextliteral(worktap,0,&pos);
		
		if(strcamp(token,"quit")||strcamp(token,"exit"))break;
		if(strcamp(token,"help")){
			help();
		}else if(strcamp(token,"clear")){
			system("cls");
		}else if(strcamp(token,"reset")){
			init_system();
		}else
			statement(worktap,0);
	}
	
	free(worktap);
	
	return 0;
}

