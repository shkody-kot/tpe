#ifndef _tpe_
#define _tpe_

#include <vector>
#include <chrono>
#include <cmath>
#include "aes_rnd.h"

class tpe
{
	public:
	tpe(char *, int, int);										//initiator
	
	~tpe();
	
	void encrypt(std::vector<uint8_t> &, std::vector<uint8_t> &, std::vector<uint8_t> &, int, int);	//ecrypt(r, g, b)
	void decrypt(std::vector<uint8_t> &, std::vector<uint8_t> &, std::vector<uint8_t> &, int, int);	//decrypt(r, g, b)
	
	private:
	struct heap
	{
		char * key;
		int iterations;
		int blocksize;
	} * base = new heap;
};

#endif