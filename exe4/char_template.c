#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_BIGRAMS ((LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR))

#define NEWLINE '\n'
#define IN_WORD 1

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2

int count[MAX_BIGRAMS] = { 0 };

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	if (count[va] == count[vb]) return va - vb;	return count[vb] - count[va];
}

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp_di (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	// sort according to second char if counts and the first char equal
	if (count[va] == count[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
	// sort according to first char if counts equal
	if (count[va] == count[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
	return count[vb] - count[va];
}

int is_char(int ch){
	return ch < LAST_CHAR && ch >= FIRST_CHAR;
}

int is_white_space(int ch){
	return ch == '\n' || ch == ' ' || ch == '\t';
}

void swap(int *v1, int *v2){
	int tmp = *v1;
	*v1 = *v2;
	*v2 = tmp;
}

void selection_sort(int indexes[], int charCnt[], int tabSize){
	for (int i = 0; i < tabSize - 1; i++){
		int maxi = i;
		for (int j = i + 1; j < tabSize; j++){
			if (charCnt[indexes[j]] > charCnt[indexes[maxi]])
				maxi = j;
		}
		swap(&indexes[maxi], &indexes[i]);
	}
}


// count lines, words & chars in a given text file
void wc(int *nl, int *nw, int *nc) {
	*nl = *nw = *nc = 0;
	int ch;
	int prevch = ' ';
	while ((ch = fgetc(stdin)) != EOF){
		(*nc)++;
		if ('\n' == ch)
			(*nl)++;

		if (is_white_space(prevch) && !(is_white_space(ch)))
			(*nw)++;

		prevch = ch;
	}
}

void char_count(int char_no, int *n_char, int *cnt) {
	int charCnt[MAX_CHARS] = {0};
	int ch;

	while ((ch = fgetc(stdin)) != EOF){
		charCnt[ch - FIRST_CHAR]++;
	}

	int indexes[MAX_CHARS];

	for (int i = 0; i < MAX_CHARS; i++){
		indexes[i] = i;
	}

	selection_sort(indexes, charCnt, MAX_CHARS);

	*n_char = indexes[char_no - 1] + FIRST_CHAR;
	*cnt = charCnt[indexes[char_no - 1]];
}

int bigram_to_bigram_index(int ch1, int ch2){
	return (ch1 - FIRST_CHAR) * MAX_CHARS + ch2 - FIRST_CHAR;
}

void bigram_count(int bigram_no, int bigram[]) {
	int ch;
	int prevch = 0;
	int bigramsCnt[MAX_BIGRAMS] = {0};
	while ((ch = fgetc(stdin)) != EOF){
		if (is_char(prevch) && is_char(ch)){
			int bigramIndex = bigram_to_bigram_index(prevch, ch);
			bigramsCnt[bigramIndex]++;
		}
		prevch = ch;
	}

	int indexes[MAX_BIGRAMS];

	for (int i = 0; i < MAX_BIGRAMS; i++){
		indexes[i] = i;
	}

	selection_sort(indexes, bigramsCnt, MAX_BIGRAMS);

	bigram[0] = indexes[bigram_no - 1] / MAX_CHARS + FIRST_CHAR;
	bigram[1] = indexes[bigram_no - 1] % MAX_CHARS + FIRST_CHAR;
	bigram[2] = bigramsCnt[indexes[bigram_no - 1]];
}

void find_comments(int *line_comment_counter, int *block_comment_counter) {
	int isLineComment = 0;
	int isBlockComment = 0;
	int ch;
	int prevch = 0;
	while ((ch = fgetc(stdin)) != EOF){
		if ('\n' == ch){
			isLineComment = 0;
		}

		if (isBlockComment && ('*' == prevch) && ('/' == ch)){
			isBlockComment = 0;
		}
		else if ('/' == prevch && '/' == ch){
			if (!(isBlockComment || isLineComment)){
				(*line_comment_counter)++;
				isLineComment = 1;
			}

		}
		else if ('/' == prevch && '*' == ch)
		{
			if (!(isBlockComment || isLineComment)){
				(*block_comment_counter)++;
				isBlockComment = 1;
			}
		}

		prevch = ch;
	}
}

#define MAX_LINE 128

int read_int() {
	char line[MAX_LINE];
	fgets(line, MAX_LINE, stdin); // to get the whole line
	return (int)strtol(line, NULL, 10);
}

int main(void) {
	int to_do;
	int nl, nw, nc, char_no, n_char, cnt;
	int line_comment_counter, block_comment_counter;
	int bigram[3];

	to_do = read_int();
	switch (to_do) {
		case 1: // wc()
			wc (&nl, &nw, &nc);
			printf("%d %d %d\n", nl, nw, nc);
			break;
		case 2: // char_count()
			char_no = read_int();
			char_count(char_no, &n_char, &cnt);
			printf("%c %d\n", n_char, cnt);
			break;
		case 3: // bigram_count()
			char_no = read_int();
			bigram_count(char_no, bigram);
			printf("%c%c %d\n", bigram[0], bigram[1], bigram[2]);
			break;
		case 4:
			find_comments(&line_comment_counter, &block_comment_counter);
			printf("%d %d\n", block_comment_counter, line_comment_counter);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}

