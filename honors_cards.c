#include<stdio.h>
#include <time.h> //Both libraries for generating random integers 
#include <stdlib.h>
#include <assert.h>


const int Cards_in_hand = 7; // Cards in a poker hand
long int n_card_draws = 1000000;

char suits_array[][15] = {"Clubs", "Diamonds", "Hearts", "Spades"};
char values_array[][10] = {"Ace", "Two", "Three", "Four", "Five", "Six", "Seven", 
                    "Eight", "Nine", "Ten", "J", "Q", "K"};

typedef enum suit {clubs, diamonds, hearts, spades} Suit;
typedef enum value {ace, two, three, four, five, six, seven, eight, nine, 
                    ten, J, Q, K} Value;

typedef struct card {Suit s; Value v;} card;

//Auxiliary functions

void print_array(int array[], int size){
    for (int i = 0; i < size; i++){
        printf("%d ", array[i]);
    }
}


void print_hand_of_cards(card hand_of_cards[]){
    for (int i = 0; i < Cards_in_hand; i++){
        printf("Card %d: %s of %s\n", i+1,
        values_array[hand_of_cards[i].v], suits_array[hand_of_cards[i].s]);}
    printf("\n");
}

void print_hand_of_cards_size(card hand_of_cards[], int size){
    for (int i = 0; i < size; i++){
        printf("Card %d: %s of %s\n", i+1,
        values_array[hand_of_cards[i].v], suits_array[hand_of_cards[i].s]);}
    printf("\n");
}

int element_in_array(int array[], int element, int size){
    for (int i = 0; i < size; i++){
        if (array[i]==element){
            return 1;} 
    }
    return 0;
}

// Creates a hand of cards taking out the cards indexed by the indices array
void create_hand_of_cards_without_indices(card hand_of_cards[], card new_hand[], int indices[], 
                                          int size_old, int size_new, int size_indices){
    
    for (int i = 0, j = 0; i < size_old; i++){
        if (!element_in_array(indices, i, size_indices )){
            new_hand[j++] = hand_of_cards[i];}
    }
}

// Checking if a card is in a hand of cards
int there_is_card(card hand_of_cards[], card target_card, int* ptr_position, int size){
    for (int i = 0; i < size; i++){
        card current_card = hand_of_cards[i];
        if (current_card.s == target_card.s && current_card.v == target_card.v){
            *ptr_position = i;
            return 1;}
        }
    return 0;
}
int no_repeated_card(card hand_of_cards[]){
    for (int i = 0; i < Cards_in_hand; i++){
        for (int j = i+1; j< Cards_in_hand; j++){
            if (hand_of_cards[i].s == hand_of_cards[j].s && hand_of_cards[i].v == hand_of_cards[j].v){
                return 0;
        }
    }
}   
    return 1;
}

// Draw without replacement
void draw_cards(card hand_of_cards[]){
    int value, suit, none;
    card sample_card;

    for (int i = 0; i < Cards_in_hand; i++){
        while (1){
            value = rand() %  13;
            suit = rand() % 4;
            sample_card.s = suit;
            sample_card.v = value;
            if ( (i==0) || !there_is_card(hand_of_cards, sample_card, &none, i) ){
                break;}   
        }
        hand_of_cards[i] = sample_card;}
}

int there_is_card_without_size(card hand_of_cards[], card target_card, int* ptr_position){
    return there_is_card(hand_of_cards, target_card, ptr_position, Cards_in_hand);
}



int there_is_Pair(card hand_of_cards[], int position[], int hand_size){

    assert((position[0] == -1) && (position[1] == -1)); // Contains the positions in the hand where the pair was found

    for (int i = 0; i < hand_size; i++){
        card Current_card = hand_of_cards[i];
        card hand_without_card[Cards_in_hand-1];
        int indices[1] = {i}, count = 1;
        
        create_hand_of_cards_without_indices(hand_of_cards, hand_without_card, indices,
                                            hand_size, hand_size-1, 1 );
        for(int j = 0; j < hand_size-1; j++){
            if ((Current_card.v == hand_without_card[j].v) && (!(Current_card.s == hand_without_card[j].s))){
                position[0] = i;
                position[1] = j+1;
                return 1;}   
        }
    }
    return 0;
}

int there_is_NoPair(card hand_of_cards[]){
    int position[2] = {-1, -1};
    return !there_is_Pair(hand_of_cards, position, Cards_in_hand);
}

int there_is_TwoPair(card hand_of_cards[]){
    int position1[2] = {-1, -1}; // Where the first pair is
    int position2[2] = {-1, -1}; // Where the second pair is
    card hand_without_positions[Cards_in_hand-2];

    if (there_is_NoPair(hand_of_cards)){
        return 0;}
    there_is_Pair(hand_of_cards, position1, Cards_in_hand);
    create_hand_of_cards_without_indices(hand_of_cards, hand_without_positions, position1,
                                         Cards_in_hand, Cards_in_hand-2, 2 );

    if (there_is_Pair(hand_without_positions, position2, Cards_in_hand-2)){
        return 1;}
    else{
        return 0;}
}

int there_is_ThreeOfAKind(card hand_of_cards[], int hand_size){
    int indices[1], count;
    card Current_card;

    for (int i = 0; i < hand_size; i++){
        Current_card = hand_of_cards[i];
        card hand_without_card[Cards_in_hand-1];
        count = 1;
        indices[0] = i;
        create_hand_of_cards_without_indices(hand_of_cards, hand_without_card, indices,
                                            Cards_in_hand, Cards_in_hand-1, 1);
        // For each card in the hand we count how many cards in the remaining hand have the same value
        for (int j = 0; j < hand_size-1; j++){
            if (hand_without_card[j].v == Current_card.v){
                count++;}
        }
        if (count >= 3){ // If at least we have three cards with the same value
            return 1;}
        }
    return 0;
}

int there_is_Flush(card hand_of_cards[]){
    int suits_counts[4] = {0, 0, 0, 0};

    for (int i = 0; i < Cards_in_hand; i++){
        suits_counts[hand_of_cards[i].s]++;
    }
    for (int j = 0; j < 4; j++){
        if (suits_counts[j] >= 5){
            return 1;}
    }
    return 0;
}

// Only for being used when computing a full house (a more complex version than the last function)
int there_is_ThreeOfAKind_forFH(card hand_of_cards[], int hand_size, int position[]){

    assert((position[0] == -1) && (position[1] == -1) && (position[2] == -1)); // Contains the positions in the hand where the three of a kind was found
    int indices[1], count, crd_1, crd_2, crd_3;
    card Current_card;

    for (int i = 0; i < hand_size; i++){
        Current_card = hand_of_cards[i];
        card hand_without_card[Cards_in_hand-1];
        count = 1;
        indices[0] = i;
        crd_1 = i;
        create_hand_of_cards_without_indices(hand_of_cards, hand_without_card, indices,
                                            Cards_in_hand, Cards_in_hand-1, 1);
        // For each card in the hand we count how many cards in the remaining hand have the same value
        for (int j = 0; j < hand_size-1; j++){
            
            if (hand_without_card[j].v == Current_card.v){
                count++;
                if (count == 2){
                    crd_2 = j+1;}
                if ( count == 3){
                    crd_3 = j+1;}
                }
        }
        if (count >= 3){ // If at least we have three cards with the same value
            position[0] = crd_1, position[1] = crd_2, position[2] = crd_3;
            return 1;}
        }
    return 0;
}


int there_is_FullHouse(card hand_of_cards[]){
    int three_of_positions[3] = {-1, -1, -1};
    card hand_without_positions[Cards_in_hand-3];

    int three_of_a_kind = there_is_ThreeOfAKind_forFH(hand_of_cards, Cards_in_hand, three_of_positions);

    if (!three_of_a_kind){
        return 0;}  // If there is no three of a kind, there is no full house

    // there_is_Pair(hand_of_cards, pair_position, Cards_in_hand);
    create_hand_of_cards_without_indices(hand_of_cards, hand_without_positions, three_of_positions,
                                         Cards_in_hand, Cards_in_hand-3, 3 );
    three_of_positions[0] = three_of_positions[1] = -1;
    if (there_is_Pair(hand_without_positions, three_of_positions, Cards_in_hand-3)){
        return 1;} // There are a three of a kind and a pair
    else{
        return 0;} // Although there is a three of a kind, if there are no pairs, there is no full house
}

int there_is_FourOfAKind(card hand_of_cards[]){
    int indices[1], count;
    card Current_card;

    for (int i = 0; i < Cards_in_hand; i++){
        Current_card = hand_of_cards[i];
        card hand_without_card[Cards_in_hand-1];
        count = 1;
        indices[0] = i;
        create_hand_of_cards_without_indices(hand_of_cards, hand_without_card, indices,
                                            Cards_in_hand, Cards_in_hand-1, 1);
        // For each card in the hand we count how many cards in the remaining hand have the same value
        for (int j = 0; j < Cards_in_hand-1; j++){
            if (hand_without_card[j].v == Current_card.v){
                count++;}
        }
        if (count >= 4){// If at least we have four cards with the same value
            return 1;}
        }
    return 0;
}

int there_is_StraightFlush(card hand_of_cards[]){
    card Card_1 = {clubs, ace}, Card_2 = {clubs, K}, Card_3 = {clubs, Q}, Card_4 = {clubs, J}, Card_5 = {clubs, ten};
    int expression, _;

    for (int suit = 0; suit < 4; suit++){
        Card_1.s = Card_2.s = Card_3.s = Card_4.s = Card_5.s = suit;
        for (int value = 0; value < 9; value ++){ // Until value 8 that means a card with a nine
        Card_1.v = value;
        Card_2.v = value + 1;
        Card_3.v = value + 2;
        Card_4.v = value + 3;
        Card_5.v = value + 4;
        
        expression = ( there_is_card_without_size(hand_of_cards, Card_1, &_) && there_is_card_without_size(hand_of_cards, Card_2, &_) \
                   && there_is_card_without_size(hand_of_cards, Card_3, &_) && there_is_card_without_size(hand_of_cards, Card_4, &_) \
                   && there_is_card_without_size(hand_of_cards, Card_5, &_) );

        if (expression){
            return 1;}
        }
    }
    return 0;
    }

int there_is_RoyalFlush(card hand_of_cards[]){
    card Ace = {clubs, ace}, King = {clubs, K}, Queen = {clubs, Q}, Joker = {clubs, J}, Ten = {clubs, ten};
    int expression, _;

    for (int suit = 0; suit < 4; suit++){
        Ace.s = King.s = Queen.s = Joker.s = Ten.s = suit;
        expression = ( there_is_card_without_size(hand_of_cards, Ace, &_) && there_is_card_without_size(hand_of_cards, Ace, &_) \
                   && there_is_card_without_size(hand_of_cards, Ace, &_) && there_is_card_without_size(hand_of_cards, Ace, &_) \
                   && there_is_card_without_size(hand_of_cards, Ace, &_) );
        if (expression){
            return 1;}
    }
    return 0;
    }

int there_is_Straight(card hand_of_cards[]){

    if ( there_is_StraightFlush(hand_of_cards) || there_is_RoyalFlush(hand_of_cards)){
    return 0;}
    
    int value_counts[13] = {0};
}


// Test function, only for debugging purposes, do not read
void test(){
    // srand(12345);
    for (int i=0; i<1; i++){
    int positions[3] = {-1, -1, -1};
    // card hand_of_cards[Cards_in_hand];
    // draw_cards(hand_of_cards);
    // print_hand_of_cards(hand_of_cards);
    card test_card ={0, 7};
    card King = {clubs, six}, Queen ={diamonds, six}, Joker = {diamonds, five};
    card Ace = {diamonds, nine}, Ten = {hearts, six};
    card Eight = {clubs, ten}, Seven = {spades, five};
    card hand_of_cards[] = {King, Queen, Joker, Ace, Ten, Eight, Seven };
    print_hand_of_cards(hand_of_cards);
    // printf("%d\n", there_is_FourOfAKind(hand_of_cards2c, Cards_in_hand, positions));
    // printf("%d", there_is_RoyalFlush(hand_of_cards2));

    // print_array(positions, 2);
    printf("Pair? %d\n", there_is_Pair(hand_of_cards, positions, Cards_in_hand ));
    positions[0] = positions[1] = positions[2] =-1;
    // print_array(positions, 2);
    printf("Two Pair? %d\n", there_is_TwoPair(hand_of_cards));
    printf("Three of a kind? %d\n", there_is_ThreeOfAKind(hand_of_cards, Cards_in_hand));
    printf("Full house? %d\n", there_is_FullHouse(hand_of_cards));
    printf("4 of a kind? %d\n", there_is_FourOfAKind(hand_of_cards));
    
    
    printf("3 of a kind FH? %d\n\n", there_is_ThreeOfAKind_forFH(hand_of_cards, Cards_in_hand, positions));
    print_array(positions, 3);
    positions[0] = positions[1] = positions[2] =-1;
    // print_array(positions, Cards_in_hand);
    }
}

void compute_probs(){
    srand(12345);
    
    double no_pairs, pairs, two_pairs, three_ofs, full_houses, four_ofs;
    no_pairs = pairs = two_pairs = three_ofs = full_houses = four_ofs = 0;
    int positions[3];
    for (int draw = 0; draw < n_card_draws; draw++){
        positions[0] = positions[1] = positions[2] =-1;
        card drew_hand[Cards_in_hand];
        draw_cards(drew_hand);
        assert(no_repeated_card(drew_hand));
        
        if (there_is_FourOfAKind(drew_hand)){
            four_ofs++;
            // printf("draw %d 4 kind\n", draw);
            // print_hand_of_cards(drew_hand);
            continue;}
        if (there_is_FullHouse(drew_hand)){
            full_houses++;
            positions[0] = positions[1] = positions[2] =-1;
            // printf("draw %d full\n", draw);
            // print_hand_of_cards(drew_hand);
            continue;}
        if (there_is_ThreeOfAKind(drew_hand, Cards_in_hand)){
            three_ofs++;
            // printf("draw %d 3 kind\n", draw);
            // print_hand_of_cards(drew_hand);
            continue;}
        if (there_is_TwoPair(drew_hand)){
            // printf("%d--", there_is_TwoPair(drew_hand));
            two_pairs++;
            // printf("draw %d 2 pairs\n", draw);
            // print_hand_of_cards(drew_hand);
            continue;}
        if (there_is_Pair(drew_hand, positions, Cards_in_hand)){
            pairs++;
            continue;}
            // printf("draw %d pairs\n", draw);
            // print_hand_of_cards(drew_hand);
           
        if (there_is_NoPair(drew_hand)){
            no_pairs++;
            continue;}
        // printf("draw %d else\n", draw);
        // print_hand_of_cards(drew_hand);
    }

    printf("No pairs ratio: %.2f %%, %.0f\n", 100*no_pairs/n_card_draws, no_pairs);
    printf("Pairs ratio: %.2f %%, %.0f\n", 100*pairs/n_card_draws, pairs);
    printf("Two pairs ratio: %.2f %%, %.0f\n", 100*two_pairs/n_card_draws, two_pairs);
    printf("3 of a kind ratio: %.2f %%, %.0f\n", 100*three_ofs/n_card_draws, three_ofs);
    printf("Full house ratio: %.2f %%, %.0f\n", 100*full_houses/n_card_draws, full_houses);
    printf("4 of a kind ratio: %.2f %%, %.0f\n", 100*four_ofs/n_card_draws, four_ofs);

}
int main(void){
    // test();
    compute_probs();
    // int k[1]={3};
    // RAND_MAX = 13*4;
    // printf("%d", RAND_MAX);
     
    return 0;
}