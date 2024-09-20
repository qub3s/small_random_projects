#include "board.hpp"
#include "final_hand.hpp"

#include <omp.h>
#include <stdlib.h>
#include <chrono>
#include <random>
#include <iostream>


int* get_rand_distinct_2(int x, int* a, std::mt19937_64 rnd){
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

void speed_test_hand_calc(int n){
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel 
    {
        std::mt19937_64 mt(time(NULL));
        #pragma omp for schedule(dynamic) 
        for( int k = 0; k < n; k++){
            int c[7] = { 0 };
            get_rand_distinct_2(7,c,mt); 
            board bo(c);
        }
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = end - start;

    std::cout << "Ticks per second: " << (int)(n/time.count()) << "\n"; 
}

bool test_hand_eval(){
    int fhv[9] = {0};

    #pragma omp parallel for schedule(dynamic)
    for( int k1 = 0; k1 < 52; k1++){
        for( int k2 = k1+1; k2 < 52; k2++){
            for( int k3 = k2+1; k3 < 52; k3++){
                for( int k4 = k3+1; k4 < 52; k4++){
                    for( int k5 = k4+1; k5 < 52; k5++){
                        for( int k6 = k5+1; k6 < 52; k6++){
                            for( int k7 = k6+1; k7 < 52; k7++){
                                if( k1 < 52 && k2 < 52 && k3 < 52 && k4 < 52 && k5 < 52 && k6 < 52 && k7 < 52){
                                    int tmp[7] = {k1, k2, k3, k4, k5, k6, k7 };
                                    board bo( tmp );
                                    final_hand fh = bo.get_final_hand();
                                    
                                    #pragma omp atomic 
                                    fhv[fh.strength] += 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    
    if( fhv[0] == 23294460 && fhv[1] == 58627800 && fhv[2] == 31433400 && fhv[3] == 6461620 && fhv[4] == 6180020 && fhv[5] == 4047644 && fhv[6] == 3473184 && fhv[7] == 224848 && fhv[8] == 41584 ){
        return true;
    }
    else{
        return false;
    }
}

void run_tests(){
    std::cout << "Start Testing!\n";
    speed_test_hand_calc(100000000);
    std::cout << "Testing Finished!\n";
    //test_hand_eval();
    //run_board_tests();
}
