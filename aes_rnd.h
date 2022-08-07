#ifndef _aes_rnd_
#define _aes_rnd_

#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

//crypto

class aes_rnd
{
	public:
	aes_rnd(uint8_t *, int);
	~aes_rnd();
	
	uint8_t next();
	uint8_t get_new_couple(uint8_t, uint8_t, bool);
	std::vector<int> get_new_permutation(int);
	
	int get_ctr();
	void set_ctr(int);
	
	private:
	struct heap
	{
		std::vector<uint8_t> key;
		int counter;
		std::vector<uint8_t> data;
	} * base = new heap;
	
};

#endif