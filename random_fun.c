#include "random_fun.h"

void sgenrand(unsigned long seed)
{
	int i;
	for (i = 0; i < NN; i++)
	{
		mt[i] = seed & 0xffff0000;
		seed = 69069 * seed + 1;
		mt[i] |= (seed & 0xffff0000) >> 16;
		seed = 69069 * seed + 1;
	}
	mti = NN;
}
void lsgenrand(unsigned long seed_array[])
{
	int i;
	for (i = 0; i < NN; i++)
		mt[i] = seed_array[i];
	mti = NN;
}
double genrand()
{
	unsigned long y;
	static unsigned long mag01[2] = {0x0, MATRIX_A};
	if (mti >= NN)
	{
		int kk;
		if (mti == NN + 1)
			sgenrand(4357);
		for (kk = 0; kk < NN - MM; kk++)
		{
			y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
			mt[kk] = mt[kk + MM] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (; kk < NN - 1; kk++)
		{
			y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
			mt[kk] = mt[kk + (MM - NN)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (mt[NN - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
		mt[NN - 1] = mt[MM - 1] ^ (y >> 1) ^ mag01[y & 0x1];
		mti = 0;
	}
	y = mt[mti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);
	return y;
}
double randf()
{
	return ((double)genrand() * 2.3283064370807974e-10);
}

long randi(unsigned long LIM)
{
	return ((unsigned long)genrand() % LIM);
}