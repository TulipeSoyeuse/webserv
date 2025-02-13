#include "includes.hpp"

extern char **environ;

int main(int argc, char **argv, char **envp)
{
	std::cout << setenv("VARIABLE_TEST", "TEST_VALUE", 1) << "\n";
	for (int i = 0; environ[i]; i++)
	{
		std::cout << environ[i] << "\n";
	}
}
