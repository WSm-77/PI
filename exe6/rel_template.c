#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100
#define true 1
#define false 0

#define TEST 1

typedef int my_bool;

typedef struct {
	int first;
	int second;
} pair;

int add_relation (pair *tab, int n, pair new_pair);

int read_relation(pair *relation);

void print_int_array(int *array, int n);

my_bool is_reflexive(pair *tab, int n);

my_bool is_irreflexive(pair *tab, int n);

my_bool is_symmetric(pair *tab, int n);

my_bool is_antisymmetric(pair *tab, int n);

my_bool is_asymmetric(pair *tab, int n);

my_bool is_transitive(pair *tab, int n);

my_bool is_partial_order(pair *tab, int n);

my_bool is_total_order(pair *tab, int n);

int find_max_elements(pair *tab, int n, int *max_elements);

int find_min_elements(pair *tab, int n, int *min_elements);

int get_domain(pair *tab, int n, int *domain);

my_bool is_connected(pair *tab, int n);

int composition (pair *rel_1, int n1, pair *rel_2, int n2, pair *rel_3);

void add_ascending(int tab[], int *lastElemIdx, int elem){
	int i = 0;
	while (tab[i] < elem){
		i++;
	}
	
	if (elem != tab[i]){
		while (i < *lastElemIdx){
			int tmp = tab[i];
			tab[i] = elem;
			elem = tmp;
			i++;
		}
		tab[(*lastElemIdx)++] = elem;
	}
}

// Add pair to existing relation if not already there
int add_relation (pair *tab, int n, pair new_pair) {
	for (int i = 0; i < n; i++){
		if(tab[i].first == new_pair.first && tab[i].second == new_pair.second){
			return 0;
		}
	}

	tab[n] = new_pair;

	return 1;
 }

// Read number of pairs, n, and then n pairs of ints
int read_relation(pair *relation) {
	int n;
	int lastElem = 0;

	#if TEST
		printf("enter relation size: ");
	#endif
	scanf("%d", &n);

	for (int i = 0; i < n; i++){
		pair new_pair;
		#if TEST
			printf("enter 2 ints separated with space: ");
		#endif

		scanf("%d %d", &new_pair.first, &new_pair.second);

		#if TEST
			printf("entered pair %d: (%d, %d)\n", i, new_pair.first, new_pair.second);
		#endif

		lastElem += add_relation(relation, lastElem, new_pair);
	}

	return lastElem;
	// return n;
}

void print_int_array(int *array, int n) {
	printf("%d\n", n);
	for (int i = 0; i < n; ++i) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

//------------------------------------------------
// Case 1:

// The relation R is reflexive if xRx for every x in X
my_bool is_reflexive(pair *tab, int n) {
	int domain[MAX_REL_SIZE];
	int domainSize = get_domain(tab, n, domain);

	for (int j = 0; j < domainSize; j++){
		int x = domain[j];
		my_bool isReflexive = false;

		for (int i = 0; i < n; i++){
			if (tab[i].first == x && tab[i].second == x){
				isReflexive = true;
				break;
			}
		}

		if (!(isReflexive)){
			return false;
		}
	}

	return true;
}

// The relation R on the set X is called irreflexive
// if xRx is false for every x in X
my_bool is_irreflexive(pair *tab, int n) {
	for (int i = 0; i < n; i++){
		if (tab[i].first == tab[i].second){
			return false;
		}
	}

	return true;
 }

// A binary relation R over a set X is symmetric if:
// for all x, y in X xRy <=> yRx
my_bool is_symmetric(pair *tab, int n) {
	for (int i = 0; i < n; i++){
		my_bool found = false;
		for (int j = 0; j < i; j++){
			if (tab[i].first == tab[j].second && tab[i].second == tab[j].first){
				found = true;
				break;
			}
		}

		if (found){
			continue;
		}

		for (int j = i + 1; j < n; j++){
			if (tab[i].first == tab[j].second && tab[i].second == tab[j].first){
				found = true;
				break;
			}
		}

		if (!(found)){
			return false;
		}

	}

	return true;
 }

// A binary relation R over a set X is antisymmetric if:
// for all x,y in X if xRy and yRx then x=y
my_bool is_antisymmetric(pair *tab, int n) {
	for (int i = 0; i < n; i++){
		if (tab[i].first == tab[i].second){
			continue;
		}

		for (int j = 0; j < n; j++){
			if(tab[i].first == tab[j].second && tab[i].second == tab[j].first){
				return false;
			}
		}
	}

	return true;
 }

// A binary relation R over a set X is asymmetric if:
// for all x,y in X if at least one of xRy and yRx is false
my_bool is_asymmetric(pair *tab, int n) {
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			if (tab[i].first == tab[j].second && tab[i].second == tab[j].first){
				return false;
			}
		}
	}

	return true;
 }

// A homogeneous relation R on the set X is a transitive relation if:
// for all x, y, z in X, if xRy and yRz, then xRz
my_bool is_transitive(pair *tab, int n) {
	for(int i = 0; i < n; i++){
		int x = tab[i].first;
		int y = tab[i].second;

		for (int j = 0; j < n; j++){
			if (tab[j].first != y){
				continue;
			}

			int z = tab[j].second;
			my_bool found = false;

			for (int k = 0; k < n; k++){
				if (tab[k].first == x && tab[k].second == z){
					found = true;
				}
			}

			if (!(found)){
				return false;
			}
		}
	}

	return true;
 }

//------------------------------------------------

// Case 2:

// A partial order relation is a homogeneous relation that is
// reflexive, transitive, and antisymmetric
my_bool is_partial_order(pair *tab, int n) {
	my_bool isReflexive = is_reflexive(tab, n);
	my_bool isAntisymmetric = is_antisymmetric(tab, n);
	my_bool isTransitive = is_transitive(tab, n);

	#if TEST
		printf("isReflexive: %d, isAntisymmetric: %d, isTransitive: %d\n", isReflexive, isAntisymmetric, isTransitive);
	#endif

	return (isReflexive && isAntisymmetric && isTransitive);
}

// A total order relation is a partial order relation that is connected
my_bool is_total_order(pair *tab, int n) { 
	return (is_partial_order(tab, n) && is_connected(tab, n));
}

int find_max_elements(pair *tab, int n, int *max_elements) { // tab - partial order
	int domain[MAX_REL_SIZE];
	int domainSize = get_domain(tab, n, domain);

	int maxElemCnt = 0;
	
	for (int i = 0; i < domainSize; i++){
		int candidate = tab[i].second;

		my_bool isMaxElem = true;

		for (int j = 0; j < n; j++){
			if (tab[j].first != candidate){
				continue;
			}

			if (!(tab[j].first == tab[j].second)){
				isMaxElem = false;
			}

		}

		if (isMaxElem){
			add_ascending(max_elements, &maxElemCnt, candidate);
		}
	}

	return maxElemCnt;
}

int find_min_elements(pair *tab, int n, int *min_elements) { // tab - strong partial order
	return 0;
}

int get_domain(pair *tab, int n, int *domain) { 
	int domainSize = 0;
	for (int i = 0; i < n; i++){
		// int x = tab[i].first;
		// int y = tab[i].second;
		// my_bool inDomainX = false;
		// my_bool inDomainY = false;

		// for (int j = 0; j < domainSize; j++){
		// 	if (x == domain[j]){
		// 		inDomainX = true;
		// 	}
		// 	else if (y == domain[j])
		// 		inDomainY = true;
		// }

		// if ( ! (inDomainX)){
		// 	add_ascending(domain, &domainSize, x);
		// }

		// if (x != y && !(inDomainY)){
		// 	add_ascending(domain, &domainSize, y);
		// }

		add_ascending(domain, &domainSize, tab[i].first);
		add_ascending(domain, &domainSize, tab[i].second);
	}

	#if TEST
		printf("domian: ");
		for (int i = 0; i < domainSize; i++){
			printf("%d ", domain[i]);
		}
		printf("\n");
	#endif

	return domainSize;
}


// Relation R is connected if for each x, y in X:
// xRy or yRx (or both)
my_bool is_connected(pair *tab, int n) { 
	int domain[MAX_REL_SIZE];
	int domainSize = get_domain(tab, n, domain);

	for (int i = 0; i < domainSize; i++){
		for (int j = 0; j < domainSize; j++){
			int x = domain[i];
			int y = domain[j];

			my_bool found = false;

			for (int k = 0; k < n; k++){
				if((tab[k].first == x && tab[k].second == y) || (tab[k].first == y && tab[k].second == x)){
					found = true;
					break;
				}
			}

			if (!(found)){
				return false;
			}
		}
	}

	return true;
}

//------------------------------------------------

// Case 3:

// x(S o R)z iff exists y: xRy and ySz
int composition (pair *rel_1, int n1, pair *rel_2, int n2, pair *rel_3) { }

int main(void) {
	int to_do;
	pair relation[MAX_REL_SIZE];
	pair relation_2[MAX_REL_SIZE];
	pair comp_relation[MAX_REL_SIZE];
	int domain[MAX_REL_SIZE];
	int max_elements[MAX_REL_SIZE];
	int min_elements[MAX_REL_SIZE];

	scanf("%d",&to_do);
	int size = read_relation(relation);
	int ordered, size_2, n_domain;
	int no_max_elements;
	int no_min_elements;

	switch (to_do) {
		case 1:
			printf("%d ", is_reflexive(relation, size));
			printf("%d ", is_irreflexive(relation, size));
			printf("%d ", is_symmetric(relation, size));
			printf("%d ", is_antisymmetric(relation, size));
			printf("%d ", is_asymmetric(relation, size));
			printf("%d\n", is_transitive(relation, size));
			break;
		case 2:
			ordered = is_partial_order(relation, size);
			n_domain = get_domain(relation, size, domain);
			printf("%d %d\n", ordered, is_total_order(relation, size));
			print_int_array(domain, n_domain);
			if (!ordered) break;
			no_max_elements = find_max_elements(relation, size, max_elements);
			no_min_elements = find_min_elements(relation, size, min_elements);
			print_int_array(max_elements, no_max_elements);
			print_int_array(min_elements, no_min_elements);
			break;
		case 3:
			size_2 = read_relation(relation_2);
			printf("%d\n", composition(relation, size, relation_2, size_2, comp_relation));
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	
	return 0;
}

