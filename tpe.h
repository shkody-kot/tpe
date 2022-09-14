#ifndef _tpe_
#define _tpe_

#include <vector>
#include <chrono>
#include <cmath>
#include <thread>
#include "aes_rnd.h"

class tpe
{
	public:
	tpe(char *, int, int);										//initiator
	
	~tpe();
	
	uint8_t * encrypt(uint8_t *, uint8_t *, uint8_t *, int, int);	//ecrypt(data, sub_rndbox, per_rndbox)
	uint8_t * decrypt(uint8_t *, uint8_t *, uint8_t *, int, int);	//decrypt(data, sub_rndbox, per_rndbox)
	
	private:	
	struct pixels
	{
		uint8_t pixel1 = 0;
		uint8_t pixel2 = 0;
		bool encrypt = false;
		aes_rnd * random = nullptr;
	};
	
	static void generate(tpe::pixels *);
	
	struct heap
	{
		char * key;
		int iterations;
		int blocksize;
	} * base = new heap;
};

#endif