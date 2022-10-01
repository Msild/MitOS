/*
	File: Source/lib/rand.c
	Copyright: MLworkshop
	Author: Msild
*/

unsigned int rand_seed = 1;

int rand(void)
{
	rand_seed = rand_seed * 1103515245 + 12345;
	return (unsigned int) (rand_seed >> 16) & 32767;
}
