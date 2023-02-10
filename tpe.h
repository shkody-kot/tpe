#ifndef _tpe_
#define _tpe_

#include <vector>
#include <chrono>
#include <cmath>
#include "aes_rnd.h"

class tpe
{
	public:
	tpe(char *, int, int, int);										//initiator
	
	~tpe();
	
	uint8_t *  encrypt(uint8_t *, uint8_t *, uint8_t *, int, int);	//ecrypt(data, sub_rndbox, per_rndbox)
	uint8_t * decrypt(uint8_t *, uint8_t *, uint8_t *, int, int);	//decrypt(data, sub_rndbox, per_rndbox)
	
	private:
	struct heap
	{
		char * key;
		int iterations;
		int block_x;
		int block_y;
		int blocksize;
	} * base = new heap;
};

#endif