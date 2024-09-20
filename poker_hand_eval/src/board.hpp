# pragma once
#include "final_hand.hpp"
#include <random>

class board{
    public:
        int c[7];
        int count[13] = {0};
        int fsuite = -1;

        // constructor TODO -> implement check for double cards
        board(int* vs);

        // prints representation of the board
        void show();

        // returns the final hand of a board
        final_hand get_final_hand();

        // evalutates which hand is winning
        int eval_hand(int a1, int a2, int b1, int b2, int* c);

        // get x random and distinct numbers
        int* get_rand_distinct(int x, int* a, std::mt19937_64 rnd);

        // sorts the board by cardvalue
        void sort();

        // Checks for High Card, Pair, 2-Pair, 3 of a Kind, Full House and 4 of a Kind
        final_hand check_counted_hands();

        // check for straight
        bool check_straight();

        // check for flush
        bool check_flush();

        // returns High Card
        final_hand fh_high_card();

        // return Pairs
        final_hand fh_pair( int maxc );

        // return Two Pairs
        final_hand fh_two_pair(int maxc, int maxc_2);

        // return Three of a Kind
        final_hand fh_three_of_a_kind(int maxc);

        // return Straight
        final_hand fh_straight();

        // returns flush
        final_hand fh_flush();

        // return Full House
        final_hand fh_full_house(int maxc, int maxc_2);

        // return Four of a Kind
        final_hand fh_four_of_a_kind(int maxc, int maxc_2);

        // returns Royal Flush if exists, otherwise final_hand(-1, {0})
        final_hand fh_straight_flush();

        // returns Royal Flush if exists, otherwise final_hand(-1, {0})
        final_hand fh_royal_flush();
};
