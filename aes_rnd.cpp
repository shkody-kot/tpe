#include "aes_rnd.h"

aes_rnd::aes_rnd(uint8_t * array)
{
	base->counter = 0;
	base->data.clear();
	base->data.reserve(sizeof(*array));
	for (int i : base->data) { i = array[i]; }
}

aes_rnd::~aes_rnd()
{
	delete base;
}

uint8_t aes_rnd::next()
{
	//std::cout << "aes.next();" << std::endl;
	return base->data[base->counter++];
}

int aes_rnd::get_ctr() { return base->counter; }

void aes_rnd::set_ctr(int right) { base->counter = right; }

uint8_t aes_rnd::get_new_couple(uint8_t p, uint8_t q, bool encrypt)
{
	uint8_t rnd = this->next();
	uint8_t sum = p + q;
	if (sum <= 255)
	{
		if (encrypt) { rnd = (p + rnd) % (sum + 1); }
		else { rnd = (p - rnd) % (sum + 1); }
		if (rnd < 0) { rnd = rnd + sum + 1; }
	}
	else
	{
		if (encrypt)
		{
			rnd = 255 - (p - rnd) % (511 - sum);
		}
		else 
		{
			rnd = (255 - p - rnd) % (511 - sum);
			while (rnd < (sum - 255)) { rnd += 511 - sum; }
		}
	}
	return rnd;
}

std::vector<int> aes_rnd::get_new_permutation(int block_size)
{
	int length = block_size * block_size;
	std::vector<uint8_t> permutation(length);
	std::vector<int> indices(length);
	
	for (int z = 0; z < length; z++) { permutation[z] = this->next(); }
	
	for (int i = 0; i < length; ++i) { indices[i] = i; }
	
	//sort
	bool sorted = false;
	bool swapped;
	int temp;
	while (!sorted)
	{
		for (int index = 0; index < length - 1; index++)
		{
			if (permutation[index] > permutation[index + 1])
			{
				temp = indices[index];
				indices[index] = indices[index + 1];
				indices[index + 1] = temp;
				
				temp = permutation[index];
				permutation[index] = permutation[index + 1];
				permutation[index + 1] = temp;
				
				swapped = true;
			}
		}
		if (swapped) { swapped = false; }		//loop again
		else { sorted = true; }					//break
	}
	
	std::cout << "end for permutation in aes_rand; blocksize = " << block_size << std::endl;
	
	return indices;
	
}

std::vector<uint8_t> aes_rnd::import(char * right)
{
	std::vector<uint8_t> key(sizeof(right)); 			//128 in this case
	for (int index = 0; index < sizeof(right); index++)
	{
		key[index] = right[index];
	}
	return key;
}

void aes_rnd::encrypt(int need)
{
	std::cout << "need = " << need << std::endl;
	std::vector<int> iv(16);
	std::vector<int> plain(need);
	std::vector<int> buffer(need);
	
	//this is where the encryption happens????
	//encrypt plain into buffer using key and iv
	//don't do the "converting it back to string" because i need an int vector
	
	
	for (auto index = 0; index < need; index++)
	{
		//this doesn't work because it's a menace (deragotory)
		base->data[index] = std::rand() % 256;
	}
	
	//for (int i : base->data) { std::cout << i << "-"; }
	std::cout << std::endl;
	std::cout << "for testing purposes, random numbers generated" << std::endl;
}
