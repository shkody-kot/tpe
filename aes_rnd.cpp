#include "aes_rnd.h"

aes_rnd::aes_rnd(uint8_t * array, int start, int need)
{
	base->counter = 0;
	base->data.clear();
	base->data.reserve(need);
	std::cout << "need: " << need << "; start: " << start << std::endl;
	
	for (int i = 0; i < need; i++) { base->data[i] = array[start + i]; }
}

aes_rnd::~aes_rnd()
{
	delete base;
}

uint8_t aes_rnd::next()
{
	return base->data[base->counter++];
}

uint8_t aes_rnd::prev() { return base->data[base->counter--]; }

void aes_rnd::set_ctr(int right) { base->counter = right; }

uint8_t aes_rnd::get_new_couple(uint8_t p, uint8_t q, bool encrypt)
{
	int rnd = this->next();
	int sum = p + q;
	//std::cout << "rnd: " << rnd << " sum: " << sum << std::endl;
	if (sum <= 255)
	{
		if (encrypt) { rnd = (p + rnd) % (sum + 1); }
		else { rnd = (p - rnd) % (sum + 1); }
		if (rnd < 0) { rnd = rnd + sum + 1; }
	}
	else
	{
		if (encrypt) { rnd = 255 - (p + rnd) % (511 - sum); }
		else 
		{
			rnd = (255 - p - rnd) % (511 - sum);
			while (rnd < (sum - 255)) { rnd += 511 - sum; }
		}
	}
	//std::cout << "rnd --> " << static_cast<int>(rnd) << "; " << std::endl;
	return rnd;
}

std::vector<int> aes_rnd::get_new_permutation(int block_size)
{
	int length = block_size * block_size;
	std::vector<int> indices(length);
	
	for (int i = 0; i < length; ++i) { indices[i] = i; }
	
	int temp;
	int next;
	
	//swaps the pixels within a block "randomly" 
	for (int index = 0; index < length; index++)
	{
		next = this->next() % length;		//generates an index within the block
		temp = indices[index];
		indices[index] = indices[next];
		indices[next] = temp;
	}
		
	return indices;
	
}

