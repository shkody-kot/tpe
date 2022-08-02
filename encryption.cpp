#include <iostream>
#include <vector>
#include <emscripten.h>
#include "tpe.h"

//extern "C"
//{
	void EMSCRIPTEN_KEEPALIVE decrypt(std::vector<int> &, std::vector<int> &, std::vector<int> &, int, int, tpe &);
	void EMSCRIPTEN_KEEPALIVE encrypt(std::vector<int> &, std::vector<int> &, std::vector<int> &, int, int, tpe &);
	tpe * EMSCRIPTEN_KEEPALIVE create(char *, int, int);
//}

int main()
{
	std::cout << "Worked" << std::endl;
	std::vector<int> color1 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> color2 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> color3 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	char test[] = "testkey";
	
	tpe * image = create(test, 10, 20);
	encrypt(color1, color2, color3, 10, 1, *image);
	decrypt(color1, color2, color3, 10, 1, *image);
	delete image;
	
	return 0;
}

tpe * create(char * key, int iterations, int block)
{
	tpe * image = new tpe(key, iterations, block);
	std::cout << "created a tpe object" << std::endl;
	return image;
}

void encrypt(std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, int width, int height, tpe &right)
{
	right.encrypt(red, green, blue, width, height);
}

void decrypt(std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, int width, int height, tpe &right)
{
	right.decrypt(red, green, blue, width, height);
}