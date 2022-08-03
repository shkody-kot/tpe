#include "aes_rnd.h"

aes_rnd::aes_rnd(char * rkey, int total_need)
{
	base->counter = 0;
	base->data.clear();
	base->data.resize(total_need);
	if (sizeof(rkey) == 0) {std::cout << "key not set!" << std::endl; }
	else
	{
		//temporary
		std::cout << "key set" << std::endl;
		base->key = import(rkey);
		std::cout << "key 'imported'" << std::endl;
		encrypt(total_need);
	}
}

aes_rnd::~aes_rnd()
{
	delete base;
}

int aes_rnd::next()
{
	//std::cout << "aes.next();" << std::endl;
	base->counter++;
	return base->data[base->counter - 1];
}

int aes_rnd::get_ctr() { return base->counter; }

void aes_rnd::set_ctr(int right) { base->counter = right; }

int aes_rnd::get_new_couple(int p, int q, bool encrypt)
{
	int rnd = this->next();
	int sum = p + q;
	if (sum <= 255)
	{
		if (encrypt) { rnd = (p + rnd) % (sum + 1); }
		else { rnd = (p - rnd) % (sum + 1); }
		if (rnd < 0) { rnd = rnd + sum + 1; }
		return rnd;
	}
	else
	{
		if (encrypt)
		{
			rnd = 255 - (p - rnd) % (511 - sum);
			return rnd;
		}
		else 
		{
			rnd = (255 - p - rnd) % (511 - sum);
			while (rnd < (sum - 255)) { rnd += 511 - sum; }
			return rnd;
		}
	}
}

std::vector<int> aes_rnd::get_new_permutation(int block_size)
{
	std::vector<int> permutation;
	int length = block_size * block_size;
	std::vector<int> indices(length);
	
	for (int z = 0; z < length; z++) { permutation.push_back(this->next()); }
	
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
				swapped = true;
			}
		}
		if (swapped) { swapped = false; }		//loop again
		else { sorted = true; }					//break
	}
	
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
	/*std::vector<int> * iv = new std::vector<int>(16);
	std::vector<int> * plain = new std::vector<int>(need);
	std::vector<int> * buffer = new std::vector<int>(need);*/
	
	//this is where the encryption happens????
	//encrypt plain into buffer using key and iv
	//don't do the "converting it back to string" because i need an int vector
	
	for (auto index = 0; index < need; index++)
	{
		base->data[index] = rand() % 256;
	}
	std::cout << "for testing purposes, random numbers generated" << std::endl;
	/*delete iv;
	delete buffer;
	delete plain;*/
}
