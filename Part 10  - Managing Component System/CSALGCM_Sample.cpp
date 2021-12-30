#include <iostream>
#include <vector>
#include <string>

void number_one(std::vector<std::string> input)
{
	//your algorithm

	for(int i = 1; i < 5; i++)
	{
		for (int j = 1; j < 5; j++)
		{
			std::cout << "My algorithm runs!" << (i * j) << std::endl;
		}
	}
}