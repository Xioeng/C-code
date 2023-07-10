#include<stdio.h>
#include<time.h> 
#include<stdlib.h>
#include<assert.h>


//Double linked list as structure
typedef struct double_linked_list{
    int data;
    struct double_linked_list *next;
    struct double_linked_list *previous;
}double_linked_list;

//Double linked list creation
double_linked_list* create_list(int d){
    double_linked_list* head = malloc(sizeof(double_linked_list));
    head -> data = d;
    head -> next = NULL;
    head -> previous = NULL;
    return head;
}

//Adding elements at the end
double_linked_list* add_at_end(int d, double_linked_list* list){
    double_linked_list* new = create_list(d);
    double_linked_list* traverser = list;
    while ((traverser -> next) !=NULL){traverser = traverser -> next;} //goes to the list tail
    //Adds new element
    traverser -> next = new; 
    new -> previous = traverser;
    return list;
}

//Converts array to double linked list
double_linked_list* array_to_double_linked_list(int data[], int array_size){
    assert(array_size > 0);
    double_linked_list* head = create_list(data[0]);
    for (int i = 1; i < array_size; i++){
        head = add_at_end(data[i], head);
    }
    return head;
}

//Generates an array of random integers in the interval [0, upper_limit]
int* sample_random_samples(int size, int upper_limit){
    assert(upper_limit>0);
    int* number_list = (int*)malloc(size*sizeof(int));
    for (int i = 0; i < size; i++){number_list[i] = rand() % upper_limit;}
    return number_list;
}

//Converts any integer array to a double linked list
double_linked_list* array_to_double_linked(int array[], int size){
    assert(size > 0);
    double_linked_list* list = create_list(array[0]);
    for(int i = 1; i < size; i++){list = add_at_end(array[i], list);}
    return list;
}

//Deletes a fixed d repeated element in the linked list (if any)
double_linked_list* delete_repeated_element(int d, double_linked_list* list){
    int count = 0;
    double_linked_list* head = list;

    while (list != NULL){ 
        if (count == 1 && list->data == d){
            (list -> previous) -> next = list -> next;
            if((list -> next)!=NULL){(list -> next) -> previous = list ->previous;}} //If the tail is needed to be removed
            
        if (count == 0 && list->data == d){count++;}
        list = list -> next;
        }
    return head;
}

//Removes every repeated element in the interval [0, upper_bound]
double_linked_list* delete_repeated_elements(double_linked_list* list, int upper_bound){
    for(int i = 0; i < upper_bound; i++){delete_repeated_element(i, list); }
    return list;
}                  

void print_array(int array[], int size){
    assert(size > 0);
    for (int i = 0; i < size; i++){printf("%d ", array[i]);}
}

void print_double_linked(double_linked_list* list){
    while (list!=NULL){
        printf("%d ", list -> data);
        list = list -> next;}
}

void main(void){
    int n_numbers = 200;
    int mod = 49;
    int* number_list = sample_random_samples(n_numbers, mod);
    double_linked_list* list = array_to_double_linked(number_list, n_numbers);
    printf("Original linked list:\n");
    print_double_linked(list);


    printf("\n-----------------------------------------\n\
Depured linked list:\n");
    delete_repeated_elements(list, mod);
    print_double_linked(list);

}