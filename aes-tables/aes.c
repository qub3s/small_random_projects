#include <stdio.h> // Muss noch entfernt werden
#include "tables.c"


typedef unsigned int uint32_t;

int nr;
int nk;

uint32_t rotWord(uint32_t word){
    return (word << 8) ^ (word >> 24);
}

uint32_t subWord(uint32_t word){
    return (s_box[word >> 24] << 24) ^ (s_box[(word >> 16) % 0x100] << 16) ^ (s_box[(word >> 8) % 0x100] << 8) ^ (s_box[word % 0x100]);
}

void keyexpansion( uint32_t* enckey, uint32_t* deckey , uint32_t* bkey){
    if(enckey == NULL && deckey == NULL){
        return;
    }

    if(enckey == NULL){
        uint32_t tempenckey[(nr+1)*4];
        enckey = tempenckey;
    }

    int nr = nk +6;
    int i = 0;
    uint32_t t;

    for(;i < nk; i++){
        enckey[i] = bkey[i];
    }

    i = nk;

    while(i < 4*(nr+1)){
        t = enckey[i-1];
        
        if(i % nk == 0){
            t = subWord(rotWord(t)) ^ rcon[(i/nk)-1];
        }
        
        else if(nk > 6 && i%nk == 4){
            t = subWord(t);
        }
        int a = i - nk;
        enckey[i] = enckey[i-nk] ^ t;
        i = i + 1;
    }

    if(deckey == NULL){
        return;
    }

	for(int i = 0; i < (nr+1)*4; i += 4 ){
		int ei = (nr+1)*4 - i - 4;
		for(int j = 0; j < 4; j++ ){
			uint32_t t = enckey[ei+j];
			if( i > 0 && i+4 < (nr+1)*4 ){
				t = d0[s_box[ t >> 24]] ^ d1[s_box[ t>> 16 & 0xff]] ^ d2[s_box[ t >> 8 & 0xff]] ^ d3[s_box[ t & 0xff]];
            }
			deckey[i+j] = t;
		}
	}
}


void encrypt(uint32_t* state, uint32_t* key){
    uint32_t t0,t1,t2,t3;
    state[0] = state[0] ^ key[0];
    state[1] = state[1] ^ key[1];
    state[2] = state[2] ^ key[2];
    state[3] = state[3] ^ key[3];

    for(int i = 1; i < nr; i++){
        t0 = e0[ state[0] >>24 ] ^ e1[ state[1] >> 16 & 0xff ] ^ e2[ state[2] >> 8 & 0xff ] ^ e3[ state[3] & 0xff ];
        t1 = e0[ state[1] >>24 ] ^ e1[ state[2] >> 16 & 0xff ] ^ e2[ state[3] >> 8 & 0xff ] ^ e3[ state[0] & 0xff ];
        t2 = e0[ state[2] >>24 ] ^ e1[ state[3] >> 16 & 0xff ] ^ e2[ state[0] >> 8 & 0xff ] ^ e3[ state[1] & 0xff ];
        t3 = e0[ state[3] >>24 ] ^ e1[ state[0] >> 16 & 0xff ] ^ e2[ state[1] >> 8 & 0xff ] ^ e3[ state[2] & 0xff ];
        state[0] = t0 ^ key[i*4];
        state[1] = t1 ^ key[i*4+1];
        state[2] = t2 ^ key[i*4+2];
        state[3] = t3 ^ key[i*4+3];
    }

    t0 = (s_box[state[0] >> 24] << 24) ^ (s_box[state[1] >> 16 & 0xff] << 16) ^ (s_box[state[2] >> 8 & 0xff] << 8) ^ s_box[state[3] & 0xff];
    t1 = (s_box[state[1] >> 24] << 24) ^ (s_box[state[2] >> 16 & 0xff] << 16) ^ (s_box[state[3] >> 8 & 0xff] << 8) ^ s_box[state[0] & 0xff];
    t2 = (s_box[state[2] >> 24] << 24) ^ (s_box[state[3] >> 16 & 0xff] << 16) ^ (s_box[state[0] >> 8 & 0xff] << 8) ^ s_box[state[1] & 0xff];
    t3 = (s_box[state[3] >> 24] << 24) ^ (s_box[state[0] >> 16 & 0xff] << 16) ^ (s_box[state[1] >> 8 & 0xff] << 8) ^ s_box[state[2] & 0xff];
    
    state[0] = t0 ^ key[nr*4];
    state[1] = t1 ^ key[nr*4+1];
    state[2] = t2 ^ key[nr*4+2];
    state[3] = t3 ^ key[nr*4+3];
    
    return;
}

//https://cs.opensource.google/go/go/+/master:src/crypto/aes/block.go;l=146?q=expansion&sq=&ss=go%2Fgo:src%2Fcrypto%2Faes%2F
void decrypt(uint32_t* state, uint32_t* key){
    uint32_t t0,t1,t2,t3;
    state[0] = state[0] ^ key[0];
    state[1] = state[1] ^ key[1];
    state[2] = state[2] ^ key[2];
    state[3] = state[3] ^ key[3];

    for(int i = 1; i < nr; i++){
        t0 = d0[ state[0] >>24 ] ^ d1[ state[3] >> 16 & 0xff ] ^ d2[ state[2] >> 8 & 0xff ] ^ d3[ state[1] & 0xff ];
        t1 = d0[ state[1] >>24 ] ^ d1[ state[0] >> 16 & 0xff ] ^ d2[ state[3] >> 8 & 0xff ] ^ d3[ state[2] & 0xff ];
        t2 = d0[ state[2] >>24 ] ^ d1[ state[1] >> 16 & 0xff ] ^ d2[ state[0] >> 8 & 0xff ] ^ d3[ state[3] & 0xff ];
        t3 = d0[ state[3] >>24 ] ^ d1[ state[2] >> 16 & 0xff ] ^ d2[ state[1] >> 8 & 0xff ] ^ d3[ state[0] & 0xff ];
        state[0] = t0 ^ key[i*4];
        state[1] = t1 ^ key[i*4+1];
        state[2] = t2 ^ key[i*4+2];
        state[3] = t3 ^ key[i*4+3];
    }

    t0 = (i_box[state[0] >> 24] << 24) ^ (i_box[state[3] >> 16 & 0xff] << 16) ^ (i_box[state[2] >> 8 & 0xff] << 8) ^ i_box[state[1] & 0xff];
    t1 = (i_box[state[1] >> 24] << 24) ^ (i_box[state[0] >> 16 & 0xff] << 16) ^ (i_box[state[3] >> 8 & 0xff] << 8) ^ i_box[state[2] & 0xff];
    t2 = (i_box[state[2] >> 24] << 24) ^ (i_box[state[1] >> 16 & 0xff] << 16) ^ (i_box[state[0] >> 8 & 0xff] << 8) ^ i_box[state[3] & 0xff];
    t3 = (i_box[state[3] >> 24] << 24) ^ (i_box[state[2] >> 16 & 0xff] << 16) ^ (i_box[state[1] >> 8 & 0xff] << 8) ^ i_box[state[0] & 0xff];
    
    state[0] = t0 ^ key[nr*4];
    state[1] = t1 ^ key[nr*4+1];
    state[2] = t2 ^ key[nr*4+2];
    state[3] = t3 ^ key[nr*4+3];
    
    return;
}

void printstate( uint32_t* state ){
    for( int x = 0; x < 4; x++){
        printf("%x\n",state[x] );
    }
    printf("\n");
    return;
}

int setmode(int s){
    if(s == 128){
        nk = 4;
        nr = 10;
        return 1;
    }
    if(s == 192){
        nk = 6;
        nr = 12;
        return 1;
    }
    if(s == 256){
        nk = 8;
        nr = 14;
        return 1;
    }
    return 0;
}

/*
            nk  nb  nr
AES-128     4   4   10
AES-192     6   4   12
AES-256     8   4   14
*/

void main(){
    uint32_t basekey[] = { 0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c };  
    uint32_t state[] = { 0x3243f6a8, 0x885a308d, 0x313198a2, 0xe0370734};
    uint32_t out[] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000};      
    setmode(128);
    
    uint32_t enckey[(nr+1)*4];
    uint32_t deckey[(nr+1)*4];
    keyexpansion(enckey, deckey, basekey);
    
    printstate(state);

    encrypt(state,enckey);

    printstate(state);

    decrypt(state,deckey);

    printstate(state);
}


