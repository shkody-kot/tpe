#ifndef _aes_rnd_
#define _aes_rnd_

#include <iostream>
#include <vector>
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
	
	void set_ctr(int);
	
	private:
	struct heap
	{
		int counter;
		std::vector<uint8_t> data;
	} * base = new heap;
	
};

#endif