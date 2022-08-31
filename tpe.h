#ifndef _tpe_
#define _tpe_

#include <vector>
#include <chrono>
#include <cmath>
#include <pthread.h>
#include "aes_rnd.h"

#define NUM_THREADS 3

class tpe
{
	public:
	tpe(char *, int, int);										//initiator
	
	~tpe();
	
	uint8_t *  encrypt(uint8_t *, uint8_t *, uint8_t *, int, int);	//ecrypt(data, sub_rndbox, per_rndbox)
	uint8_t * decrypt(uint8_t *, uint8_t *, uint8_t *, int, int);	//decrypt(data, sub_rndbox, per_rndbox)
	
	private:
	static void * generate(void * argument);
	
	struct pixels
	{
		uint8_t pixel1;
		uint8_t pixel2;
		bool encrypt;
		aes_rnd * random;
	};
	
	struct heap
	{
		char * key;
		int iterations;
		int blocksize;
	} * base = new heap;
};

#endif