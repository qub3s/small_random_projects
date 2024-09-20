#include <cstdlib>
#include <iostream>
#include "../src/board.hpp"
#include <omp.h>
#include <stdlib.h>

// check if there are dublicates in board and the hands
bool check_dublicate(int *t, int s){
    for( int k = 0; k < s; k++){
        for( int j = k; j < s; j++){
            if( t[k] == t[j] && k != j){
                return false;
            }
        }    
    }
    return true;
}

#define IS_TRUE(x) { if (!(x)) std::cout << __FUNCTION__ << " failed on line " << __LINE__ << std::endl; }

bool highcard_1(){
    int t[7] = {12, 11, 20, 18, 19, 3, 4};
    board bo( t );
    final_hand fh = bo.get_final_hand();

    return (fh.strength == 0 && fh.c[0] == 12 && fh.c[4] == 3);
}

bool speedtest(){    
    long x = 0;
    // ALL HANDS  9.089.480.400 = 91 *  99884400
    // ALL HANDS 16.880.463.600 = 169 * 99884400

    // now ->    16.880.463.600 / 133.784.560 = 126x
    // now ->    9.089.480.400 / 133.784.560 =  68x

    // 133.784.560 < 10sec -> 9.089.480.400 = 10 min

    #pragma omp parallel for schedule(dynamic)
    for(int k = 0; k < 52; k++){
        for(int j = k+1; j < 52; j++){
            for(int i = j+1; i < 52; i++){
                for(int h = i+1; h < 52; h++){
                    for(int g = h+1; g < 52; g++){
                        for(int f = g+1; f < 52; f++){
                            for(int e = f+1; e < 52; e++){

                                int t[7] = {k,j,i,h,g,f,e};
                                
                                if(check_dublicate(t,7)){                        
                                    board bo( t );
                                    final_hand fh = bo.get_final_hand();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    std::cout << x << "\n";
    return true;
}

void board_test()
{
    //IS_TRUE(highcard_1());
    IS_TRUE(speedtest());
}

// 1712304 * 168 = 287667072
