#include "board.hpp"
#include "final_hand.hpp"
#include <random>
#include <iostream>

// Constructur
// Does not check logic of the boards (cards occuring multple times ...)
board::board(int* vs){
    for(int k = 0; k < 7; k++){
        if( vs[k] >= 0 && vs[k] <= 52){
            c[k] = vs[k];
        }
        else{
            throw std::logic_error( "This value doesn't represent a card!!!" );
        }
    }
    sort();

    // get the valuecounts
    for(int k = 0; k < 7; k++){
        count[ c[k]%13 ] += 1;
    }
}

// 0 = draw, 1 = a won, -1 = b won
int eval_hand(int a1, int a2, int b1, int b2, int* c){
    int arr_a[7] = { a1, a2, c[0], c[1], c[2], c[3], c[4] };
    int arr_b[7] = { b1, b2, c[0], c[1], c[2], c[3], c[4] };
    
    board a(arr_a);
    board b(arr_b);

    final_hand fa = a.get_final_hand();
    final_hand fb = b.get_final_hand();

    if( fa > fb){
        return 1;
    }

    if( fb > fa){
        return -1;
    }

    return 0;
}

// x: number of values, a: array of that size
int* get_rand_distinct(int x, int* a, std::mt19937_64 rnd){
    a[0] = rnd()%52;
    for(int k = 1; k < x; k++){
        a[k] = rnd()%52;
        for(int l = 0; l < k; l++){
            if( a[l] == a[k] ){
                k -= 1;
                break;
            }
        }
    }
    return a;
}

// prints representation of the board
void board::show(){
    std::cout << "Board: ";
    for( int k = 0; k < 7; k++){
        std::cout << (int) c[k] << " ";
    }
    std::cout << "\n";
}

// returns final_hand struct to be compared
final_hand board::get_final_hand(){
    bool f = check_flush();
    bool s = check_straight();

    if( !f && !s ){
        return check_counted_hands();
    }

    if( !f && s ){
        return fh_straight();
    }

    if( f && !s ){
        return fh_flush();
    }

    if( f && s ){
        return fh_straight_flush();
    }
    
    throw std::logic_error( "This cant happen !!!" );
}

// sorts the board by cardvalue
void board::sort(){
    for( int j, i = 1; i < 7; i++){       
        int tmp = c[i];         
        for( j = i-1; j >= 0 && c[j]%13 < tmp%13; j-- ){
            std::swap(c[j+1], c[j]);
        }
        c[j+1] = tmp;
    }
}

// Checks for High Card, Pair, 2-Pair, 3 of a Kind, Full House and 4 of a Kind
final_hand board::check_counted_hands(){
    int maxc;
    int maxc_2;

    // get maximum
    int v = -1;
    for(int k = 0; k < 13; k++){
        if( v <= count[k] ){
            maxc = k;
            v = count[k];
        }
    }

    v = -1;
    for(int k = 0; k < 13; k++){
        if( v <= count[k] && maxc != k){
            maxc_2 = k;
            v = count[k];
        }
    }

    if( count[maxc] == 1 ){
        return fh_high_card();
    }

    if( count[maxc] == 2 ){
        if( count[maxc_2] == 2){
            return fh_two_pair(maxc, maxc_2);
        }
        else{
            return fh_pair(maxc);
        }
    }

    if( count[maxc] == 3 ){
        if( count[maxc_2] == 2 || count[maxc_2] == 3){
            return fh_full_house(maxc, maxc_2);
        }
        else{
            return fh_three_of_a_kind(maxc);
        }
    }

    if( count[maxc] == 4){
            return fh_four_of_a_kind(maxc, maxc_2);    
    }
    else{
        std::cout << "\nCount: " << maxc << "  " << maxc_2 << "\n";
        throw std::logic_error( "There cant be 5 Card that are the same !!!" );
    }
}

// check for straight
bool board::check_straight(){
    int t = 0;
    for( int k = 0; k < 13; k++){
        if(count[k] > 0){
            t ++;
        }
        else{
            t = 0;
        }

        if( t == 5){
            return true;
        }
    }

    // check for lowest street
    if(count[12] >= 1 && count[0] >= 1 && count[1] >= 1 && count[2] >= 1 && count[3] >= 1){ return true; }
    
    return false;
}

// check for flush
bool board::check_flush(){
    int t[4] = { 0 };

    for(int k = 0; k < 7; k++){
        t[ c[k]/13 ] += 1;
    }

    int suite = 0;
    for(int k = 1; k < 4; k++){
        if( t[suite] < t[k]){
            suite = k;
        }
    }

    if( t[suite] >= 5 ){
        this->fsuite = suite; 
        return true;
    }
    return false;
}

// returns the hand "High Card"
final_hand board::fh_high_card(){
    // card values from 0-12
    int cv [5] = { 0 };
    int t = 0;

    // There can be no pairs so you are
    for(int k = 12; k >= 0 && t < 5; k--){
        if(count[k] != 0){
            cv[t] = k;
            t++;
        }
    }

    return final_hand(cv, 0);
}

// returns the hand "Pair"
final_hand board::fh_pair( int maxc ){
    // card values from 0-12
    int cv [5] = { maxc, maxc, 0, 0, 0 };
    int t = 2;

    // There can be no pairs so you are
    for(int k = 12; k >= 0 && t < 5; k--){
        if(count[k] != 0 && k != maxc){
            cv[t] = k;
            t++;
        }
    }

    return final_hand(cv, 1);
}

// returns the hand "Two Pair"
final_hand board::fh_two_pair(int maxc, int maxc_2){
    // card values from 0-12
    int cv [5] = { maxc, maxc, maxc_2, maxc_2, 0 };
    int t = 4;

    // There can be no pairs so you are
    for(int k = 12; k >= 0 && t < 5; k--){
        if(count[k] != 0 && k != maxc && k != maxc_2){
            cv[t] = k;
            t++;
        }
    }

    return final_hand(cv, 2);
}

// return hand "Three of a Kind"
final_hand board::fh_three_of_a_kind(int maxc){
    // card values from 0-12
    int cv [5] = { maxc, maxc, maxc, 0, 0 };
    int t = 3;

    for(int k = 12; k >= 0 && t < 5; k--){
        if(count[k] != 0 && k != maxc){
            cv[t] = k;
            t++;
        }
    }

    return final_hand(cv, 3);
}

// return hand "Straight"
final_hand board::fh_straight(){
    // aproach also works for ace high straights because -1 is simply the lowest staight 
    int t = 1;
    int lcard = -1;
        
    for( int k = 1; k < 7; k++){
        // Check if counter needs to be reset
        if( c[k-1]%13 != c[k]%13 && c[k-1]%13-1 != c[k]%13 ){
            t = 1;
        }
        else{
            t += 1;
            if(t == 5){
                lcard = c[k]%13;
            }
        }
    }

    int cv[5] = {lcard + 4, lcard + 3, lcard + 2, lcard + 1, lcard};
    return final_hand(cv, 4);
}

// returns hand "Flush"
final_hand board::fh_flush(){
    // Card value
    int cv[5] = { 0 };
    int t = 0;

    t = 0;
    for(int k = 0; k < 7; k++){
        if(fsuite * 13 <= c[k] && (fsuite+1) * 13 > c[k] && t < 5){
            cv[t] = c[k]%13;
            t++;
        }
    }

    return  final_hand(cv, 5);
}

// returns hand "Full House"
final_hand board::fh_full_house(int maxc, int maxc_2){
    int cv [5] = { maxc, maxc, maxc, maxc_2, maxc_2 };
    return final_hand(cv, 6);
}

// return hand "Four of a Kind"
final_hand board::fh_four_of_a_kind(int maxc, int maxc_2){
    // card values from 0-12
    int cv [5] = { maxc, maxc, maxc, maxc, maxc_2};
    return final_hand(cv, 7);
}

// check and returns "Straight Flush" or else "Flush"
final_hand board::fh_straight_flush(){
    int sc[7] = { 0 }; // suited cards
    int len_sc = 0;
    int t = 1;
    int lcard = -1;
    
    for(int k = 0; k < 7; k++){
        if( c[k] >= fsuite*13 && c[k] < (fsuite+1)*13 ){
            sc[len_sc] = c[k];
            len_sc++;
        }
    }

    for( int k = 1; k < len_sc; k++){
        if( sc[k-1] != sc[k] && sc[k-1]-1 != sc[k] ){
            t = 1;
        }
        else{
            t += 1;
            if(t == 5){
                lcard = sc[k]%13;
            }
        }
    }

    
    int cv[5] = {lcard + 4, lcard + 3, lcard + 2, lcard + 1, lcard};

    // checks for the lowest street
    if( lcard != -1 || ( sc[0]%13 == 12 && sc[len_sc-1]%13 == 0 && sc[len_sc-2]%13 == 1 && sc[len_sc-3]%13 == 2 && sc[len_sc-4]%13 == 3 )){
        return final_hand(cv,8);
    }
    else{
        return fh_flush();
    }
}
