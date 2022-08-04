#ifndef _aes_rnd_
#define _aes_rnd_

#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

//crypto
//#include "filters.h"
//#include "aes.h"
//#include "ccm.h"
//#include "assert.h"

class aes_rnd
{
	public:
	aes_rnd(char *, int);
	~aes_rnd();
	
	int next();
	int get_new_couple(int, int, bool);
	std::vector<int> get_new_permutation(int);
	
	int get_ctr();
	void set_ctr(int);
	
	private:
	std::vector<uint8_t> import(char *);
	void encrypt(int);
	struct heap
	{
		std::vector<uint8_t> key;
		int counter;
		std::vector<int> data;
	} * base = new heap;
	
};

#endif