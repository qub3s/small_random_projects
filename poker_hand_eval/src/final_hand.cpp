#include "final_hand.hpp"

/*
Hand Rankings
- 8 -> Straight Flush
- 7 -> Four of a Kind
- 6 -> Full House
- 5 -> Flush
- 4 -> Straight
- 3 -> Three of a Kind
- 2 -> Two Pair
- 1 -> Pair
- 0 -> Highcard

Result consits of one Handrank var and the 5 card the hand is made up
Results must be comparable
*/


final_hand::final_hand(int* cv, int s){
    // check if board is possible ???
    strength = s;

    for(int k = 0; k < 5; k++){
        c[k] = cv[k];
    }
}

void final_hand::show(){
    std::cout << "Final Hand: ";
    for( int k = 0; k < 5; k++)
        std::cout << (int) c[k] << " ";
    
    std::cout << "\n";
}

