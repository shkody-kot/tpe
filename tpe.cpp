#include "tpe.h"

tpe::tpe(char * rkey, int riterations, int rblocksize)
{
	std::cout << rkey << std::endl;				//important for whatever reason?????
	strcpy(base->key, rkey);
	base->iterations = riterations;
	base->blocksize = rblocksize;
	std::cout << base->key << " -- " << base->iterations << " -- " << base->blocksize << std::endl; 
}

tpe::~tpe()
{
	delete base;
}

uint8_t * tpe::encrypt(uint8_t * image, uint8_t * sub_array, uint8_t * perm_array, int width, int height)
{
	//make srruct to contain pixel blocks instead of 3 vecotrs
	int m = std::floor(width / base->blocksize);
	int n = std::floor(height / base->blocksize);
	
	std::cout << "m: " << m << " n: " << n << std::endl; 
	
	std::vector<int> permutation;
	uint8_t r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2;
	int p, q, x, y;
	
	//substitute pixels	
	int total_for_perm = base->iterations * n * m * base->blocksize * base->blocksize;
	int total_for_sub = base->iterations * n * m * 
		(base->blocksize * base->blocksize - (base->blocksize * base->blocksize) % 2) / 2 * 3;
	
	auto start = std::chrono::high_resolution_clock::now();	
	aes_rnd * s_aes = new aes_rnd(sub_array, total_for_sub);
	//this is where it breaks 
	aes_rnd * p_aes = new aes_rnd(perm_array, total_for_perm);
	auto endof_aes = std::chrono::high_resolution_clock::now();
	
	std::cout << "initialized" << std::endl;
	
	for (auto ccc = 0; ccc < base->iterations; ccc++)
	{
		//substitution begins
		//for each row of blocks...
		for (int i = 0; i < n; i++)
		{
			//for each column of blocks...
			for (int j = 0; j < m; j++)
			{
				//for each pixel within those blocks
				for (int k = 0; k < base->blocksize * base->blocksize - 1; k = k + 2)
				{
					p = std::floor(k / base->blocksize); 			//row# of pixel
					q = k % base->blocksize;			//col# of pixel
					x = std::floor((k + 1) / base->blocksize);		//adjacent pixel
					y = (k + 1) % base->blocksize;
					
					//fetch pixel pairs
					r1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4];
					g1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1];
					b1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2];
					
					r2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4];
					g2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 1];
					b2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 2];
					
					//generate random numbers fot pixels bt keep sum
					rt1 = s_aes->get_new_couple(r1, r2, true);
					rt2 = r1 + r2 - rt1;
					
					gt1 = s_aes->get_new_couple(g1, g2, true);
					gt2 = g1 + g2 - gt1;
					
					bt1 = s_aes->get_new_couple(b1, b2, true);
					bt2 = b1 + b2 - bt1;
					
					//replace pixels with random pixels
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4] = rt1;
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1] = gt1;
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2] = bt1;
					
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4] = rt2;
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 1] = gt2;
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 2] = bt2;
				}
			}
		}
		
		std::cout << "substituted" << std::endl;
		
		//permutation
		uint8_t r, g, b;
		std::vector<uint8_t> r_list, b_list, g_list;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				r_list.clear();
				g_list.clear();
				b_list.clear();
				
				r_list.reserve(base->blocksize * base->blocksize);
				g_list.reserve(base->blocksize * base->blocksize);
				b_list.reserve(base->blocksize * base->blocksize);
				
				for (int k = 0; k < base->blocksize * base->blocksize; k++)
				{
					p = std::floor(k / base->blocksize);
					q = k % base->blocksize;
					r = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4];
					g = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1];
					b = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2];
					r_list[k] = r;
					g_list[k] = g;
					b_list[k] = b;
				}
				
				permutation = p_aes->get_new_permutation(base->blocksize);
				
				for (int k = 0; k < base->blocksize * base->blocksize; k++)
				{
					p = std::floor(k / base->blocksize);
					q = k % base->blocksize;
										
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4] = r_list[permutation[k]];
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1] = g_list[permutation[k]];
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2] = b_list[permutation[k]];
				}
			}
		}
	}
	
	std::cout << "permutation complete (main); blocksize: " << base->blocksize << std::endl;
	
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "aes: " << std::chrono::duration_cast<std::chrono::milliseconds>(endof_aes - start).count() << std::endl;
	std::cout << "after aes: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - endof_aes).count() << std::endl;
	std::cout << "TPE encrypt FIN" << std::endl;
	delete s_aes;
	delete p_aes;
	return image;
}


uint8_t * tpe::decrypt(uint8_t * image, uint8_t * sub_array, uint8_t * perm_array, int width, int height)
{
	int m = std::floor(width / base->blocksize);
	int n = std::floor(height / base->blocksize);
	
	std::vector<int> permutation;
	uint8_t r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2;
	int p, q, x, y;
	
	//substitute pixels	
	int total_for_perm = base->iterations * n * m * base->blocksize * base->blocksize;
	int total_for_sub = base->iterations * n * m * 
		(base->blocksize * base->blocksize - (base->blocksize * base->blocksize) % 2) / 2 * 3;
	
	aes_rnd * s_aes = new aes_rnd(sub_array, total_for_sub);
	aes_rnd * p_aes = new aes_rnd(perm_array, total_for_perm);
	
	auto start = std::chrono::high_resolution_clock::now();	
	for (int ccc = 0; ccc < base->iterations; ccc++)
	{
		s_aes->set_ctr(base->iterations - (ccc + 1) * (total_for_sub / base->iterations));
		p_aes->set_ctr(base->iterations - (ccc + 1) * (total_for_perm / base->iterations));
		
		//permutation reverse
		uint8_t r, g, b;
		std::vector<uint8_t> r_list, g_list, b_list;
		for (int i = 0; i < n; i++)
		{			
			for (int j = 0; j < m; j++)
			{
				r_list.clear();
				g_list.clear();
				b_list.clear();
				
				r_list.reserve(base->blocksize * base->blocksize);
				g_list.reserve(base->blocksize * base->blocksize);
				b_list.reserve(base->blocksize * base->blocksize);
				
				for (int k= 0; k < base->blocksize * base->blocksize; k++)
				{
					p = k / base->blocksize;
					q = k % base->blocksize;
					r = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4];
					g = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1];
					b = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2];
					r_list[k] = r;
					g_list[k] = g;
					b_list[k] = b;
				}
				permutation = p_aes->get_new_permutation(base->blocksize);
				
				for (int k = 0; k < base->blocksize * base->blocksize; k++)
				{
					p = permutation[k] / base->blocksize;
					q = k % base->blocksize;
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4] = r_list[k];
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1] = g_list[k];
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2] = b_list[k];
				}
			}
		}
		//substitution reverse
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < base->blocksize * base->blocksize - 1; k = k + 2)
				{
					p = k / base->blocksize;
					q = k % base->blocksize;
					x = (k + 1) / base->blocksize;
					y = (k + 1) % base->blocksize;
					
					//fetch pixel pairs
					r1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4];
					g1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1];
					b1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2];
					
					r2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4];
					g2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 1];
					b2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 2];
					
					//generate the same random numbers but backwards this time
					rt1 = s_aes->get_new_couple(r1, r2, false);
					rt2 = r1 + r2 - rt1;
					
					gt1 = s_aes->get_new_couple(g1, g2, false);
					gt2 = g1 + g2 - gt1;
					
					bt1 = s_aes->get_new_couple(b1, b2, false);
					bt2 = b1 + b2 - bt1;
					
					//replace pixels with random pixels
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4] = rt1;
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1] = gt1;
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2] = bt1;
					
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4] = rt2;
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 1] = gt2;
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 2] = bt2;
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "decryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	std::cout << "TPE decrypt FIN" << std::endl;
	delete s_aes;
	delete p_aes;
	return image;
}