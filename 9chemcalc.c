#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <ctype.h>
#include "amu.h"
#define BUFSIZE 1024

void
rmchars(char* str, int n)
{
	memmove(str, str+n, strlen(str));
}

void
cutstr(char* dest, char* origin, int n)
{
	snprint(dest, n+1, "%.*s", n, origin);	/* strcpy breaks when used until last char. read manual if parameters are confusing */
	rmchars(origin, n);
}

int
coefatoi(char* tocoef)
{
	int coef = atoi(tocoef);
	if(coef)
		return coef;
	return 1;
}

void*
saferealloc(void* p, size_t size)
{
	void* retval = realloc(p, size);
	if(p == NULL)
		exits("\nallocation failed\n");
	return retval;
}


float
element2amu(char* element)
{
	int i = 0;
	char *name = dict[i].elmnt;
	element[strcspn(element, "\0")] = 0;
	while(name)
	{
		if(strcmp(name, element) == 0)
			return dict[i].gpm;
		name = dict[++i].elmnt;
	}
	return 0;
}

struct elementcoef
tokenizer(char* str, int scalar)
{
	int i;
	char tokbuf[BUFSIZE];
	struct elementcoef tokenized;
	
//	printf("  |before the cut, the input is %s ", str);

	for(i = 0; !(str[i+1] >= 'A' && str[i+1] <= 'Z') && str[i+1] != '(' && str[i+1] != '\0'; i++);	/* walks until end of string or new element, marked by a capital letter or parentheses */
	cutstr(tokbuf, str, i+1);

//	printf("after the cut, the input is %s, and the buffer is %s|\n", str, tokbuf);

	for(i = 0; tokbuf[i+1] >= 'a' && tokbuf[i+1] <= 'z'; i++);
	cutstr(tokenized.elmnt, tokbuf, i+1);
	tokenized.coef = scalar * coefatoi(tokbuf);

	return tokenized;
}

struct elementcoef*
parentokenize(char* str)
{
	int i;
	char outcoeftoi[100], tokbuf[100];
	struct elementcoef *tokenized = malloc(0);
//	struct elementcoef *tokenized = malloc(sizeof(struct elementcoef) * 10);

	rmchars(str, 1);
	for(i = 0; str[i+1] != ')' && str[i+1] != '\0'; i++);
	cutstr(tokbuf, str, i+1);
	
	rmchars(str, 1);
	for(i = 0; str[i+1] != '\0' && !(str[i+1] >= 'A' && str[i+1] <= 'Z') && str[i+1] != '('; i++);
	cutstr(outcoeftoi, str, i+1);
	
	int outcoef = coefatoi(outcoeftoi);
	for(int j = 0; tokbuf[0] != '\0'; tokenized[j++] = tokenizer(tokbuf, outcoef))
		tokenized = realloc(tokenized, sizeof(struct elementcoef)*(j+1));

	return tokenized;
}

void
main(void)
{
	int j = 0, k = 0;
	float totamu = 0.0;
	char input[100];
	struct elementcoef *token = malloc(0);

	print("enter a molecule\n\n");
	scanf("%s", input);
	while(input[0] != '\0')
	{
		if(input[0] == '(')
		{
			k = 0;
			for(struct elementcoef *tmp = parentokenize(input); tmp[k].elmnt[0] != '\0'; token[j++] = tmp[k++])
				token = realloc(token, sizeof(struct elementcoef)*(j+1));
		}
		else
		{
			token = realloc(token, sizeof(struct elementcoef)*(j+1));
			token[j++] = tokenizer(input, 1);
		}
	}

	for(int i = 0; token[i].coef != 0; i++)
		totamu += token[i].coef*element2amu(token[i].elmnt);
/*		printf("element %d is %s, has coef of %d\n", i+1, token[i].elmnt, token[i].coef); */
	free(token);
	print("%.3f g/mol\n", totamu);
	exits(nil);
}
