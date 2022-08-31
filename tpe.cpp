#include "tpe.h"

tpe::tpe(char * rkey, int riterations, int rblocksize)
{
	std::cout << rkey << std::endl;				//important for whatever reason?????
	strcpy(base->key, rkey);
	base->iterations = riterations;
	base->blocksize = rblocksize;
	//std::cout << base->key << " -- " << base->iterations << " -- " << base->blocksize << std::endl; 
}

tpe::~tpe()
{
	delete base;
}

uint8_t * tpe::encrypt(uint8_t * image, uint8_t * sub_array, uint8_t * perm_array, int width, int height)
{
	int m = std::floor(width / base->blocksize);
	int n = std::floor(height / base->blocksize);
	
	std::vector<int> permutation;
	uint8_t r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2;
	int p, q, x, y;
	
	//calculate sizes for each random number box	
	int total_for_perm = base->iterations * n * m * base->blocksize * base->blocksize;
	int total_for_sub = base->iterations * n * m * 
		(base->blocksize * base->blocksize - (base->blocksize * base->blocksize) % 2) / 2 * 3;
	
	auto start = std::chrono::high_resolution_clock::now();	
	//create a pseudo random number box for both the subsitution and permutation
	aes_rnd * s_aes = new aes_rnd(sub_array, total_for_sub); 
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
				for (int k = 0; k < base->blocksize * base->blocksize; k += 2)
				{
					p = k / base->blocksize; 			//row# of pixel
					q = k % base->blocksize;			//col# of pixel
					x = (k + 1) / base->blocksize;		//adjacent pixel
					y = (k + 1) % base->blocksize;
					
					//fetch pixel pairs
					r1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4];
					g1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1];
					b1 = image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2];
					
					r2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4];
					g2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 1];
					b2 = image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 2];
					
					//set up threads
					int r_thread, b_thread, g_thread;
					pthread_t threads[NUM_THREADS];
					
					struct pixels * red, * green, * blue;
					//set structs with arguments for get_new_couple function to pass to threads
					red->pixel1 = r1;
					red->pixel2 = r2;
					red->encrypt = true;
					red->random = s_aes;
					
					green->pixel1 = g1;
					green->pixel2 = g2;
					green->encrypt = true;
					green->random = s_aes;
					
					blue->pixel1 = b1;
					blue->pixel2 = b2;
					blue->encrypt = true;
					blue->random = s_aes;
					
					//create threads to generate random pixels
					r_thread = pthread_create(&threads[0], NULL, generate, (void *)red);
					g_thread = pthread_create(&threads[1], NULL, generate, (void *)green);
					b_thread = pthread_create(&threads[2], NULL, generate, (void *)blue);
					
					pthread_join(threads[0], NULL);
					pthread_join(g_thread, NULL);
					pthread_join(b_thread, NULL);
					
					//generate random numbers fot pixels bt keep sum
					/*rt1 = red.pixel1;
					rt2 = r1 + r2 - rt1;
					
					gt1 = green.pixel1;
					gt2 = g1 + g2 - gt1;
					
					bt1 = blue.pixel1;
					bt2 = b1 + b2 - bt1;*/
					
					//replace pixels with random pixels
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4] = red->pixel1;
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 1] = green->pixel1;
					image[(i * width * base->blocksize + p * width + j * base->blocksize + q) * 4 + 2] = blue->pixel1;
					
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4] = red->pixel2;
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 1] = green->pixel2;
					image[(i * width * base->blocksize + x * width + j * base->blocksize + y) * 4 + 2] = blue->pixel2;
				}
			}
		}
		
		//permutation
		uint8_t r, g, b;
		permutation.clear();
		std::vector<uint8_t> r_list, b_list, g_list;
		
		r_list.reserve(base->blocksize * base->blocksize);
		g_list.reserve(base->blocksize * base->blocksize);
		b_list.reserve(base->blocksize * base->blocksize);
		
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				//r_list.clear();
				//g_list.clear();
				//b_list.clear();
				
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
	//for (int i = 0; i < width * height; i++) { std::cout << static_cast<int>(image[i]) << "-"; }
	//std::cout << std::endl;
	return image;
}


uint8_t * tpe::decrypt(uint8_t * image, uint8_t * sub_array, uint8_t * perm_array, int width, int height)
{
	int m = std::floor(width / base->blocksize);
	int n = std::floor(height / base->blocksize);
	
	std::vector<int> permutation;
	uint8_t r1, b1, g1, r2, b2, g2, rt1, gt1, bt1, rt2, gt2, bt2;
	int p, q, x, y;
	
	int total_for_perm = base->iterations * n * m * base->blocksize * base->blocksize;
	int total_for_sub = base->iterations * n * m * 
		(base->blocksize * base->blocksize - (base->blocksize * base->blocksize) % 2) / 2 * 3;
	
	aes_rnd * s_aes = new aes_rnd(sub_array, total_for_sub);
	aes_rnd * p_aes = new aes_rnd(perm_array, total_for_perm);
	
	auto start = std::chrono::high_resolution_clock::now();	
	for (int ccc = 0; ccc < base->iterations; ccc++)
	{
		s_aes->set_ctr((base->iterations - (ccc + 1)) * (total_for_sub / base->iterations));
		p_aes->set_ctr((base->iterations - (ccc + 1)) * (total_for_perm / base->iterations));
		
		//permutation reverse
		uint8_t r, g, b;
		std::vector<uint8_t> r_list, g_list, b_list;
		
		r_list.reserve(base->blocksize * base->blocksize);
		g_list.reserve(base->blocksize * base->blocksize);
		b_list.reserve(base->blocksize * base->blocksize);
				
		for (int i = 0; i < n; i++)
		{			
			for (int j = 0; j < m; j++)
			{
				r_list.clear();
				g_list.clear();
				b_list.clear();
				
				//copy pixels per block into temporary lists by color
				for (int k = 0; k < base->blocksize * base->blocksize; k++)
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
				
				//unscramble pixels back into order they were in before encryption
				for (int k = 0; k < base->blocksize * base->blocksize; k++)
				{
					p = permutation[k] / base->blocksize;
					q = permutation[k] % base->blocksize;
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
				for (int k = 0; k < base->blocksize * base->blocksize - 1; k += 2)
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

void * tpe::generate(void * argument)
{
	struct pixels * data;
	uint8_t temp;
	data = (struct pixels *) argument;
	temp = data->random->get_new_couple(data->pixel1, data->pixel2, data->encrypt);
	data->pixel2 = data->pixel1 + data->pixel2 - temp;
	data->pixel1 = temp;
	std::cout << "within thread; "; 
	
	pthread_exit(NULL);
}