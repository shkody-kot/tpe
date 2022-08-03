#include <iostream>
#include <vector>
#include <cstring>
#include <emscripten.h>
#include "tpe.h"

//extern "C"
//{
	void EMSCRIPTEN_KEEPALIVE decrypt(int *, int * , int *, int, int, tpe *);
	void EMSCRIPTEN_KEEPALIVE encrypt(int *, int * , int *, int, int, tpe *);
	tpe * EMSCRIPTEN_KEEPALIVE create(char *, int, int);
//}

int main()
{
	std::cout << "Worked" << std::endl;
	int color1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int color2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int color3[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	char test[] = "testkey";
	
	tpe * image = create(test, 10, 20);
	encrypt(color1, color2, color3, 10, 1, image);
	decrypt(color1, color2, color3, 10, 1, image);
	delete image;
	
	return 0;
}

tpe * create(char * key, int iterations, int block)
{
	//std::cout << key << std::endl;
	tpe * image = new tpe(key, iterations, block);
	std::cout << "created a tpe object" << std::endl;
	return image;
}

void encrypt(int * r, int * g, int * b, int width, int height, tpe * right)
{
	int size = sizeof(*r) / sizeof(r[0]);
	std::vector<int> red(r, r + size);
	std::vector<int> green(g, g + size);
	std::vector<int> blue(b, b + size);
	
	std::cout << "vectors created" << std::endl;

	right->encrypt(red, green, blue, width, height);
	
	for (int i: red) { std::cout << i << " "; }
	std::cout << std::endl;
}

void decrypt(int * r, int * g, int * b, int width, int height, tpe * right)
{
	int size = sizeof(*r) / sizeof(r[0]);
	std::vector<int> red(r, r + size);
	std::vector<int> green(g, g + size);
	std::vector<int> blue(b, b + size);
	
	right->decrypt(red, green, blue, width, height);
	
	for (int i: red) { std::cout << i << " "; }
	std::cout << std::endl;
}