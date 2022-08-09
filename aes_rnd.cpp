#include "aes_rnd.h"

aes_rnd::aes_rnd(uint8_t * array, int need)
{
	base->counter = 0;
	base->data.clear();
	base->data.reserve(need);
	
	for (int i = 0; i < need; i++) { base->data[i] = array[i]; }
}

aes_rnd::~aes_rnd()
{
	delete base;
}

uint8_t aes_rnd::next()
{
	return base->data[base->counter++];
}

void aes_rnd::set_ctr(int right) 
{ 
	//std::cout << "old: " << base->counter;
	base->counter = right;
	//std::cout << " updated: " << base->counter << std::endl;
}

uint8_t aes_rnd::get_new_couple(uint8_t p, uint8_t q, bool encrypt)
{
	uint8_t rnd = this->next();
	int sum = p + q;
	if (sum <= 255)
	{
		if (encrypt) { rnd = (p + rnd) % (sum + 1); }
		else { rnd = (p - rnd) % (sum + 1); }
		if (rnd < 0) { rnd = rnd + sum + 1; }
		//return rnd;
	}
	else
	{
		if (encrypt)
		{
			rnd = 255 - (p + rnd) % (511 - sum);
			//return rnd;
		}
		else 
		{
			rnd = (255 - p - rnd) % (511 - sum);
			while (rnd < (sum - 255)) { rnd += 511 - sum; }
			//return rnd;
		}
	}
	//std::cout << "rnd --> " << static_cast<int>(rnd) << "; " << std::endl;
	return rnd;
}

std::vector<int> aes_rnd::get_new_permutation(int block_size)
{
	int length = block_size * block_size;
	std::vector<int> indices(length);
	std::vector<uint8_t> permutation(length);
	
	for (int i = 0; i < length; ++i) { indices[i] = i; }
	for (int z = 0; z < length; z++) { permutation[z] = this->next(); }
	
	int temp;
	uint8_t next;
	
	//swaps the pixels within a block "randomly" 
	for (int index = 0; index < length; index++)
	{
		next = this->next() % length;
		temp = indices[index];
		indices[index] = indices[next];
		indices[next] = temp;
	}
	
	//sorts the pixels within a block by sorting permutation in increasing order
	//and matching the index with indices array
	/*bool sorted = false;
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
	}*/
	//std::cout << "end for permutation in aes_rand; blocksize = " << block_size << std::endl;
	
	return indices;
	
}

