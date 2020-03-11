#include <iostream>
#include "icarus3D/icarus3D.h"

using namespace std;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	icarus3D *icarus = icarus3D::Instance();

	icarus->init();

	return 0;
}