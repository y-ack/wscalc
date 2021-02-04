#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <setjmp.h>
#include <readline/readline.h>
#include <readline/history.h>

#define when(n) break; case n

typedef long int Num;
typedef char* Str;
typedef Num (*Op)(/*any args lol*/); //typedef Op
typedef struct {Str name; Op func;} OpDef;

#define OPDEF(name, expr) Num op_##name(Num a, Num b) { return (expr); }
OPDEF(neg,-a);
OpDef prefix[] = {
	{"-",op_neg},
	{NULL, NULL},
};
OPDEF(add,a+b);OPDEF(sub,a-b);OPDEF(mul,a*b);OPDEF(div,a/b);OPDEF(mod,fmod(a,b));OPDEF(pow,pow(a,b));
OpDef infix[] = {
	{"+",op_add},{"-",op_sub},{"*",op_mul},{"/",op_div},{"%",op_mod},{"^",op_pow},
	{NULL, NULL},
};

// GLOBALS
Num ans = 0;
jmp_buf env;

Op search(Str* str, OpDef* ops) {
	for (; ops->name ; ops++) {
		int len = strlen(ops->name);
		if (strncmp(*str, ops->name, len)==0) {
			*str += len;
			return ops->func;
		}
	}
	return NULL;
}

Num readExpr(Str*, int);

Num readValue(Str* str, int depth) {
	//	if ((*str)[0]>='0' && (*str)[0]<='9' || (*str)[0]=='.') {
	if ((*str)[0]>='0' && (*str)[0]<='9'
		|| (*str)[0]=='b' || (*str)[0]=='x' || (*str)[0]=='o') {
		Str end;
		Num num;
		if((*str)[0]=='b') num = strtol(++(*str), &end, 2);
		else if((*str)[0]=='o') num = strtol(++(*str), &end, 8);
		else if((*str)[0]=='x') num = strtol(++(*str), &end, 16);
		else num = strtol(*str, &end, 10);
		if (end) {
			*str = end;
			return num;
		}
	}
	if ((*str)[0]=='c') {
		Num num = (long int)(char)(++(*str))[0];
		(*str)++;
		return num;
	}
	if ((*str)[0]=='a') {
		(*str)++;
		return ans;
	}
	if ((*str)[0]==' ') { // Start group
		(*str)++;
		return readExpr(str, depth+1);
	}
	Op op = search(str, prefix);
	if (op)
		return op(readValue(str, depth), 0);
	longjmp(env, 1);
}

Num readAfter(Str* str, int depth, Num acc) {
	if ((*str)[0]==' ') { // End group
		(*str)++;
		if (depth>0)
			return acc;
	}
	if ((*str)[0]=='\0') // End group (end of string)
		return acc;
	
	Op op = search(str, infix);
	if (op) {
		Num v = readValue(str, depth);
		return readAfter(str, depth, op(acc, v));
	}
	longjmp(env, 2);
}

Num readExpr(Str* str, int depth) {
	Num acc = readValue(str, depth);
	return readAfter(str, depth, acc);
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }
	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;
	do {
		tmp_value = value;
		value /= base;
		*ptr++ =
		"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"
			[35 + (tmp_value - value * base)];
	} while ( value );
	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

void display_result(Num res) {
	char bin [33];
	itoa (res,bin,2);
	printf("b: %s\n", bin);
	//printf("o: %o\n", res);
	printf("d: %d\n", res);
	printf("x: %x\n", res);
	printf("c: '%c'\n", res);
}	

int main(int argc, Str* argv) {
	while (1) {
		Str line = readline("<< ");
		add_history(line);
		if (!line)
			break;
		//if (line[0]=='\0')
		//	continue;
		switch (setjmp(env)) {
		when(0):;
			Str expr = line;
			Num res = readExpr(&expr, 0);
			ans = res;
			display_result(res);
		when(1):;
			printf("! Error: expected value (number, a) or prefix operator (");
			OpDef* op;
			for (op=prefix; op->name; op++) {
				if (op!=prefix)
					printf(", ");
				printf("%s", op->name);
			}
			printf(")\n");
			goto err;
		when(2):;
			printf("! Error: expected operator (");
			for (op=infix; op->name; op++) {
				if (op!=infix)
					printf(", ");
				printf("%s", op->name);
			}
			printf(")\n");
			goto err;
		err:;
			printf("! %.*s⟨here⟩%s\n", (int)(expr-line), line, expr);
		}
		free(line);
	}
}
