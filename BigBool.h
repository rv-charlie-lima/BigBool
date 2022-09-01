#ifndef BIGBOOL_H
#define BIGBOOL_H

#include <stdio.h>
#include <stdint.h>

#define MAX_PARTS 20

typedef struct BigBool
{
    uint8_t vector[20];
    size_t num_part;
    size_t num_bit;
} BigBool;

BigBool* BigBool_from_str(char*);
BigBool* BigBool_from_uint64(uint64_t);
char* BigBool_to_str(BigBool*);

void inversion(BigBool*);
BigBool* conjunction(BigBool*, BigBool*);
BigBool* disjunction(BigBool*, BigBool*);
BigBool* xor(BigBool*, BigBool*);

void left_shift(BigBool*, int);
void right_shift(BigBool*, int);
void left_circular_shift(BigBool*, int);
void right_circular_shift(BigBool*, int);

size_t BigBool_len(BigBool*);
void leading_zeros_remove(BigBool*);
void set_bit(BigBool*, int, int);
int get_bit(BigBool*, int);

size_t high_bit_pos_cycle(uint64_t num);

#endif