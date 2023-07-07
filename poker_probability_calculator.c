#include<stdio.h>
#include<time.h> //Both libraries for generating random integers 
#include<stdlib.h>
#include<assert.h>


const int Cards_in_hand = 5; // Cards in a (5 or 7)-poker hand
const long int n_card_draws = 10000000;



// Names and numbers of the suits and values in a card deck
char suits_array[][15] = {"Clubs", "Diamonds", "Hearts", "Spades"};
char values_array[][10] = {"Ace", "Two", "Three", "Four", "Five", "Six", "Seven", 
                    "Eight", "Nine", "Ten", "J", "Q", "K"};

// Defining 'card' objects; each is a card, which has a suit and value attribute indicating its value and suit
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

// Prints a hand of cards
void print_hand_of_cards_size(card hand_of_cards[], int size){
    for (int i = 0; i < size; i++){
        printf("Card %d: %s of %s\n", i+1,
        values_array[hand_of_cards[i].v], suits_array[hand_of_cards[i].s]);}
    printf("\n");
}

void print_hand_of_cards(card hand_of_cards[]){
    print_hand_of_cards_size(hand_of_cards, Cards_in_hand);
}

// Checks whether there is an specific element in a given array
int element_in_array(int array[], int element, int size){
    for (int i = 0; i < size; i++){
        if (array[i]==element){
            return 1;} 
    }
    return 0;
}

/* Creates a hand of cards taking out the cards indexed by the indices array
Ex: if the hand_of_cards == {Card1, Card2, Card3, Card4} and indices == {3, 1}, therefore
new_hand == {Card1, Card3}*/
void create_hand_of_cards_without_indices(card hand_of_cards[], card new_hand[], int indices[], 
                                          int size_old, int size_new, int size_indices){
    
    for (int i = 0, j = 0; i < size_old; i++){
        if (!element_in_array(indices, i, size_indices )){
            new_hand[j++] = hand_of_cards[i];}
    }
}

// Checking if a card is in a hand of cards and stores the index value using the pointer ptr_position
int there_is_card(card hand_of_cards[], card target_card, int* ptr_position, int size){
    for (int i = 0; i < size; i++){
        card current_card = hand_of_cards[i];
        if (current_card.s == target_card.s && current_card.v == target_card.v){
            *ptr_position = i;
            return 1;}
        }
    return 0;
}

// Checks whether there are repeated cards
int no_repeated_card(card hand_of_cards[]){
    for (int i = 0; i < Cards_in_hand; i++){
        for (int j = i+1; j< Cards_in_hand; j++){
            if (hand_of_cards[i].s == hand_of_cards[j].s && hand_of_cards[i].v == hand_of_cards[j].v){
                return 0;}
    }
}   
    return 1;
}

int there_is_card_without_size(card hand_of_cards[], card target_card, int* ptr_position){
    return there_is_card(hand_of_cards, target_card, ptr_position, Cards_in_hand);
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

// Each of the following returns a boolean indicating if the given card set has the given hand
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

    // int pair = 
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
        for (int value = 0; value < 10; value ++){ // when value == 9, value + 4 == 13 allows considering the hand 10, J, Q, K, Ace
        Card_1.v = value;
        Card_2.v = value + 1;
        Card_3.v = value + 2;
        Card_4.v = value + 3;
        Card_5.v = (value + 4) % 13; //we take mod since 13 % 13 == 0
        
        expression = ( there_is_card_without_size(hand_of_cards, Card_1, &_) && there_is_card_without_size(hand_of_cards, Card_2, &_) \
                   && there_is_card_without_size(hand_of_cards, Card_3, &_) && there_is_card_without_size(hand_of_cards, Card_4, &_) \
                   && there_is_card_without_size(hand_of_cards, Card_5, &_) );
        // printf("suit %s value %d\n", suits_array[suit], value);
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
        expression = ( there_is_card_without_size(hand_of_cards, Ace, &_) && there_is_card_without_size(hand_of_cards, King, &_) \
                   && there_is_card_without_size(hand_of_cards, Queen, &_) && there_is_card_without_size(hand_of_cards, Joker, &_) \
                   && there_is_card_without_size(hand_of_cards, Ten, &_) );
        if (expression){
            return 1;}
    }
    return 0;
    }

int there_is_Straight(card hand_of_cards[]){
    int value_counts[13] = {0}, product = 1;

    for (int i = 0; i < Cards_in_hand; i++){
        value_counts[hand_of_cards[i].v]++;
    }
    for (int j = 0; j < 10; j++){
        product = 1;
        for (int k = j; k < j + 5; k++){
            product *= value_counts[k % 13]; // when k == j == 9, k + 4 == 13 allows considering the hand 10, J, Q, K, Ace
            }
        if (product){
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

// Counts and compute each probability.
void compute_probs(){
    double no_pairs, pairs, two_pairs, three_ofs, straights, flushes, full_houses, four_ofs, royal_fls, straight_fls;
    no_pairs = pairs = two_pairs = three_ofs = straights = flushes = full_houses = four_ofs = straight_fls = royal_fls = 0;
    int positions[3];

    for (int draw = 0; draw < n_card_draws; draw++){
        positions[0] = positions[1] = positions[2] =-1;
        card drew_hand[Cards_in_hand];
        draw_cards(drew_hand);
        assert(no_repeated_card(drew_hand));

        if (there_is_RoyalFlush(drew_hand)){
            royal_fls++;
            continue;}
        if (there_is_StraightFlush(drew_hand)){
            straight_fls++;
            continue;}        
        if (there_is_FourOfAKind(drew_hand)){
            four_ofs++;
            continue;}
        if (there_is_FullHouse(drew_hand)){
            full_houses++;
            positions[0] = positions[1] = positions[2] =-1;
            continue;}
        if (there_is_Flush(drew_hand)){
            flushes++;
            continue;}
        if (there_is_Straight(drew_hand)){
            straights++;
            continue;}
        if (there_is_ThreeOfAKind(drew_hand, Cards_in_hand)){
            three_ofs++;
            continue;}
        if (there_is_TwoPair(drew_hand)){
            two_pairs++;
            continue;}
        if (there_is_Pair(drew_hand, positions, Cards_in_hand)){
            pairs++;
            continue;}
        no_pairs++;
    }

    printf("Royal flush ratio:        %.4f%% | %.0f\n", 100*royal_fls/n_card_draws, royal_fls);
    printf("Straight flush ratio:     %.4f%% | %.0f\n", 100*straight_fls/n_card_draws, straight_fls);
    printf("Four of a kind ratio:     %.4f%% | %.0f\n", 100*four_ofs/n_card_draws, four_ofs);
    printf("Full house ratio:         %.4f%% | %.0f\n", 100*full_houses/n_card_draws, full_houses);
    printf("Flush ratio:              %.4f%% | %.0f\n", 100*flushes/n_card_draws, flushes);
    printf("Straight ratio:           %.4f%% | %.0f\n", 100*straights/n_card_draws, straights);
    printf("Three of a kind ratio:    %.4f%% | %.0f\n", 100*three_ofs/n_card_draws, three_ofs);
    printf("Two pairs ratio:          %.4f%% | %.0f\n", 100*two_pairs/n_card_draws, two_pairs);
    printf("Pairs ratio:              %.4f%% | %.0f\n", 100*pairs/n_card_draws, pairs);
    printf("No pairs/high card ratio: %.4f%% | %.0f\n", 100*no_pairs/n_card_draws, no_pairs);

}

int main(void){
    printf("\nThis program computes the probabilities of each hand in a %d-poker game \n \
by using Monte-Carlo's method using %ld randomly drawn hands \n \
Note: Set the parameters by modifying lines 7,8 and the random seed of this code.\n \
-------------------------------------------------------------------------------\n\n", Cards_in_hand, n_card_draws);
    srand(12345); // Random seed
    assert((Cards_in_hand == 5 || Cards_in_hand == 7));
    compute_probs();
    return 0;
}