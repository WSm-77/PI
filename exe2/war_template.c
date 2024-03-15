#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define NUMBER_OF_CARDS 52
#define NUMBER_OF_COLORS 4

#define PLAYER_A_WINS 0
#define	PLAYER_B_WINS 1
#define WAR 2
#define DRAW 3

void first_takes_cards(int number_of_cards_on_table, int winner[], int *winnerTopCardIndex, int *winnerLastCardIndex, int *winnerDeckSize,
		const int loser[], int *loserTopCardIndex, int *loserLastCard, int *loserDeckSize);

void both_take_cards(int player_A[], int *topCardIndexA, int *lastCardIndexA,
		int player_B[], int *topCardIndexB, int *lastCardIndexB);

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
	return id / NUMBER_OF_COLORS;
}

int conflict_verdict(int card1Id, int card2Id, int simplifiedMode){
	if (card_strength(card1Id) > card_strength(card2Id)){
		return PLAYER_A_WINS;
	}
	else if (card_strength(card1Id) < card_strength(card2Id)){
		return PLAYER_B_WINS;
	}
	else{
		if (simplifiedMode) {
			return DRAW;
		}
		else{
			return WAR;
		}
	}
}

void print_deck(int deck[], int topCardIndex, int lastCardIndex, int deckSize){
	for (int i = 0; i < deckSize; i++){
		printf("%d ", deck[topCardIndex]);
		topCardIndex++;
		topCardIndex %= NUMBER_OF_CARDS;
	}
	// printf("\n");
}

void game(int n, int player_A[], int player_B[], int max_conflicts, int simplified_mode) {
	int deckSizeA = NUMBER_OF_CARDS / 2;
	int deckSizeB = NUMBER_OF_CARDS - deckSizeA;
	int conflictsCnt = 0;
	int topCardIndexA = 0;
	int lastCardIndexA = deckSizeA - 1;
	int topCardIndexB = 0;
	int lastCardIndexB = deckSizeB - 1;

	enum GameResult {
		GAME_NOT_FINISHED = 0,
		LAST_CONFLICT_WITHOUT_VERDICT = 1,
		PLAYER_A_VICTORY = 2, 
		PLAYER_B_VICTORY = 3
	};

	enum GameResult gameResult = GAME_NOT_FINISHED;

	int numberOfCardsOnTable = 0;
	while (conflictsCnt < max_conflicts){
		numberOfCardsOnTable++;
		int cardOnTableIndexA = (topCardIndexA + numberOfCardsOnTable - 1) % NUMBER_OF_CARDS;
		int cardOnTableIndexB = (topCardIndexB + numberOfCardsOnTable - 1) % NUMBER_OF_CARDS;

		if (numberOfCardsOnTable > deckSizeA || numberOfCardsOnTable > deckSizeB){
			gameResult = LAST_CONFLICT_WITHOUT_VERDICT;
			break;
		}

		conflictsCnt++;

		int verdict = conflict_verdict(player_A[cardOnTableIndexA], player_B[cardOnTableIndexB], simplified_mode);

		// printf("Deck A: ");
		// print_deck(player_A, topCardIndexA, lastCardIndexA, deckSizeA);
		// printf("Deck B: ");
		// print_deck(player_B, topCardIndexB, lastCardIndexB, deckSizeB);
		// printf("\n\n");

		// printf("conflict number: %d\n", conflictsCnt);
		// printf("card A: %d, card B: %d\n", player_A[cardOnTableIndexA], player_B[cardOnTableIndexB]);
		// printf("strength A: %d, strength B: %d\tverdict: %d\n", card_strength(player_A[cardOnTableIndexA]), 
		// 	card_strength(player_B[cardOnTableIndexB]), verdict);
		
		switch (verdict){
			case PLAYER_A_WINS:
				first_takes_cards(numberOfCardsOnTable, player_A, &topCardIndexA, &lastCardIndexA, &deckSizeA, 
					player_B, &topCardIndexB, &lastCardIndexB, &deckSizeB);
				numberOfCardsOnTable = 0;
			break;
			
			case PLAYER_B_WINS:
				first_takes_cards(numberOfCardsOnTable, player_B, &topCardIndexB, &lastCardIndexB, &deckSizeB,
					player_A, &topCardIndexA, &lastCardIndexA, &deckSizeA);
				numberOfCardsOnTable = 0;
			break;

			case WAR:
				numberOfCardsOnTable++;
			break;

			case DRAW:
				both_take_cards(player_A, &topCardIndexA, &lastCardIndexA, player_B, &topCardIndexB, &lastCardIndexB);
				numberOfCardsOnTable = 0;
			break;
			
			default:
				printf("something gone wrong\n");
			break;
		}

		// printf("deck A size: %d\tdeck B size: %d\n\n", deckSizeA, deckSizeB);

		if (deckSizeA == 0){
			gameResult = PLAYER_B_VICTORY;
			break;
		}
		else if (deckSizeB == 0){
			gameResult = PLAYER_A_VICTORY;
			break;
		}
	}

	printf("%d ", gameResult);
	switch (gameResult)
	{
		case GAME_NOT_FINISHED:
		case LAST_CONFLICT_WITHOUT_VERDICT:
			printf("%d ", deckSizeA);
			printf("%d", deckSizeB);

		break;

		case PLAYER_A_VICTORY:
			printf("%d", conflictsCnt);
		break;

		case PLAYER_B_VICTORY:
			printf("\n");
			print_deck(player_B, topCardIndexB, lastCardIndexB, deckSizeB);
		break;
		
		default:
			printf("unexpected result\n");
		break;
	}
}

void first_takes_cards(int number_of_cards_on_table, int winner[], int *winnerTopCardIndex, int *winnerLastCardIndex, int *winnerDeckSize,
		const int loser[], int *loserTopCardIndex, int *loserLastCard, int *loserDeckSize) {
	
	// najpierw bierzemy swoje karty
	for (int i = 0; i < number_of_cards_on_table; i++){
		*winnerLastCardIndex = ((*winnerLastCardIndex) + 1) % NUMBER_OF_CARDS;
		winner[*winnerLastCardIndex] = winner[*winnerTopCardIndex];
		*winnerTopCardIndex = ((*winnerTopCardIndex) + 1) % NUMBER_OF_CARDS;
	}

	// a pozniej przeciwnika
	for (int i = 0; i < number_of_cards_on_table; i++){
		*winnerLastCardIndex = ((*winnerLastCardIndex) + 1) % NUMBER_OF_CARDS;
		winner[*winnerLastCardIndex] = loser[*loserTopCardIndex];
		*loserTopCardIndex = ((*loserTopCardIndex) + 1) % NUMBER_OF_CARDS;
		(*winnerDeckSize)++;
		(*loserDeckSize)--;
	}
}

void both_take_cards(int player_A[], int *topCardIndexA, int *lastCardIndexA,
		int player_B[], int *topCardIndexB, int *lastCardIndexB) {
	
	*lastCardIndexA = ((*lastCardIndexA) + 1) % NUMBER_OF_CARDS;
	player_A[*lastCardIndexA] = player_A[*topCardIndexA];
	*topCardIndexA = ((*topCardIndexA) + 1) % NUMBER_OF_CARDS;

	*lastCardIndexB = ((*lastCardIndexB) + 1) % NUMBER_OF_CARDS;
	player_B[*lastCardIndexB] = player_B[*topCardIndexB];
	*topCardIndexB = ((*topCardIndexB) + 1) % NUMBER_OF_CARDS;
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

