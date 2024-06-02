#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define OWNER_MAX_SIZE 100

enum VehicleType {
    car,
    bus,
    truck
};

enum VehicleDrive {
    electric,
    hybrid,
    combustion
};

union Data{
    // car
    int maxPeople;
    float enginePower;
    
    // bus
    int seats;
    int standingPlaces;

    // truck
    float engineCapacity;
    float maxPresure;
};

typedef struct Vehicle {
    char owner[OWNER_MAX_SIZE];
    int visitDate[3];
    enum VehicleType type;
    enum VehicleDrive drive;
    union Data data1;
    union Data data2;
} Vehicle;

void new_owner(Vehicle *pvehicle){
    printf("please enter new owner: ");
    int c;
    int idx = 0;

    while (EOF != (c = fgetc(stdin)))
    {
        if ('\n' == c){
            pvehicle->owner[idx] = '\0';
            break;
        }

        pvehicle->owner[idx++] = (char)c;

    }
    
}

int is_delayed(int date[], int referenceDate[]){
    printf("visit date: %02d-%02d-%04d\n", date[0], date[1], date[2]);
    printf("visit reference date: %02d-%02d-%04d\n", referenceDate[0], referenceDate[1], referenceDate[2]);
    if (referenceDate[2] > date[2])
        return 1;
    else if (referenceDate[2] < date[2])
        return 0;
    else if (referenceDate[1] > date[1])
        return 1;
    else if (referenceDate[1] < date[1])
        return 0;
    else if (referenceDate[0] > date[0])
        return 1;
    
    return 0;
}

int delayed(Vehicle *pvehicle, int size, int *baseDate, char ***delayedOwners){
    int delayedCnt = 0;
    
    for(int i = 0; i < size; i++){
        delayedCnt += is_delayed(pvehicle[i].visitDate, baseDate);
    }

    if (0 == delayedCnt){
        *delayedOwners = NULL;
    }
    else {
        *delayedOwners = malloc(sizeof(char *) * delayedCnt);
        int ownerIdx = 0;

        for (int i = 0; i < size; i ++){
            if (is_delayed(pvehicle[i].visitDate, baseDate)){
                printf("is delayed: %s\n", pvehicle[i].owner);
                (*delayedOwners)[ownerIdx++] = pvehicle[i].owner;
            }
        }
    }

    return delayedCnt;
}

void print_vehicle(Vehicle *pvehicle){
    printf("Vehicle:\n");
    printf("owner: %s\n", pvehicle->owner);
    printf("visit date: %02d-%02d-%04d\n", pvehicle->visitDate[0], pvehicle->visitDate[1], pvehicle->visitDate[2]);
    printf("type: %d\n", pvehicle->type);
    printf("drive: %d\n", pvehicle->drive);

    switch (pvehicle->type)
    {
    case car:
        printf("max people: %d\n", pvehicle->data1.maxPeople);
        printf("engine power: %f\n", pvehicle->data2.enginePower);
        break;
    case bus:
        printf("seats: %d\n", pvehicle->data1.seats);
        printf("standing places: %d\n", pvehicle->data2.standingPlaces);
        break;
    case truck:
        printf("engine capacity: %f\n", pvehicle->data1.engineCapacity);
        printf("max axis presure: %f\n", pvehicle->data2.maxPresure);
        break;
    
    default:
        printf("Unknow type of vehicle!!!\n");
    }
}

int main(){
    union Data data1;
    union Data data2;
    data1.maxPeople = 5;
    data2.enginePower = 250.5;
    Vehicle vehicle1 = {"James Bond", {1,6,2024}, car, combustion, data1, data2};
    data1.seats = 20;
    data2.standingPlaces = 10;
    Vehicle vehicle2 = {"Jack Smith", {7,5,2024}, bus, hybrid, data1, data2};
    data1.engineCapacity = 50.6;
    data2.maxPresure = 60.2;
    Vehicle vehicle3 = {"Hans Miller", {8,7,2024}, truck, electric, data1, data2};

    Vehicle vehicleArray[] = {vehicle1, vehicle2, vehicle3};

    for (int i = 0; i < 3; i++){
        print_vehicle(&vehicleArray[i]);
        printf("\n");
    }

    new_owner(&vehicle1);

    print_vehicle(&vehicle1);

    int baseDate[3] = {2,6,2024};
    char **delayedOwners = NULL;
    int delayedCnt = delayed(vehicleArray, 3, baseDate, &delayedOwners);

    printf("number of delayed owners: %d\n", delayedCnt);

    for (int i = 0; i < delayedCnt; i++){
        printf("%s is late\n", delayedOwners[i]);
    }

    if (delayedOwners != NULL)
        free(delayedOwners);

    return 0;
}