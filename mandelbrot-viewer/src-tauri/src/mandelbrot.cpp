#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <fstream>
using namespace std::chrono;
using namespace std;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct Complex{
    long double re;
    long double im;

    inline Complex operator + ( Complex C ) {
        return { C.re + re, C.im + im };
    };

    inline Complex operator * ( Complex C ){
        return { ( C.re * re - C.im * im), (C.re * im + C.im * re)};
    };
};

// recursively calculates the value for 1 point
int reccalcpoint(int maxiterations, int iterations, Complex save, Complex num){
    if( iterations >= maxiterations ){
        return maxiterations;
    }

    if( ( num.re * num.re + num.im * num.im ) > 4 ){
        return iterations;
    }

    return reccalcpoint(maxiterations, iterations+1, save, ((num * num) + save) );
}

struct threadparam {                                                   
    int wp;
    int hp;
    long double distancex;
    long double distancey;
    Complex lupoint;
    int maxiterations;
    int* res;
};    

// wp,h are the number of points should be calculated
// distance is the distance between the left and the right border
// lupoint ist the most left and most upward point
void calcthread(void* vtp){    
    struct threadparam *tp = (threadparam*) vtp;
    
    int reslength = tp->wp;
    
    Complex point = { tp->lupoint.re, tp->lupoint.im };
    
    for( int h = 0; h < tp->hp; h++ ){
        for( int w = 0; w < tp->wp; w++ ){
            tp->res[reslength * h + w] = reccalcpoint( tp->maxiterations, 0, point, {0,0} );
            point.re += tp->distancex / tp->wp;
        }
        point.re = tp->lupoint.re;
        point.im -= tp->distancey / tp->hp;
    }
    pthread_exit(NULL);
}

// vorerst immer wp = hp
void calc(int wp, int hp, long double distance, Complex center, int maxiterations, int*res, int numthreads){
    if( numthreads > hp ){ numthreads = hp; }
    if( numthreads < 1 ){ numthreads = 1; }
    int numlines, rest;

    if( numthreads == 1 ){
        numlines = hp;
        rest = hp;
    }
    else{
        numlines = hp / (numthreads);
        rest = hp % numthreads + numlines;
    }

    threadparam params[numthreads];
    thread t[numthreads];
    
    for( int i = 0; i < numthreads-1; i++ ){
        params[i] = { wp, numlines, distance, distance/numthreads, { center.re - distance/2, center.im + distance / 2 - (distance / numthreads * i) }, maxiterations, res + i*wp*numlines };
        t[i] = thread(calcthread,&(params[i]));
    }

    params[numthreads-1] = { wp, rest, distance, distance/numthreads, { center.re - distance/2, center.im + distance / 2 - (distance / numthreads * (numthreads-1)) }, maxiterations, res + (numthreads-1)*wp*numlines };
    t[numthreads-1] = thread(calcthread, &(params[numthreads-1]));

    for( int i = 0; i < numthreads; i++ ){
        t[i].join();
    }

    return;
}

void writehex(ofstream* o, char* p, int l){
     for(; l > 0; l--){
        *o << p[l-1];
     }
     return;
}

// g++ mandelbrot.cpp -o mandelbrot && ./mandelbrot -1.25 -0.37777 5000 0.1 16 33 wallpaper
// x, y, pixel in one dimension, breite, number of threads, search depth, name
int main(int argc, char** argv){
    Complex lupoint = {atof(argv[1]), atof(argv[2])*-1};
    int n = atoi(argv[3]);
    long double width = atof(argv[4]);
    int numthreads = atoi(argv[5]);
    int depth = atoi(argv[6]);
    
    // Memory allocation
    size_t a = (n * n) * sizeof(int);
    int* res = (int*) malloc(a);

    auto start = high_resolution_clock::now();
    calc(n, n, width, lupoint, depth, res, numthreads);
    
    int test = 70;
    cout << hex << (int) ((char*) &test)[0] << "\n";

    // --------------------------------------------------------------------------------------------------------------------------
    // write file
    ofstream binfile(argv[7], ios::out | ios::binary);
    
    uint32_t tmp;
    // write header
    
    //cout << tmp << "\n"; 
    //cout << (int) ((char*) &tmp)[0] << "\n";

    int padding = (n*3)%4;

    
    binfile << "BM";
    binfile.write( (char*) &( tmp = (uint32_t) 54+(padding*n) + (n*n*3) ), sizeof(uint32_t) );                 // !    Size of file
    binfile.write( (char*) &( tmp = (uint32_t) 0),  sizeof(uint32_t) );                 //      Unused
    binfile.write( (char*) &( tmp = (uint32_t) 54), sizeof(uint32_t) );                 //      Start of Pixel data
    binfile.write( (char*) &( tmp = (uint32_t) 40), sizeof(uint32_t) );                 //      Number of bytes in the header
    binfile.write( (char*) &( tmp = (uint32_t) n),  sizeof(uint32_t) );                 // !    width
    binfile.write( (char*) &( tmp = (uint32_t) n),  sizeof(uint32_t) );                 // !    height
    binfile.write( (char*) &( tmp = (uint32_t) 0x00180001), sizeof(uint32_t) );         //      Colorplanes and bits per pixel
    binfile.write( (char*) &( tmp = (uint32_t) 0),  sizeof(uint32_t) );                 //      Compression
    binfile.write( (char*) &( tmp = (uint32_t) (padding*n) + (n*n*3) ), sizeof(uint32_t) );                 // !    Size of Bitmap data
    binfile.write( (char*) &( tmp = (uint32_t) 0x00000b13), sizeof(uint32_t) );         //      resolution
    binfile.write( (char*) &( tmp = (uint32_t) 0x00000b13), sizeof(uint32_t) );         //      resolution
    binfile.write( (char*) &( tmp = (uint32_t) 0),  sizeof(uint32_t) );                 //      colors 
    binfile.write( (char*) &( tmp = (uint32_t) 0),  sizeof(uint32_t) );                 //      important colors
    
    for( int n1 = 0; n1 < n; n1++ ){
        for( int n2 = 0; n2 < n; n2++ ){
            if( res[n1*n+n2] < depth/10){
                binfile << (uint8_t) 0 << (uint8_t) 22  << (uint8_t) 22;
            }
            else if( res[n1*n+n2] < depth/10*2){
                binfile << (uint8_t) 0 << (uint8_t) 44  << (uint8_t) 44;
            }
            else if( res[n1*n+n2] < depth/10*3){
                binfile << (uint8_t) 0 << (uint8_t) 66  << (uint8_t) 66;
            }
            else if( res[n1*n+n2] < depth/10*4){
                binfile << (uint8_t) 0 << (uint8_t) 88  << (uint8_t) 88;
            }
            else if( res[n1*n+n2] < depth/10*5){
                binfile << (uint8_t) 0 << (uint8_t) 110  << (uint8_t) 110;
            }
            else if( res[n1*n+n2] < depth/10*6){
                binfile << (uint8_t) 0 << (uint8_t) 132  << (uint8_t) 132;
            }
            else if( res[n1*n+n2] < depth/10*7){
                binfile << (uint8_t) 0 << (uint8_t) 154  << (uint8_t) 154;
            }
            else if( res[n1*n+n2] < depth/10*8){
                binfile << (uint8_t) 0 << (uint8_t) 176  << (uint8_t) 176;
            }
            else if( res[n1*n+n2] < depth/10*9){
                binfile << (uint8_t) 0 << (uint8_t) 198  << (uint8_t) 198;
            }
            else if( res[n1*n+n2] < depth){
                binfile << (uint8_t) 0 << (uint8_t) 220  << (uint8_t) 220;
            }
            else{
                binfile << (uint8_t) 55 << (uint8_t) 55 << (uint8_t) 55;
            }
        }
        for( int n2 = 0; n2 < padding; n2++){
            binfile << (uint8_t) 0x00;
        }
    }
    auto stop = high_resolution_clock::now();   
    cout << ((double)duration_cast<microseconds>(stop - start).count())/1000000 << "s \n";
}
