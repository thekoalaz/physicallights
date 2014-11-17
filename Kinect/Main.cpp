#include <iostream>
#include <string>
#include <Windows.h>
#include "PhysicalLightClient.h"

#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

void circle(double & x, double & z, const double radius, const double angle)
{
	x = radius * cos(angle);
	z = radius * sin(angle);

	return;
}

void tester(PhysicalLightClient * client)
{
	double angle = 0;
	double radius = 300;
	int step = 100;
	double angle_increment = M_PI * 2 / step;
	double x, z;
	int id = 0;
	while (true)
	{
		while (true) {
			if (angle > M_PI * 2)
			{
				angle = 0;
				break;
			}
			angle += angle_increment;
			circle(x, z, radius, angle);
			client->translate(id, x, 180, z);
		}
		id = (id == 0) ? 1 : 0;
		Sleep(50);
	}

}
using namespace std;

int main(){
	PhysicalLightClient * client = PhysicalLightClient::Instance();
	// Testing
	tester(client);
	cout << "Press any key to exit";
	std::getchar();
}

