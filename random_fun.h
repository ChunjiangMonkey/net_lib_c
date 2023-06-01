#ifndef _RANDOM_FUN_H_
#define _RANDOM_FUN_H_
#define NN 624
#define MM 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define NAMEOUT "K4b075r5Q2"
#define TEMPERING_SHIFT_U(y) (y >> 11)
#define TEMPERING_SHIFT_S(y) (y << 7)
#define TEMPERING_SHIFT_T(y) (y << 15)
#define TEMPERING_SHIFT_L(y) (y >> 18)
static unsigned long mt[NN]; /* the array for the state vector  */
static int mti = NN + 1;     /* mti==NN+1 means mt[NN] is not initialized */
void sgenrand(unsigned long seed);
void lsgenrand(unsigned long seed_array[]);
double genrand();
double randf();
long randi(unsigned long LIM);
#endif // !_RANDOM_FUN_H_