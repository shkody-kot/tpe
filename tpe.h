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
	char * key;
	int iterations;
	int blocksize;
		
	struct pixels
	{
		uint8_t * image = nullptr;
		int block_height = 0;
		int block_width = 0;
		int width = 0;
		aes_rnd * sub = nullptr;
		aes_rnd * perm = nullptr;
	};
		
	static void encrypt_thread(tpe::pixels *, int, int);
};

#endif