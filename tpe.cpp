#include "tpe.h"

tpe::tpe(char * rkey, int riterations, int rblocksize)
{
	key = rkey;
	iterations = riterations;
	blocksize = rblocksize;
	//std::cout << key << " -- " << iterations << " -- " << blocksize << std::endl; 
}

tpe::~tpe()
{
	std::cout << "deleted" << std::endl;
}

uint8_t * tpe::encrypt(uint8_t * image, uint8_t * sub_array, uint8_t * perm_array, int width, int height)
{
	int m = std::floor(width / blocksize);
	int n = std::floor(height / blocksize);
	int third = std::floor(n / 3);
	int extra;
	
	if (n % 3 != 0) { extra = n % 3;}
	
	/*std::vector<int> permutation;
	uint8_t r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2;
	int p, q, x, y;*/
	
	//calculate sizes for each random number box	
	int total_for_perm = iterations * third * m * blocksize * blocksize;
	int total_for_sub = iterations * third * m * 
		(blocksize * blocksize - (blocksize * blocksize) % 2) / 2 * 3;
	
	int last_block_perm = iterations * (third + extra) * m * blocksize * blocksize;
	int last_block_sub = iterations * (third + extra) * m * 
		(blocksize * blocksize - (blocksize * blocksize) % 2) / 2 * 3;
	
	
	//create a pseudo random number box for both the subsitution and permutation
	aes_rnd * s_aes1 = new aes_rnd(sub_array, 0, total_for_sub); 
	aes_rnd * p_aes1 = new aes_rnd(perm_array, 0, total_for_perm);
	
	aes_rnd * s_aes2 = new aes_rnd(sub_array, total_for_sub, total_for_sub); 
	aes_rnd * p_aes2 = new aes_rnd(perm_array, total_for_perm, total_for_perm);
	
	aes_rnd * s_aes3 = new aes_rnd(sub_array, total_for_sub * 2, last_block_sub); 
	aes_rnd * p_aes3 = new aes_rnd(perm_array, total_for_perm * 2, last_block_perm);
	
	auto start = std::chrono::high_resolution_clock::now();
	
	std::cout << "initialized" << std::endl;
	
	pixels * one = new pixels;
	pixels * two = new pixels;
	pixels * three = new pixels;
	
	int third_of_pixels = third * m * blocksize * blocksize * 4;
	std::cout << "blocks: " << n;
	std::cout << " third of blocks: " << third << " third of pixels: " << third_of_pixels << std::endl;
	
	uint8_t * data1 = new uint8_t[third_of_pixels];
	uint8_t * data2 = new uint8_t[third_of_pixels];
	uint8_t * data3 = new uint8_t[(third + extra) * m * blocksize * blocksize * 4];
	
	for (int i = 0; i < third_of_pixels; i++)
	{
		data1[i] = image[i];
		data2[i] = image[third_of_pixels + i];		
	}
	
	for (int i = 0; i < (third + extra) * m * blocksize * blocksize * 4; i++) { data3[i] = image[(2 * third_of_pixels) + i]; }
	
	one->image = data1;
	one->block_height = third;
	one->block_width = m;
	one->width = width;
	one->sub = s_aes1;
	one->perm = p_aes1;
	
	two->image = data2;
	two->block_height = third;
	two->block_width = m;
	two->width = width;
	two->sub = s_aes2;
	two->perm = p_aes2;
	
	three->image = data3;
	three->block_height = third + extra;
	three->block_width = m;
	three->width = width;
	three->sub = s_aes3;
	three->perm = p_aes3;	
	
	std::thread thread1(encrypt_thread, one, blocksize, iterations);
	std::thread thread2(encrypt_thread, two, blocksize, iterations);
	std::thread thread3(encrypt_thread, three, blocksize, iterations);
	
	thread1.join();
	thread2.join();
	thread3.join();
	
	/*for (auto ccc = 0; ccc < iterations; ccc++)
	{
		//substitution begins
		//for each row of blocks...
		for (int i = 0; i < n; i++)
		{
			//for each column of blocks...
			for (int j = 0; j < m; j++)
			{
				//for each pixel within those blocks
				for (int k = 0; k < blocksize * blocksize; k += 2)
				{
					p = k / blocksize; 			//row# of pixel
					q = k % blocksize;			//col# of pixel
					x = (k + 1) / blocksize;		//adjacent pixel
					y = (k + 1) % blocksize;
					
					//fetch pixel pairs
					r1 = image[(i * width * blocksize + p * width + j * blocksize + q) * 4];
					g1 = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1];
					b1 = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2];
					
					r2 = image[(i * width * blocksize + x * width + j * blocksize + y) * 4];
					g2 = image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 1];
					b2 = image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 2];
					
					//generate random numbers fot pixels bt keep sum
					rt1 = red.pixel1;
					rt2 = r1 + r2 - rt1;
					
					gt1 = green.pixel1;
					gt2 = g1 + g2 - gt1;
					
					bt1 = blue.pixel1;
					bt2 = b1 + b2 - bt1;
					
					//replace pixels with random pixels
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4] = rt1;
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1] = gt1;
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2] = bt1;
					
					image[(i * width * blocksize + x * width + j * blocksize + y) * 4] = rt2;
					image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 1] = gt2;
					image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 2] = bt2;
				}
				
			}
		}
		
		//permutation
		uint8_t r, g, b;
		permutation.clear();
		std::vector<uint8_t> r_list, b_list, g_list;
		
		r_list.reserve(blocksize * blocksize);
		g_list.reserve(blocksize * blocksize);
		b_list.reserve(blocksize * blocksize);
		
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				//r_list.clear();
				//g_list.clear();
				//b_list.clear();
				
				for (int k = 0; k < blocksize * blocksize; k++)
				{
					p = std::floor(k / blocksize);
					q = k % blocksize;
					r = image[(i * width * blocksize + p * width + j * blocksize + q) * 4];
					g = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1];
					b = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2];
					r_list[k] = r;
					g_list[k] = g;
					b_list[k] = b;
				}
				
				permutation = p_aes->get_new_permutation(blocksize);
				
				for (int k = 0; k < blocksize * blocksize; k++)
				{
					p = std::floor(k / blocksize);
					q = k % blocksize;
										
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4] = r_list[permutation[k]];
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1] = g_list[permutation[k]];
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2] = b_list[permutation[k]];
				}
			}
		}
	}*/
	
	for (int i = 0; i < third_of_pixels; i++)
	{
		image[i] = one->image[i];
		image[third_of_pixels + i] = two->image[i];
	}
	
	for (int i = 0; i < (third + extra) * m * blocksize * blocksize * 4; i++) { image[(2 * third_of_pixels) + i] = three->image[i]; }
	
	std::cout << "permutation complete (main); blocksize: " << blocksize << std::endl;
	
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "encryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	std::cout << "TPE encrypt FIN" << std::endl;
	delete s_aes1;
	delete p_aes1;
	delete s_aes2;
	delete p_aes2;
	delete s_aes3;
	delete p_aes3;
	
	delete[] data1;
	delete[] data2;
	delete[] data3;
	
	delete one;
	delete two;
	delete three;
	
	return image;
}


uint8_t * tpe::decrypt(uint8_t * image, uint8_t * sub_array, uint8_t * perm_array, int width, int height)
{
	int m = std::floor(width / blocksize);
	int n = std::floor(height / blocksize);
	
	std::vector<int> permutation;
	uint8_t r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2;
	int p, q, x, y;
	
	int total_for_perm = iterations * n * m * blocksize * blocksize;
	int total_for_sub = iterations * n * m * 
		(blocksize * blocksize - (blocksize * blocksize) % 2) / 2 * 3;
	
	aes_rnd * s_aes = new aes_rnd(sub_array, 0, total_for_sub);
	aes_rnd * p_aes = new aes_rnd(perm_array, 0, total_for_perm);
	
	auto start = std::chrono::high_resolution_clock::now();	
	for (int ccc = 0; ccc < iterations; ccc++)
	{
		s_aes->set_ctr((iterations - (ccc + 1)) * (total_for_sub / iterations));
		p_aes->set_ctr((iterations - (ccc + 1)) * (total_for_perm / iterations));
		
		//permutation reverse
		uint8_t r, g, b;
		std::vector<uint8_t> r_list, g_list, b_list;
		
		r_list.reserve(blocksize * blocksize);
		g_list.reserve(blocksize * blocksize);
		b_list.reserve(blocksize * blocksize);
				
		for (int i = 0; i < n; i++)
		{			
			for (int j = 0; j < m; j++)
			{
				r_list.clear();
				g_list.clear();
				b_list.clear();
				
				//copy pixels per block into temporary lists by color
				for (int k = 0; k < blocksize * blocksize; k++)
				{
					p = k / blocksize;
					q = k % blocksize;
					r = image[(i * width * blocksize + p * width + j * blocksize + q) * 4];
					g = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1];
					b = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2];
					r_list[k] = r;
					g_list[k] = g;
					b_list[k] = b;
				}
				permutation = p_aes->get_new_permutation(blocksize);
				
				//unscramble pixels back into order they were in before encryption
				for (int k = 0; k < blocksize * blocksize; k++)
				{
					p = permutation[k] / blocksize;
					q = permutation[k] % blocksize;
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4] = r_list[k];
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1] = g_list[k];
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2] = b_list[k];
				}
			}
		}
		
		//substitution reverse
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < blocksize * blocksize - 1; k += 2)
				{
					p = k / blocksize;
					q = k % blocksize;
					x = (k + 1) / blocksize;
					y = (k + 1) % blocksize;
					
					//fetch pixel pairs
					r1 = image[(i * width * blocksize + p * width + j * blocksize + q) * 4];
					g1 = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1];
					b1 = image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2];
					
					r2 = image[(i * width * blocksize + x * width + j * blocksize + y) * 4];
					g2 = image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 1];
					b2 = image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 2];
					
					//generate the same random numbers but backwards this time
					rt1 = s_aes->get_new_couple(r1, r2, false);
					rt2 = r1 + r2 - rt1;
					
					gt1 = s_aes->get_new_couple(g1, g2, false);
					gt2 = g1 + g2 - gt1;
					
					bt1 = s_aes->get_new_couple(b1, b2, false);
					bt2 = b1 + b2 - bt1;
					
					//replace pixels with random pixels
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4] = rt1;
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 1] = gt1;
					image[(i * width * blocksize + p * width + j * blocksize + q) * 4 + 2] = bt1;
					
					image[(i * width * blocksize + x * width + j * blocksize + y) * 4] = rt2;
					image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 1] = gt2;
					image[(i * width * blocksize + x * width + j * blocksize + y) * 4 + 2] = bt2;
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

void tpe::encrypt_thread(pixels * data, int blocksize, int iterations)
{
	std::vector<int> permutation;
	uint8_t r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2;
	int p, q, x, y;
	
	for (auto ccc = 0; ccc < iterations; ccc++)
	{
		//substitution begins
		//for each row of blocks...
		for (int i = 0; i < data->block_height; i++)
		{
			//for each column of blocks...
			for (int j = 0; j < data->block_width; j++)
			{
				//for each pixel within those blocks
				for (int k = 0; k < blocksize * blocksize; k += 2)
				{
					p = k / blocksize; 			//row# of pixel
					q = k % blocksize;			//col# of pixel
					x = (k + 1) / blocksize;		//adjacent pixel
					y = (k + 1) % blocksize;
					
					//fetch pixel pairs
					r1 = data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4];
					g1 = data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 1];
					b1 = data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 2];
					
					r2 = data->image[(i * data->width * blocksize + x * data->width + j * blocksize + y) * 4];
					g2 = data->image[(i * data->width * blocksize + x * data->width + j * blocksize + y) * 4 + 1];
					b2 = data->image[(i * data->width * blocksize + x * data->width + j * blocksize + y) * 4 + 2];
					
					//generate random numbers fot pixels bt keep sum
					rt1 = data->sub->get_new_couple(r1, r2, true);
					rt2 = r1 + r2 - rt1;
					
					gt1 = data->sub->get_new_couple(g1, g2, true);
					gt2 = g1 + g2 - gt1;
					
					bt1 = data->sub->get_new_couple(b1, b2, true);
					bt2 = b1 + b2 - bt1;
					
					//replace pixels with random pixels
					data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4] = rt1;
					data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 1] = gt1;
					data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 2] = bt1;
					
					data->image[(i * data->width * blocksize + x * data->width + j * blocksize + y) * 4] = rt2;
					data->image[(i * data->width * blocksize + x * data->width + j * blocksize + y) * 4 + 1] = gt2;
					data->image[(i * data->width * blocksize + x * data->width + j * blocksize + y) * 4 + 2] = bt2;
				}
				
			}
		}
		
		//permutation
		uint8_t r, g, b;
		permutation.clear();
		std::vector<uint8_t> r_list, b_list, g_list;
		
		r_list.reserve(blocksize * blocksize);
		g_list.reserve(blocksize * blocksize);
		b_list.reserve(blocksize * blocksize);
		
		for (int i = 0; i < data->block_height; i++)
		{
			for (int j = 0; j < data->block_width; j++)
			{
				//r_list.clear();
				//g_list.clear();
				//b_list.clear();
				
				for (int k = 0; k < blocksize * blocksize; k++)
				{
					p = std::floor(k / blocksize);
					q = k % blocksize;
					r = data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4];
					g = data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 1];
					b = data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 2];
					r_list[k] = r;
					g_list[k] = g;
					b_list[k] = b;
				}
				
				permutation = data->perm->get_new_permutation(blocksize);
				
				for (int k = 0; k < blocksize * blocksize; k++)
				{
					p = std::floor(k / blocksize);
					q = k % blocksize;
										
					data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4] = r_list[permutation[k]];
					data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 1] = g_list[permutation[k]];
					data->image[(i * data->width * blocksize + p * data->width + j * blocksize + q) * 4 + 2] = b_list[permutation[k]];
				}
			}
		}
	}
	return;
}