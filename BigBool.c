#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BigBool.h"

BigBool* BigBool_from_str(char* vecStr) {
    if(!vecStr) {
        printf("empty string passed\n");
        return NULL;
    }

    BigBool* BB = calloc(1, sizeof(BigBool));
    if(!BB) {
        printf("memory allocation error\n");
        return NULL;
    }

    int vecLen = strlen(vecStr);
    for(int i = 0; i < vecLen; ++i) {
        if(vecStr[i] != '0' && vecStr[i] != '1') {
            printf("invalid argument passed\n");
            return NULL;
        }
    }

    BB->num_part = vecLen / 8;
    BB->num_bit = vecLen % 8;

    int index = 0;
    for(int part = 0; part < BB->num_part; ++part) {
        for(int bit = 0; bit < 8; ++bit) {
            index = vecLen - 1 - (part * 8 + bit);
            BB->vector[part] |= ((vecStr[index] - '0') << bit);
        }
    }

    for(int bit = 0; bit < BB->num_bit; ++bit) {
        index = vecLen - 1 - (BB->num_part * 8 + bit);
        BB->vector[BB->num_part] |= ((vecStr[index] - '0') << bit);
    }

    leading_zeros_remove(BB);

    return BB;
}

BigBool* BigBool_from_uint64(uint64_t num) {
    
    BigBool *BB = calloc(1, sizeof(BigBool));
    if(!num) {
        BB->num_part = 0;
        BB->num_bit = 1;
        return BB;
    }

    for(int part = 0; part < 8; ++part) {
        BB->vector[part] |= (num >> 8 * part);
    }
    
    leading_zeros_remove(BB);
    
    return BB;
}

char* BigBool_to_str(BigBool* BB) {

    if(!BB) {
        printf("null argument passed\n");
        return NULL;
    }

    size_t vecLen = BigBool_len(BB);
    char *vecStr = (char*)malloc(vecLen + 1);
    
    if(!vecStr) {
        printf("memory allocation error\n");
        return NULL;
    }

    for(int bit = vecLen - 1; bit >= 0; --bit) {
        vecStr[vecLen - bit - 1] = BB->vector[bit / 8] >> (bit % 8) & 1;
        vecStr[vecLen - bit - 1] += '0';
    }
    vecStr[vecLen] = '\0';

    return vecStr;
}

void inversion(BigBool* BB) {
    
    if(!BB) {
        printf("null argument passed\n");
        return;
    }


    uint8_t mask = 0b11111111;
    for(int part = 0; part < BB->num_part; ++part) {
       BB->vector[part] ^= mask;
    }

    for(int bit = 0; bit < BB->num_bit; ++bit) {
        BB->vector[BB->num_part] ^= 1 << bit;
    }

    return;
}

BigBool* conjunction(BigBool* BB1, BigBool* BB2) {
    if(!BB1 || !BB2) {
        printf("null argument passed\n");
        return NULL;        
    }

    BigBool* res = calloc(1, sizeof(BigBool));
    if(!res) {
        printf("memory allocation error\n");
        return NULL;        
    }

    if(BigBool_len(BB1) < BigBool_len(BB2)) {
        res = conjunction(BB2, BB1);
        return res;
    }

    int num_part = BB1->num_part + (BB1->num_bit > 0);
    for(int part = 0; part < num_part; ++part) {
        res->vector[part] = BB1->vector[part] & BB2->vector[part];
    }    

    res->num_part = BB1->num_part;
    res->num_bit = BB1->num_bit;
    leading_zeros_remove(res);
    return res;
}

BigBool* disjunction(BigBool* BB1, BigBool* BB2) {
    
    if(!BB1 || !BB2) {
        printf("null argument passed\n");
        return NULL;        
    }

    BigBool* res = calloc(1, sizeof(BigBool));
    if(!res) {
        printf("memory allocation error\n");
        return NULL;        
    }    


    if(BigBool_len(BB1) < BigBool_len(BB2)) {
        res = disjunction(BB2, BB1);
        return res;
    }
    

    int num_part = BB1->num_part + (BB1->num_bit > 0);
    for(int part = 0; part < num_part; ++part) {
        res->vector[part] = BB1->vector[part] | BB2->vector[part];
    }    

    res->num_part = BB1->num_part;
    res->num_bit = BB1->num_bit;

    return res;
}

BigBool* xor(BigBool* BB1, BigBool* BB2) {
    
    if(!BB1 || !BB2) {
        printf("null argument passed\n");
        return NULL;        
    }

    BigBool* res = calloc(1, sizeof(BigBool));
    if(!res) {
        printf("memory allocation error\n");
        return NULL;        
    }

    if(BigBool_len(BB1) < BigBool_len(BB2)) {
        res = xor(BB2, BB1);
        return res;
    }
    int parts = BB1->num_part + (BB1->num_bit > 0);
    for(int part = 0; part < parts; ++part) {
        res->vector[part] = BB1->vector[part] ^ BB2->vector[part];
    }    

    res->num_part = BB1->num_part;
    res->num_bit = BB1->num_bit;
    
    return res;
}

void left_shift(BigBool* BB, int shift) {
    
    if(!BB) {
        printf("null argument passed\n");
        return;
    }
    
    int parts = BB->num_part + (BB->num_bit > 0);
    uint8_t* res = calloc(1, parts * sizeof(uint8_t));
    if(!res) {
        printf("memory allocation error\n");
        return;        
    }

    if(shift >= MAX_PARTS * 8) {
        return;
    }

    if(shift < 0) {
        right_shift(BB, -shift);
        return;
    }

    int part = BB->num_part + (BB->num_bit > 0);
    for(part; part > shift / 8; --part) {
        res[part] = BB->vector[part - shift / 8];
        res[part] <<= shift % 8;
        res[part] |= BB->vector[part - shift / 8 - 1] >> (8 - shift % 8);
    }

    res[shift / 8] = BB->vector[0];
    res[shift / 8] <<= shift % 8;
    res[shift / 8] >>= shift % 8;
    res[shift / 8] <<= shift % 8;

    for(int part = 0; part < shift / 8; ++part) {
        res[part] = 0;
    }

    memcpy(BB->vector, res, parts);
    free(res);
}

void right_shift(BigBool* BB, int shift) {
    
    if(!BB) {
        printf("null argument passed\n");
        return;
    }

    int parts = BB->num_part + (BB->num_bit > 0);
    uint8_t* res = calloc(1, parts * sizeof(uint8_t));
    if(!res) {
        printf("memory allocation error\n");
        return;        
    }

    if(shift >= MAX_PARTS * 8) {
        return;
    }

    if(shift < 0) {
        left_shift(BB, -shift);
    }

    int lastPart = BB->num_part + (BB->num_bit > 0) - shift / 8 - 1;
    for(int part = 0; part < lastPart; ++part) {
        res[part] = BB->vector[part + shift / 8];
        res[part] >>= shift % 8;
        res[part] |= BB->vector[part + shift / 8 + 1] << (8 - shift % 8);        
    }
    res[lastPart] = BB->vector[lastPart + shift / 8];
    res[lastPart] >>= shift % 8;    

    
    memcpy(BB->vector, res, parts);
    free(res);
}

void left_circular_shift(BigBool* BB, int shift) {
    if(!BB) {
        printf("null argument passed\n");
        return;
    }
    if(shift < 0) {
        right_circular_shift(BB, -shift);
        return;
    }

    int parts = BB->num_part + (BB->num_bit > 0);
    uint8_t* res = calloc(1, parts * sizeof(uint8_t));
    if(!res) {
        printf("memory allocation error\n");
    }

    int vecLen = BigBool_len(BB); 
    shift %= vecLen;
    int shiftPos;

    for(int pos = 0; pos < vecLen; ++pos) {
        shiftPos = (pos + shift) % vecLen;
        res[shiftPos / 8] |= get_bit(BB, pos) << shiftPos % 8; 
    }

    memcpy(BB->vector, res, parts);
    free(res);

    // for(int pos = vecLen - 1; pos > 0; --pos) {
    //     bit1 = get_bit(BB, pos);
    //     shiftPos = (vecLen - shift + pos) % vecLen;
    //     bit2 = get_bit(BB, shiftPos);
    //     set_bit(BB, bit1, shiftPos);
    //     set_bit(BB, bit2, pos);
    // }

    return;
}

void right_circular_shift(BigBool* BB, int shift) {
    if(!BB) {
        printf("null argument passed\n");
        return;
    }
    if(shift < 0) {
        left_circular_shift(BB, -shift);
        return;
    }
    
    int parts = BB->num_part + (BB->num_bit > 0);
    uint8_t* res = calloc(1, parts * sizeof(uint8_t));
    if(!res) {
        printf("memory allocation error\n");
    }

    int vecLen = BigBool_len(BB); 
    shift %= vecLen;
    int shiftPos;

    for(int pos = vecLen - 1; pos >= 0; --pos) {
        shiftPos = (vecLen - shift + pos) % vecLen;
        res[shiftPos / 8] |= get_bit(BB, pos) << shiftPos % 8;
    }

    memcpy(BB->vector, res, parts);
    free(res);

    return;
}

size_t BigBool_len(BigBool* BB) {
    return BB->num_part * 8 + BB->num_bit;
}

void leading_zeros_remove(BigBool* BB) {
    
    if(!BB) {
        printf("null argument passed\n");
        return;
    }

    int parts = sizeof(BB->vector);
    while(parts > 0 && BB->vector[parts - 1] == 0 ) {
        --parts;
    }

    int bits = 0;
    while((1 << bits) <= BB->vector[parts - 1]) {
        ++bits;
    }

    if(!parts && !bits) {
        BB->num_part = 0;
        BB->num_bit = 1;
        return;
    }
    
    parts -= (bits % 8 > 0);
    bits %= 8;

    BB->num_part = parts;
    BB->num_bit = bits;

    return;
}

void set_bit(BigBool* BB, int bit, int pos) {
    if(!BB) {
        printf("null argument passed\n");
        return;
    }
    if(pos > BigBool_len(BB)) {
        printf("invalid argument passed\n");
        return;
    }
    if(bit != 0 && bit != 1) {
        printf("invalid argument passed\n");
        return;        
    }

    uint8_t mask = 0b11111111;
    mask ^= 1 << (pos % 8);
    BB->vector[pos / 8] &= mask;
    BB->vector[pos / 8] |= bit << (pos % 8);
    return;
}

int get_bit(BigBool* BB, int pos) {
    if(!BB) {
        printf("null argument passed\n");
        return -1;
    }
    if(pos > BigBool_len(BB)) {
        printf("invalid argument passed\n");
        return -1;
    }
    return BB->vector[pos / 8] >> (pos % 8) & 1;
}