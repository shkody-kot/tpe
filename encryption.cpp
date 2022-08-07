#include <iostream>
#include <vector>
#include <cstring>
#include <emscripten.h>
#include "tpe.h"

//extern "C"
//{
	void EMSCRIPTEN_KEEPALIVE decrypt(uint8_t *, uint8_t * , uint8_t *, int, int, tpe *);
	void EMSCRIPTEN_KEEPALIVE encrypt(uint8_t *, uint8_t * , uint8_t *, int, int, tpe *);
	tpe * EMSCRIPTEN_KEEPALIVE create(char *, int, int);
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

tpe * create(char * key, int iterations, int block)
{
	tpe * image = new tpe(key, iterations, block);
	std::cout << "created a tpe object" << std::endl;
	return image;
}

void encrypt(uint8_t * r, uint8_t * g, uint8_t * b, int width, int height, tpe * right)
{
	int size = width * height;
	
	std::vector<uint8_t> red(r, r + size);
	std::vector<uint8_t> green(g, g + size);
	std::vector<uint8_t> blue(b, b + size);
	
	std::cout << "vectors created" << std::endl;

	right->encrypt(red, green, blue, width, height);
	
	for (auto index = 0; index < sizeof(*r); index++)
	{
		r[index] = red[index];
		g[index] = green[index];
		b[index] = blue[index];
	}
	
	for (int i = 0; i < size; i++) {std::cout << r[i] << " "; }
	std::cout << std::endl;
}

void decrypt(uint8_t * r, uint8_t * g, uint8_t * b, int width, int height, tpe * right)
{
	int size = width * height;
	std::cout << size << " --> ";
	for (int i = 0; i < size; i++) {std::cout << r[i] << " "; }
	std::cout << std::endl;
	
	std::vector<uint8_t> red(r, r + size);
	std::vector<uint8_t> green(g, g + size);
	std::vector<uint8_t> blue(b, b + size);
	
	right->decrypt(red, green, blue, width, height);
	
	for (auto index = 0; index < sizeof(*r); index++)
	{
		r[index] = red[index];
		g[index] = green[index];
		b[index] = blue[index];
	}
	
	for (int i = 0; i < size; i++) {std::cout << r[i] << " "; }
	std::cout << std::endl;
}