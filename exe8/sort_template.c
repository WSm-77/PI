#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	int day; 
	int month; 
	int year;
} Date;

// 1 bsearch2

#define NAME_MAX  20   // max. length of name + 1
#define FOOD_MAX  30   // max. number of goods
#define RECORD_MAX 64  // max. line length

#define TEST 0

typedef struct {
	char name[NAME_MAX];
	float price;
	int amount;
	Date valid_date;
} Food;

typedef int (*ComparFp)(const void *, const void *);

void print_food_tab(Food tab[], int size){
	printf("\n\n########### food tab ###########\n\n");
	for (int i = 0; i < size; i++){
		Food currFood = tab[i];
		printf("%s %.2f %d %02d.%02d.%d\n", currFood.name, currFood.price, currFood.amount, currFood.valid_date.day, currFood.valid_date.month, currFood.valid_date.year);
	}
	printf("\n\n");
}

int cmp_date(const void *d1, const void *d2) {
    Date date1 = *((Date*)d1);
    Date date2 = *((Date*)d2);

    if(date1.year < date2.year)
        return -1;
    else if(date1.year > date2.year)
        return 1;
	else if(date1.month < date2.month)
		return -1;
	else if(date1.month > date2.month)
		return 1;
	else if(date1.day < date2.day)
		return -1;
	else if(date1.day > date2.day)
		return 1;
	else
		return 0;
}

int cmp_food_date(const void *food1, const void *food2){
	Date date1 = ((Food *)(food1))->valid_date;
	Date date2 = ((Food *)(food2))->valid_date;

	return cmp_date(&date1, &date2);
}

int cmp_price(const void* food1, const void* food2){
    Food a = *((Food*)food1);
    Food b = *((Food*)food2);

    if(a.price < b.price)
        return -1;
    else if(a.price > b.price)
        return 1;
    else{
        return 0;
    }
}

int cmp_food(const void* food1Ptr, const void* food2Ptr){
    Food a = *((Food*)food1Ptr);
    Food b = *((Food*)food2Ptr);

    int foodName = strcmp(a.name, b.name);
    if (foodName < 0)
        return -1;
    else if (foodName > 0)
        return 1;

	int price = cmp_price(food1Ptr, food2Ptr);
	if(price < 0)
		return -1;
	else if (price > 0)
		return 1;
		
	int date = cmp_date(food1Ptr, food2Ptr);
	if (date < 0)
		return -1;
	else if (date > 0)
		return 1;
	else
		return 0;
}

void *bsearch2(const void *key, void *base, size_t nitems, size_t size, int
(*compar)(const void*, const void*), char *result)
{
	if(nitems == 0){
		*result = 0;
		return base;
	}

	size_t middle = nitems/2;
	void *middle_ptr = base + middle*size;
	int cmp = compar(key,middle_ptr);

	if(cmp == 0){
		*result = 1;
		return middle_ptr;
	}
	else if(cmp < 0){
		return bsearch2(key,base,middle,size,compar,result);
	}
	else{
		return bsearch2(key, middle_ptr + size, nitems - middle - 1, size, compar, result);
	}
}

void print_art(Food *p, size_t n, const char *name) {
		for(int i = 0; i < n; i++){
		if(strcmp(p[i].name, name) == 0)
			printf("%.2f %.d %02d.%02d.%d \n", p[i].price, p[i].amount, p[i].valid_date.day, p[i].valid_date.month,
                   p[i].valid_date.year);
	}
}

Food* add_record(Food *tab, int *np, const ComparFp compar, Food *newFood) {
	char result;
	size_t foodSize = sizeof(Food);
	Food *ptr = bsearch2(newFood, tab, *np, foodSize, compar, &result);

	if (TEST){
		printf("tab address: %p, bsearch address: %p\n", tab, ptr);
	}

	if (result){
		ptr->amount += newFood->amount;
	}
	else{
		int idx = (int)(ptr - tab);
		(*np)++;

		for (int i = *np - 1; i > idx; i--){
			tab[i] = tab[i - 1];
		}

		if (TEST){
			printf("########### add new record ###########\n");
			printf("adding new food \"%s\" at index: %d\n", newFood->name, idx);
		}

		tab[idx] = *newFood;
	}

	return ptr;
}

int read_goods(Food *tab, const int no, FILE *stream, const int sorted) {
	int np = 0;
	for(int i = 0; i < no; i++){
		Food newFood;
		memset(newFood.name, 0, NAME_MAX);
		fscanf(stream, "%s", newFood.name);
		fscanf(stream, "%f", &newFood.price);
		fscanf(stream, "%d", &newFood.amount);
		fscanf(stream, "%d.%d.%d", &newFood.valid_date.day, &newFood.valid_date.month,&newFood.valid_date.year);
		
		add_record(tab, &np, cmp_food, &newFood);

		if (TEST){
			printf("\n\n########### read goods ###########\n\n");
			printf("newFood name: %s \n", newFood.name);
			printf("newFood price: %f\n", newFood.price);
			printf("newFood amount: %d\n", newFood.amount);
			printf("newFood date: %d.%d.%d\n", newFood.valid_date.day, newFood.valid_date.month, newFood.valid_date.year);
			printf("test: %s, %f, %d, %d.%d.%d\n\n", newFood.name, newFood.price, newFood.amount, newFood.valid_date.day,
					newFood.valid_date.month, newFood.valid_date.year);
			print_food_tab(tab, np);
		}
	}

	if (TEST){
		printf("finished reading goods\n");
		print_food_tab(tab, np);
	}

	return np;
}

int cmp_qs(const void *a, const void *b) {
	const Food *fa = (Food*)a, *fb = (Food*)b;
	return cmp_date(&fa->valid_date, &fb->valid_date);
}

int cmp_bs(const void *a, const void *b) {
	const Date *pd = (Date*)a;
	const Food *fb = (Food*)b;
	return cmp_date(pd, &fb->valid_date);
}


float value(Food *foodTab, size_t n, Date currDate, int anticip) {
    time_t raw_time;
    time(&raw_time);
	struct tm *expireDateTm = localtime(&raw_time);
    expireDateTm->tm_mday = currDate.day;
    expireDateTm->tm_mon = currDate.month - 1;
    expireDateTm->tm_year = currDate.year - 1900;

    mktime(expireDateTm);
    expireDateTm->tm_mday += anticip;
    mktime(expireDateTm);

	if (TEST){
		printf("\n########### before sorting ###########\n");
		print_food_tab(foodTab, n);
	}

    Date expireDate = (Date){expireDateTm->tm_mday, expireDateTm->tm_mon + 1, expireDateTm->tm_year + 1900};
    Food sampleFood;
	sampleFood.valid_date = expireDate;


    qsort(foodTab, n, sizeof(Food), cmp_food_date);
	
	if (TEST){
		printf("\n########### after sorting ###########\n");
		print_food_tab(foodTab, n);
	}

    char res;
    Food* address = bsearch2(&sampleFood, foodTab, n, sizeof(Food), cmp_food_date, &res);

    if(res == 0)
        return 0;

    float result = 0;

    int index = (int)(address - foodTab);
    
	for (int i = index; i < n; i++){
		if (0 == cmp_date(&foodTab[i].valid_date, &expireDate)){
			result += foodTab[i].price * (float)foodTab[i].amount;
		}
		else{
			break;
		}
	}

	for (int i = index - 1; i >= 0; i--){
		if (0 == cmp_date(&foodTab[i].valid_date, &expireDate)){
			result += foodTab[i].price * (float)foodTab[i].amount;
		}
		else{
			break;
		}
	}

    return result;
}

/////////////////////////////////////////////////////////////////
// 3 Succession

#define CHILD_MAX 20

enum Sex {F, M};
enum BOOL {no, yes};

struct Bit_data {
	enum Sex sex:1;
	enum BOOL in_line:1;
};

typedef struct {
	char *name;
	struct Bit_data bits;
	Date born;
	char *parent;
} Person;

typedef struct {
	char *par_name;
	int index_first;
	int index_last;
} Parent;

const Date primo_date = { 28, 10, 2011 }; // new succession act

int cmp_primo(const Person *person1, const Person *person2) {
	if (person1->bits.sex == person2->bits.sex) return 0;
	if (person1->bits.sex == F && cmp_date(&person1->born, &primo_date) > 0) return 0;
	if (person2->bits.sex == F && cmp_date(&person2->born, &primo_date) > 0) return 0;
	return person2->bits.sex - person1->bits.sex;
}

int cmp_person(const void *a, const void *b) {
	Person first = *((Person*)a);
	Person second = *((Person*)b);
	if(first.parent == NULL) return 1;
	if(second.parent == NULL) return -1;
	return strcmp(first.parent,second.parent);
}

int cmp_parent(const void *a, const void *b) {
	Parent first = *((Parent*)a);
	Parent second = *((Parent*)b);
	return strcmp(first.par_name,second.par_name);
}

int fill_indices_tab(Parent *idx_tab, Person *pers_tab, int size) {
	int no_parents = 0;
	for(int i = 0; i < size; i++){
		int found = 0;
		for(int j = 0; j < no_parents; j++){
			if(strcmp(pers_tab[i].name,idx_tab[j].par_name) == 0){
				found = 1;
				break;
			}
		}
		if(!found){
			idx_tab[no_parents].par_name = pers_tab[i].name;
			Person key = {.parent = pers_tab[i].name};
			Person *address = bsearch(&key,pers_tab,size,sizeof(Person),cmp_person);
			if(address == NULL) continue;

			int index = (int)(address - pers_tab);
			int tmp = index;

			while(tmp > 0 && cmp_person(&pers_tab[tmp], &pers_tab[tmp-1]) == 0) tmp--;
			idx_tab[no_parents].index_first = tmp;

			tmp = index;

            while(tmp < size - 1 && cmp_person(&pers_tab[tmp], &pers_tab[tmp+1]) == 0) tmp++;
			idx_tab[no_parents].index_last = tmp;

			no_parents++;
		}
	}

	qsort(idx_tab,no_parents,sizeof(Parent),cmp_parent);
	return no_parents;
}

void persons_shifts(Person *person_tab, const int size, Parent *idx_tab, const int no_parents) {
	int index = 0;
	Person tmp[33];
	
	for(int index = 0; index < size; index++){
		Parent key = {.par_name = person_tab[index].name};
		Parent *address = bsearch(&key,idx_tab,no_parents,sizeof(Parent),cmp_parent);

		if(address != NULL){
			int to_copy = address->index_last - address->index_first + 1;

			memmove(tmp,&person_tab[address->index_first],to_copy*sizeof(Person));
			memmove(&person_tab[index + to_copy + 1], &person_tab[index + 1],
				(address->index_first - index - 1) * sizeof(Person));
			memmove(&person_tab[index + 1], tmp, to_copy * sizeof(Person));

			for(int i = 0; i < no_parents; i++){
				if(idx_tab[i].index_first < address->index_first){
					idx_tab[i].index_first += to_copy;
					idx_tab[i].index_last += to_copy;
				}
			}
		}
	}
}

int cleaning(Person *person_tab, int n) {
	for(int i = 0; i < n; i++){
		if(person_tab[i].bits.in_line == no){
			memmove(&person_tab[i],&person_tab[i+1], (n-i)*sizeof(Person));
			i--;
			n--;
		}
	}
}

void print_person(const Person *p) {
	printf("%s\n", p->name);
}

int compare(const void *a, const void *b){
	Person first = *((Person*)a);
	Person second = *((Person*)b);

	if(first.parent == NULL) return -1;
	if(second.parent == NULL) return 1;

	int parent = strcmp(first.parent, second.parent);

	if(parent < 0) return -1;
	else if(parent > 0) return 1;
	else{
		int birth_date = cmp_date(&first.born, &second.born);
		if(cmp_date(&first.born, &primo_date) < 0 && cmp_date(&second.born, &primo_date) < 0){
			if(first.bits.sex == M && second.bits.sex == F) return -1;
			else if(first.bits.sex == F && second.bits.sex == M) return 1;
		}

		if(birth_date < 0) return -1;
		else if(birth_date > 0) return 1;
		else{
			if(first.bits.sex > second.bits.sex) return 1;
			else if(first.bits.sex < second.bits.sex) return -1;
			else return 0;
		}
	}
}

int create_list(Person *person_tab, int n) {
	qsort(person_tab,n,sizeof(Person),compare);
	Parent parents_tab[33];
	int no_parents = fill_indices_tab(parents_tab,person_tab,n);

	persons_shifts(person_tab,n,parents_tab,no_parents);
	return cleaning(person_tab,n);
}

int main(void) {
	Person person_tab[] = {
		{"Charles III", {M, no}, {14, 11, 1948},"Elizabeth II"},
		{"Anne", {F,yes}, {15, 8, 1950},"Elizabeth II"},
		{"Andrew", {M,yes}, {19, 2, 1960},"Elizabeth II"},
		{"Edward", {M,yes}, {10, 3, 1964} ,"Elizabeth II"},
		{"David", {M,yes}, {3, 11, 1961} ,"Margaret"},
		{"Sarah", {F,yes}, {1, 5, 1964}, "Margaret"},
		{"William", {M,yes}, {21, 6, 1982}, "Charles III"},
		{"Henry", {M,yes}, {15, 9, 1984}, "Charles III"},
		{"Peter", {M,yes}, {15, 11, 1977}, "Anne"},
		{"Zara", {F,yes}, {15, 5, 1981}, "Anne"},
		{"Beatrice", {F,yes}, {8, 8, 1988}, "Andrew"},
		{"Eugenie", {F,yes}, {23, 3, 1990}, "Andrew"},
		{"James", {M,yes}, {17, 12, 2007}, "Edward"},
		{"Louise", {F,yes}, {8, 11, 2003}, "Edward"},
		{"Charles", {M,yes}, {1, 7, 1999}, "David"},
		{"Margarita", {F,yes}, {14, 5, 2002}, "David"},
		{"Samuel", {M,yes}, {28, 7, 1996}, "Sarah"},
		{"Arthur", {M,yes}, {6, 5, 2019}, "Sarah"},
		{"George", {M,yes}, {22, 7, 2013}, "William"},
		{"George VI", {M,no}, {14, 12, 1895}, NULL},
		{"Charlotte", {F,yes}, {2, 5, 2015}, "William"},
		{"Louis", {M,yes}, {23, 4, 2018}, "William"},
		{"Archie", {M,yes}, {6, 5, 2019}, "Henry"},
		{"Lilibet", {F,yes}, {4, 6, 2021}, "Henry"},
		{"Savannah", {F,yes}, {29, 12, 2010}, "Peter"},
		{"Isla", {F,yes}, {29, 3, 2012}, "Peter"},
		{"Mia", {F,yes}, {17, 1, 2014}, "Zara"},
		{"Lena", {F,yes}, {18, 6, 2018}, "Zara"},
		{"Elizabeth II", {F,no}, {21, 4, 1926}, "George VI"},
		{"Margaret", {F,no}, {21, 8, 1930}, "George VI"},
		{"Lucas", {M,yes}, {21, 3, 2021}, "Zara"},
		{"Sienna", {F,yes}, {18, 9, 2021}, "Beatrice"},
		{"August", {M,yes}, {9, 2, 2021}, "Eugenie"},
		{"Ernest", {M,yes}, {30, 5, 2023}, "Eugenie"}
	};

	int to_do, no;
	int n;
	Food food_tab[FOOD_MAX];
	char buff[RECORD_MAX];
	fgets(buff, RECORD_MAX, stdin);
	sscanf(buff, "%d", &to_do);

	switch (to_do) {
		case 1:  // bsearch2
			fgets(buff, RECORD_MAX, stdin);
			sscanf(buff, "%d", &no);
			n = read_goods(food_tab, no, stdin, 1);
			scanf("%s",buff);
			print_art(food_tab, n, buff);
			break;
		case 2: // qsort
			fgets(buff, RECORD_MAX, stdin);
			sscanf(buff, "%d", &no);
			n = read_goods(food_tab, no, stdin, 0);
			Date curr_date;
			int days;
			scanf("%d %d %d", &curr_date.day, &curr_date.month, &curr_date.year);
			scanf("%d", &days);
			printf("%.2f\n", value(food_tab, (size_t)n, curr_date, days));
			break;
		case 3: // succession
			scanf("%d", &no);
			const int no_persons = sizeof(person_tab) / sizeof(Person);
			create_list(person_tab,no_persons);
			print_person(person_tab + no - 1);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
	}
	return 0;
}

