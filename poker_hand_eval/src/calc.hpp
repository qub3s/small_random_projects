#include <omp.h>
#include <vector>
#include <stdlib.h>
#include <fstream>

#include "board.hpp"
#include "final_hand.hpp"

// calc_postflop values (for all remaining hands)
// calc_turn values (for all remaining hands)




// number of element in the arry
bool check_distinct(int lc, int* c){
    for( int k = 0; k < lc-1; k++){
        for( int l = k+1; l < lc; l++){
            if( c[k] == c[l])
                return false;
        }
    }
    return true;
}

// array distinct from another x
bool check_distinct_from(int lc, int* c, int x){
    for( int i = 0; i < lc; i++){
        if( c[i] == x ){
            return false;
        }
    }

    return true;
}

/*
calculates the tesseract of card values (simplified, only for s subset of values, because the entire thing takes too long and the error is > 1%)
p1_c1 and p1_c2 need to be in range 0-25 (0-12 suite a) (13-25W suite a) 
to train the entire thing just edit the boundaries of b1,b2 to 52
*/
void preflop_matrix(int p1_c1, int p1_c2, double (*win)[13][13][13], double (*loss)[13][13][13]){
    int sp1_c1 = p1_c1 % 13;
    int sp1_c2 = p1_c2 % 13;

    p1_c1 += 26;
    p1_c2 += 26;

    // check if variables are in the correct range
    if( p1_c1 < 26 || p1_c1 >= 52 || p1_c2 < 26 || p1_c2 >= 52 ){
        throw std::logic_error( "Card Values have to be in range 0-26" );
    }

    int abs[13][13] = {0};        
    
    #pragma omp parallel for schedule(dynamic)
    for( int b1 = 0; b1 < 13; b1++){
        for( int b2 = b1; b2 < 26; b2++){
            int hc[4] = { b1, b2, p1_c1, p1_c2 };
            if( check_distinct( 4, hc ) ){

                for(int k1 = 0; k1 < 52; k1++ ){
                    if( check_distinct_from( 4, hc, k1) ){

                        for(int k2 = k1 + 1; k2 < 52; k2++ ){
                            if( check_distinct_from( 4, hc, k2) ){
                        
                                for(int k3 = k2 + 1; k3 < 52; k3++ ){
                                    if( check_distinct_from( 4, hc, k3) ){

                                        for(int k4 = k3 + 1; k4 < 52; k4++ ){
                                            if( check_distinct_from( 4, hc, k4) ){

                                                for(int k5 = k4 + 1; k5 < 52; k5++ ){
                                                    if( check_distinct_from( 4, hc, k5) ){

                                                        int x = b1 % 13;
                                                        int y = b2 % 13;

                                                        int board[5] = { k1, k2, k3, k4, k5 };

                                                        int res = eval_hand(p1_c1, p1_c2, b1, b2, board);

                                                        if( b1 / 13 == b2 / 13 ){
                                                            // suited
                                                            if(res == 1){
                                                                #pragma omp atomic
                                                                win[sp1_c1][sp1_c2][y][x]++;
                                                            }
                                                            
                                                            if(res == -1){
                                                                #pragma omp atomic
                                                                loss[sp1_c1][sp1_c2][y][x]++;
                                                            }
                                                            abs[y][x] += 1;
                                                        }
                                                        else{
                                                            // unsuited
                                                            if(res == 1){
                                                                #pragma omp atomic
                                                                win[sp1_c1][sp1_c2][x][y]++;
                                                            }
                                                            
                                                            if(res == -1){
                                                                #pragma omp atomic
                                                                loss[sp1_c1][sp1_c2][x][y]++;
                                                            }

                                                            abs[x][y] += 1;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
    }

    // calculate percentages
    for(int i = 0; i < 13; i++){
        for(int j = 0; j < 13; j++){
            loss[sp1_c1][sp1_c2][i][j] = loss[sp1_c1][sp1_c2][i][j] / (double)abs[i][j];
            win[sp1_c1][sp1_c2][i][j] = win[sp1_c1][sp1_c2][i][j] / (double)abs[i][j];
        }
    }
}

// writes the files to memory
void write_tesseract( double (*tess)[13][13][13], std::string path ){
    std::ofstream file;
    file.open(path);
    
    for( int i = 0; i < 13; i++){
        for( int j = 0; j < 13; j++){
            for( int k = 0; k < 13; k++){
                for( int l = 0; l < 13; l++){
                    file << tess[i][j][k][l] << "\n";
                }
            }
        }
    }
    return;
}

// reads the files in memory returns arr[13][13][13][13]
void read_tesseract( double (*tess)[13][13][13], std::string path ){
    std::string line;
    std::ifstream file(path);

    // Check if File exists
    if (!file.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'\n";
        std::exit(EXIT_FAILURE);
    }
    
    for( int i = 0; i < 13; i++){
        for( int j = 0; j < 13; j++){
            for( int k = 0; k < 13; k++){
                for( int l = 0; l < 13; l++){
                    if( !getline(file, line) ){
                        std::cerr << "File is not long enough for tess[13][13][13][13]";
                        std::exit(EXIT_FAILURE);
                    }
                    tess[i][j][k][l] = std::stod(line); 
                }
            }
        }
    }

    file.close();
    return;
}

/*
Usage:          calculates the preflop winrate and loserate files
Format:         first 2 Vars are your cards; second 2 cards are the cards of the other players
Structure:      lower half -> offsuite; upper half -> suited:
P S S S S S
O P S S S S 
O O P S S S
O O O P S S
O O O O P S 
O O O O O P     
*/
void calc_tesseract(){
    double win[13][13][13][13] = {0};
    double loss[13][13][13][13] = {0};

    #pragma omp parallel for num_threads(169) schedule(dynamic)
    for(int i = 0; i < 169; i++){
        int n = i / 13;
        int m = i % 13;

        // (n,m) would fill the upper half of the square
        if( n >= m ){
            // account for suited and unsuited
            if( n != m){
                preflop_matrix(n, m, win, loss);
                preflop_matrix(m, n + 13, win, loss);
            }
            else{
                preflop_matrix(n, m + 13, win, loss);
            }
        }
    }

    write_tesseract(win, std::string("data/win.d") );
    write_tesseract(loss, std::string("data/loss.d") );
}
