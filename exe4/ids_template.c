#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2
#define IN_STRING 4
#define IN_ID 8

#define MAX_ID_LEN 64
#define MAX_IDS 1024

#define TEST 1

int index_cmp(const void*, const void*);
int cmp(const void*, const void*);

char identifiers[MAX_IDS][MAX_ID_LEN];

#define KEYWORDS_CNT 32

const char *keywords[] = {
	"auto", "break", "case", "char",
	"const", "continue", "default", "do",
	"double", "else", "enum", "extern",
	"float", "for", "goto", "if",
	"int", "long", "register", "return",
	"short", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union",
	"unsigned", "void", "volatile", "while"
};

int is_letter(int ch){
	return ('_' == ch) || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

int is_valid_char(int ch){
	return ('_' == ch) || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9');
}

int is_white_space(int ch){
	return ch == '\n' || ch == ' ' || ch == '\t';
}

int is_keyword(char string[]){
	for (int i = 0; i < KEYWORDS_CNT; i++){
		if (strcmp(string, keywords[i]) == 0){
			return 1;
		}
	}

	return 0;
}

void add_to_identifiers(int *identifiersCnt, char string[]){
	int isUnique = 1;
	if (!(is_keyword(string))){
		for (int i = 0; i < *identifiersCnt; i++){
			if(strcmp(string, identifiers[i]) == 0){
				isUnique = 0;
				break;
			}
		}

		if (isUnique){
			strcpy(identifiers[(*identifiersCnt)++], string);
		}
	}
}

void add_to_output(char *string, char output[], int *outputSize){
	int i = 0;
	while (string[i] != '\0'){
		output[(*outputSize)++] = string[i];
		i++;
	}
}

int find_idents() {
	int isBlockComment = 0;
	int isLineComment = 0;
	int isInDoubleQuotes = 0;
	int isInSingleQuotes = 0;

	char currentString[MAX_ID_LEN] = {0};
	int mightBeIdentifier = 0;
	unsigned int currentStringIndex = 0;
	int ch;
	int prevch = ' ';
	int blockCommentCharcnt = 0;

	int identifiersCnt = 0;
	char output[1024*8] = {0};
	int outputSize = 0;

	while ((ch = fgetc(stdin)) != EOF){
		if (TEST && ch != '\n'){
			output[outputSize++] = ch;
		}

		if (isBlockComment){
			blockCommentCharcnt++;
			if (blockCommentCharcnt > 4){
				blockCommentCharcnt = 4;
			}
		}

		if (ch == '\n')
		{
			if (isLineComment){
				if(TEST){
					char str[] = "(LineCommentEnd)";
					add_to_output(str, output, &outputSize);
				}
			}
			if (TEST){
				output[outputSize++] = ch;
			}
			isLineComment = 0;
		}

		if (isBlockComment && blockCommentCharcnt > 3 && prevch == '*' && ch == '/'){
			if(TEST){
				char str[] = "(BlockCommentEnd)";
				add_to_output(str, output, &outputSize);
			}
			isBlockComment = 0;
			blockCommentCharcnt = 0;
		}
		else if (prevch == '/' && ch == '*' && !(isInDoubleQuotes || isInSingleQuotes || isLineComment || isBlockComment)){
			if(TEST){
				char str[] = "(BlockCommentStart)";
				add_to_output(str, output, &outputSize);
			}
			isBlockComment = 1;
			blockCommentCharcnt = 2;
		}
		else if (prevch == '/' && ch == '/' && !(isInDoubleQuotes || isInSingleQuotes || isLineComment || isBlockComment)){
			isLineComment = 1;
			if(TEST){
				char str[] = "(LineComment)";
				add_to_output(str, output, &outputSize);
			}
		}
		else if (ch == '"' && prevch != '\\' && !(isBlockComment || isLineComment)){
			if(TEST){
				char str[] = "(DQuotes)";
				add_to_output(str, output, &outputSize);
			}
			isInDoubleQuotes = !(isInDoubleQuotes);
		}
		else if (ch == '\'' && prevch != '\\' && !(isBlockComment || isLineComment)){
			if(TEST){
				char str[] = "(SQuotes)";
				add_to_output(str, output, &outputSize);
			}
			isInSingleQuotes = !(isInSingleQuotes);
		}
		else{
			if (!(isBlockComment || isLineComment || isInSingleQuotes || isInDoubleQuotes)){
				if (mightBeIdentifier && !(is_valid_char(ch))){
					currentString[currentStringIndex] = '\0';
					if(TEST){
						{
							char str[] = "\n(\"";
							add_to_output(str, output, &outputSize);
						}
						{
							add_to_output(currentString, output, &outputSize);
						}
						{
							char str[] = "\" identifier check)\n";
							add_to_output(str, output, &outputSize);
						}
					}

					add_to_identifiers(&identifiersCnt, currentString);
					mightBeIdentifier = 0;
					currentStringIndex = 0;
				}
				else if (is_white_space(prevch) && is_letter(ch)){
					currentString[currentStringIndex++] = ch;
					mightBeIdentifier = 1;
				}
				else if (mightBeIdentifier && is_valid_char(ch)){
					currentString[currentStringIndex++] = ch;
				}
			}
			
		}

		prevch = ch;
	}

	if (TEST){
		for (int i = 0; i < outputSize; i++){
			printf("%c", output[i]);
		}
	}

	return identifiersCnt;
}

int cmp(const void* first_arg, const void* second_arg) {
	char *a = *(char**)first_arg;
	char *b = *(char**)second_arg;
	return strcmp(a, b);
}

int index_cmp(const void* first_arg, const void* second_arg) {
	int a = *(int*)first_arg;
	int b = *(int*)second_arg;
	return strcmp(identifiers[a], identifiers[b]);
}

int main(void) {
	printf("%d\n", find_idents());
	return 0;
}

