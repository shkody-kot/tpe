
main:	tpe.h tpe.cpp aes_rnd.h aes_rnd.cpp encryption.cpp
		g++ -o main -g -Wall -std=c++11 -pthread -lpthread *.cpp

clean:	main
		rm main
