#include <iostream>
#include <vector>
#include <cstring>
#include <emscripten.h>
#include "tpe.h"

//extern "C"
//{
	uint8_t * EMSCRIPTEN_KEEPALIVE decrypt(uint8_t *, uint8_t * , uint8_t *, int, int, tpe *);
	uint8_t * EMSCRIPTEN_KEEPALIVE encrypt(uint8_t *, uint8_t * , uint8_t *, int, int, tpe *);
	tpe * EMSCRIPTEN_KEEPALIVE create(char *, int, int, int);
//}

int main()
{
	std::cout << "Worked" << std::endl;
	/*int color1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int color2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int color3[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	char test[] = "testkey";
	
	tpe * image = create(test, 10, 1);
	encrypt(color1, color2, color3, 10, 1, image);
	decrypt(color1, color2, color3, 10, 1, image);
	delete image;*/
	
	return 0;
}

tpe * create(char * key, int iterations, int blockx, int blocky)
{
	tpe * image = new tpe(key, iterations, blockx, blocky);
	std::cout << "created a tpe object" << std::endl;
	return image;
}

uint8_t * encrypt(uint8_t * image_data, uint8_t * sub_array, uint8_t * perm_array, int width, int height, tpe * right)
{
	return right->encrypt(image_data, sub_array, perm_array, width, height);
}

uint8_t * decrypt(uint8_t * image_data, uint8_t * sub_array, uint8_t * perm_array, int width, int height, tpe * right)
{	
	return right->decrypt(image_data, sub_array, perm_array, width, height);
}