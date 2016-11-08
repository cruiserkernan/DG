#include <iostream>
int main()
{
	std::cout << "alternative one: "
		<< sizeof(float) * 12 + sizeof(unsigned short) * 60
		<< std::endl;
	std::cout << "alternative two: "
		<< sizeof(float) * 60
		<< std::endl;

	system("pause");
	return 0;
}