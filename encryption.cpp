#include <iostream>
#include <string>
//#include <emscripten.h>
#include "tpe.h"
#include <thread>


uint8_t * /*EMSCRIPTEN_KEEPALIVE*/ decrypt(uint8_t *, uint8_t * , uint8_t *, int, int, tpe *);
uint8_t * /*EMSCRIPTEN_KEEPALIVE*/ encrypt(uint8_t *, uint8_t * , uint8_t *, int, int, tpe *);
tpe * /*EMSCRIPTEN_KEEPALIVE*/ create(char *, int, int);
void menu();
void options(tpe *, std::string &);


int main(int argc, char **argv, char **envp)
{
	std::cout << "Worked" << std::endl;
	std::string input;
	bool done = false;
	tpe * object = nullptr;
	
	menu();
	std::cin >> input;
	
	while (done != true)
	{
		if (input == "quit" || input == "exit")
		{
			done = true;
			break;
		}
		options(object, input);
		input.clear();
		std::cout << std::endl;
		menu();
		std::cin.ignore(10000, '\n');
		std::cin >> input;
	}
	
	return 0;
}

tpe * create(char * key, int iterations, int block)
{
	tpe * image = new tpe(key, iterations, block);
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

void menu()
{
	std::string menu = "CREATE -- ENCRYPT -- DECRYPT -- EXIT";
	std::cout << "Choose an option (all lowercase): " << std::endl;
	std::cout << menu << std::endl;
}

void options(tpe * object, std::string &input)
{
	uint8_t * d = nullptr;
	uint8_t * s;
	uint8_t * p;
		
	uint8_t sub[375];
	uint8_t perm[1000];
	
	for (int i = 0; i < 375; i++) { sub[i] = std::rand() % 256;  }
	for (int i = 0; i < 1000; i++) { perm[i] = std::rand() % 256; }
	
	s = &sub[0];
	p = &perm[0];
	
	if (input == "create")
	{
		int iterations = 10;
		int block = 3;
		char test[] = "testkey";
		object = create(test, iterations, block);
		if (object != nullptr) { std::cout << "tpe object created"; }
		else { std::cout << "error"; }
	}
	else if (input == "encrypt")
	{
		uint8_t data[400];
		
		for (int i = 0; i < 400; i++) { data[i] = std::rand() % 256;  }
		
		d = &data[0];
		d = encrypt(d, s, p, 10, 10, object);
		std::cout << "encryption complete";
	}
	else if (input == "decrypt" && d != nullptr)
	{
		d = decrypt(d, s, p, 10, 10, object);
		std::cout << "decryption complete";
	}
	else if (input == "decrypt" && d == nullptr) { std::cout << "cannot decrypt"; }
	else if (input != "quit" && input != "exit") { std::cout << "invalid input"; }
	input.clear();
}