#include "tpe.h"

tpe::tpe(char * rkey, int riterations, int rblocksize)
{
	std::cout << rkey << std::endl;
	strcpy(base->key, rkey);
	base->iterations = riterations;
	base->blocksize = rblocksize;
	std::cout << base->key << " -- " << base->iterations << " -- " << base->blocksize << std::endl; 
}

tpe::~tpe()
{
	delete base;
}

void tpe::encrypt(std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, int width, int height)
{
	int m = std::floor(width / base->blocksize);
	int n = std::floor(height / base->blocksize);
	
	std::vector<int> permutation;
	int r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2, p, q, x, y;
	
	//substitute pixels	
	int total_for_perm = base->iterations * n * m * base->blocksize * base->blocksize;
	int total_for_sub = base->iterations * n * m * 
		(base->blocksize * base->blocksize - (base->blocksize * base->blocksize) % 2) / 2 * 3;
	
	auto start = std::chrono::high_resolution_clock::now();	
	aes_rnd * s_aes = new aes_rnd(base->key, total_for_sub);
	aes_rnd * p_aes = new aes_rnd(base->key, total_for_perm);
	auto endof_aes = std::chrono::high_resolution_clock::now();
	
	for (int ccc = 0; ccc < base->iterations; ccc++)
	{
		//substitution
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < base->blocksize * base->blocksize - 1; k += 2)
				{
					p = k / base->blocksize;
					q = k % base->blocksize;
					x = (k + 1) / base->blocksize;
					y = (k + 1) % base->blocksize;
					
					//fetch pixel pairs
					r1 = red[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					g1 = green[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					b1 = blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					
					r2 = red[(i * width * base->blocksize + x * width + j * base->blocksize + y)];
					g2 = green[(i * width * base->blocksize + x * width + j * base->blocksize + y)];
					b2 = blue[(i * width * base->blocksize + x * width + j * base->blocksize + y)];
					
					//generate random numbers fot pixels bt keep sum
					rt1 = s_aes->get_new_couple(r1, r2, true);
					rt2 = r1 + r2 - rt1;
					
					gt1 = s_aes->get_new_couple(g1, g2, true);
					gt2 = g1 + g2 - gt1;
					
					bt1 = s_aes->get_new_couple(b1, b2, true);
					bt2 = b1 + b2 - bt1;
					
					//replace pixels with random pixels
					red[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = rt1;
					green[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = gt1;
					blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = bt1;
					
					red[(i * width * base->blocksize + x * width + j * base->blocksize + y)] = rt2;
					green[(i * width * base->blocksize + x * width + j * base->blocksize + y)] = gt2;
					blue[(i * width * base->blocksize + x * width + j * base->blocksize + y)] = bt2;
				}
			}
		}
		//permutation
		int r, g, b;
		std::vector<int> r_list, b_list, g_list;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				r_list.clear();
				g_list.clear();
				b_list.clear();
				for (int k = 0; k < base->blocksize * base->blocksize; k++)
				{
					p = k / base->blocksize;
					q = k % base->blocksize;
					r = red[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					g = green[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					b = blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					r_list.push_back(r);
					g_list.push_back(g);
					b_list.push_back(b);
				}
				
				permutation = p_aes->get_new_permutation(base->blocksize);
				for (int k =0; k < base->blocksize * base->blocksize; k++)
				{
					p = k / base->blocksize;
					q = k % base->blocksize;
					red[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = r_list[permutation[k]];
					green[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = g_list[permutation[k]];
					blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = b_list[permutation[k]];
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "aes: " << std::chrono::duration_cast<std::chrono::milliseconds>(endof_aes - start).count() << std::endl;
	std::cout << "after aes: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - endof_aes).count() << std::endl;
	std::cout << "TPE encrypt FIN" << std::endl;
	delete s_aes;
	delete p_aes;
}


void tpe::decrypt(std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, int width, int height)
{
	int m = std::floor(width / base->blocksize);
	int n = std::floor(height / base->blocksize);
	
	std::vector<int> permutation;
	int r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2, p, q, x, y;
	
	//substitute pixels	
	int total_for_perm = base->iterations * n * m * base->blocksize * base->blocksize;
	int total_for_sub = base->iterations * n * m * 
		(base->blocksize * base->blocksize - (base->blocksize * base->blocksize) % 2) / 2 * 3;
	
	aes_rnd * s_aes = new aes_rnd(base->key, total_for_sub);
	aes_rnd * p_aes = new aes_rnd(base->key, total_for_perm);
	
	auto start = std::chrono::high_resolution_clock::now();	
	for (int ccc = 0; ccc < base->iterations; ccc++)
	{
		s_aes->set_ctr(base->iterations - (ccc + 1) * (total_for_sub / base->iterations));
		p_aes->set_ctr(base->iterations - (ccc + 1) * (total_for_perm / base->iterations));
		
		//permutation reverse
		int r, g, b;
		std::vector<int> r_list, g_list, b_list;
		for (int i = 0; i < n; i++)
		{			
			for (int j = 0; j < m; j++)
			{
				r_list.clear();
				g_list.clear();
				b_list.clear();
				for (int k= 0; k < base->blocksize * base->blocksize; k++)
				{
					p = k / base->blocksize;
					q = k % base->blocksize;
					r = red[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					g = green[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					b = blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					r_list.push_back(r);
					g_list.push_back(g);
					b_list.push_back(b);
				}
				permutation = p_aes->get_new_permutation(base->blocksize);
				
				for (int k = 0; k < base->blocksize * base->blocksize; k++)
				{
					p = permutation[k] / base->blocksize;
					q = k % base->blocksize;
					red[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = r_list[k];
					green[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = g_list[k];
					blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = b_list[k];
				}
			}
		}
		//substitution reverse
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < base->blocksize * base->blocksize - 1; k += 2)
				{
					p = k / base->blocksize;
					q = k % base->blocksize;
					x = (k + 1) / base->blocksize;
					y = (k + 1) % base->blocksize;
					
					//fetch pixel pairs
					r1 = red[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					g1 = green[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					b1 = blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)];
					
					r2 = red[(i * width * base->blocksize + x * width + j * base->blocksize + y)];
					g2 = green[(i * width * base->blocksize + x * width + j * base->blocksize + y)];
					b2 = blue[(i * width * base->blocksize + x * width + j * base->blocksize + y)];
					
					//generate the same random numbers but backwards this time
					rt1 = s_aes->get_new_couple(r1, r2, false);
					rt2 = r1 + r2 - rt1;
					
					gt1 = s_aes->get_new_couple(g1, g2, false);
					gt2 = g1 + g2 - gt1;
					
					bt1 = s_aes->get_new_couple(b1, b2, false);
					bt2 = b1 + b2 - bt1;
					
					//replace pixels with random pixels
					red[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = rt1;
					green[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = gt1;
					blue[(i * width * base->blocksize + p * width + j * base->blocksize + q)] = bt1;
					
					red[(i * width * base->blocksize + x * width + j * base->blocksize + y)] = rt2;
					green[(i * width * base->blocksize + x * width + j * base->blocksize + y)] = gt2;
					blue[(i * width * base->blocksize + x * width + j * base->blocksize + y)] = bt2;
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "decryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	std::cout << "TPE decrypt FIN" << std::endl;
	delete s_aes;
	delete p_aes;
}