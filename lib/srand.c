/*
	File: Source/MitOS/lib/srand.c
	Copyright: MLworkshop
	Author: Msild
*/

extern unsigned int rand_seed;

void srand(unsigned int seed)
{
	rand_seed = seed;
}
