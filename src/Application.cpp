#include <iostream>
#include "SRT/srt_parser.h"

int main()
{
	SRTParser* sp = new SRTParser();
	sp->RenderWindow();
	delete sp;

	//std::cout << "parser" << std::endl;
	std::cout << std::endl;
	system("pause");
}