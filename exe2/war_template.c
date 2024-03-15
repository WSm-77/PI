#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_CARDS 52
#define NUMBER_OF_COLORS 4

#define PLAYER_A_WINS = 0
#define	PLAYER_B_WINS = 1
#define WAR = 2

int rand_from_interval(int a, int b) {
	if (a > b){
		return INT_MAX;
	}

	if ( b - a > RAND_MAX){
		return INT_MAX;
	}

	if ( a == b){
		return a;
	}

	int intervalSize = b - a + 1;
	int random = (rand() % intervalSize) + a;

	return random;
}

void swap (int array[], int i, int k) {
	int tmp = array[i];
	array[i] = array[k];
	array[k] = tmp;
}

void shuffle(int t[], int size) {
	for (int i = 0; i < size; i++){
		t[i] = i;
	}
	for (int i = 0; i < size - 1; i++){
		int k = rand_from_interval(i, size - 1);
		swap(t, i, k);
	}
}

int card_strength(int id){
	return id % NUMBER_OF_COLORS;
}

int conflict_verdict(int card1Id, int card2Id){
	if (card_strength(card1) > card_strength(card2Id)){
		return PLAYER_A_WINS;
	}
	elif (card_strength(card1Id) < card_strength(card2Id)){
		return PLAYER_B_WINS;
	}
	else{
		return WAR;
	}
}

void game(int n, int player_A[], int player_B[], int max_conflicts, int simplified_mode) {
	int deckSizeA = NUMBER_OF_CARDS / 2;
	int deckSizeB = NUMBER_OF_CARDS / 2;
	int conflictsCnt = 0;
	int topCardIndexA = 0;
	int lastCardIndexA = deckSizeA - 1;
	int topCardIndexB = 0;
	int lastCardIndexB = deckSizeB - 1;

	int numberOfCardsOnTable = 1;
	while (conflictsCnt < max_conflicts){
		numberOfCardsOnTable++;
		conflictsCnt++;
		int cardOnTableIndexA = (topCardIndexA + numberOfCardsOnTable - 1) % deckSizeA;
		int cardOnTableIndexB = (topCardIndexB + numberOfCardsOnTable - 1) % deckSizeB;
		
		switch (conflict_verdict(player_A[cardOnTableIndexA], player_B[cardOnTableIndexB])){
			case PLAYER_A_WINS:
				first_takes_cards(numberOfCardsOnTable, player_A, &topCardIndexA, &lastCardIndexA, player_B, &topCardIndexB, &lastCardIndexB);
				break;
			
			case PLAYER_B_WINS:
				first_takes_cards(numberOfCardsOnTable, player_B, &topCardIndexB, &lastCardIndexB, player_A, &topCardIndexA, &lastCardIndexA);
				break;

			case WAR:

				break;
			
			default:
				printf("something gone wrong\n");
		}
	}
}

void first_takes_cards(int number_of_cards_on_table, int winner[], int *winnerTopCardIndex, int *winnerLastCardIndex,
		const int loser[], int *loserTopCard, int *p_cards_2, int size) {
	
	 for (int i = 0; i < number_of_cards_on_table; i++){
		*winnerLastCardIndex = ((*winnerLastCardIndex) + 1) % NUMBER_OF_CARDS;
		
	 }

}

void both_take_cards(int player_1[], int *pout_1, const int *p_cards_1,
		int player_2[], int *pout_2, const int *p_cards_2, int size) {
}

int main(void) {
	int player_A[NUMBER_OF_CARDS], player_B[NUMBER_OF_CARDS];
	int deck[NUMBER_OF_CARDS];
	int max_conflicts;
	int simplified_mode;
	int seed;
	scanf("%d", &seed);
	scanf("%d", &simplified_mode);
	scanf("%d", &max_conflicts);

	for(int i = 0; i < NUMBER_OF_CARDS; i++) deck[i] = i;
	srand((unsigned) seed);
	shuffle(deck, NUMBER_OF_CARDS);
	for(int i = 0; i < NUMBER_OF_CARDS / 2; i++) {
		player_A[i] = deck[i];
		player_B[i] = deck[i + NUMBER_OF_CARDS / 2];
	}
	game(NUMBER_OF_CARDS, player_A, player_B, max_conflicts, simplified_mode);
	return 0;
}

