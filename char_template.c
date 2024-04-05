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

int is_letter(int ch){
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <='Z');
}

int is_white_space(int ch){
	return ch == '\n' || ch == ' ' || ch == '\t';
}

void swap(int *v1, int *v2){
	int tmp = *v1;
	*v1 = *v2;
	*v2 = tmp;
}

int partition(int indexes[], int charCnt[], int beg, int end){
	int i = beg;
	for (int j = beg; j < end; j++){
		if (charCnt[indexes[j]] < charCnt[indexes[end]]){
			swap(&indexes[i], &indexes[j]);
			i++;
		}
	}
	swap(&indexes[end], &indexes[i]);
	return i;
}

void sort_tab(int indexes[], int charCnt[], int beg, int end){
	if (beg < end){
		int pivot = partition(indexes, charCnt, beg, end);
		sort_tab(indexes, charCnt, beg, pivot - 1);
		sort_tab(indexes, charCnt, pivot + 1, end);
	}
}

void sort_tab2(int indexes[], int charCnt[]){
	for (int i = 0; i < MAX_CHARS - 1; i++){
		int maxi = i;
		for (int j = i + 1; j < MAX_CHARS; j++){
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

void print_tab(int tab[], int tabSize){
	for (int i = 0; i < MAX_CHARS; i++){
		printf("%d ", tab[i]);
	}
	printf("\n");
	for (int i = 0; i < MAX_CHARS; i++){
		printf("%c ", tab[i]+FIRST_CHAR);
	}
	printf("\n");

}

void char_count(int char_no, int *n_char, int *cnt) {
	int charCnt[MAX_CHARS] = {0};
	int ch;

	while ((ch = fgetc(stdin)) != EOF){
		charCnt[ch - FIRST_CHAR]++;
	}
	
	// for(int i = 0; i < MAX_CHARS; i++){
	// 	printf("%c, %d\n", i + FIRST_CHAR, charCnt[i]);
	// }

	int indexes[MAX_CHARS];

	for (int i = 0; i < MAX_CHARS; i++){
		indexes[i] = i;
	}

	// sort_tab(indexes, charCnt, 0, MAX_CHARS - 1);
	sort_tab2(indexes, charCnt);

	// print_tab(indexes, MAX_CHARS);

	*n_char = indexes[char_no - 1] + FIRST_CHAR;
	*cnt = charCnt[indexes[char_no - 1]];
}

void bigram_count(int bigram_no, int bigram[]) {
}

void find_comments(int *line_comment_counter, int *block_comment_counter) {
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

