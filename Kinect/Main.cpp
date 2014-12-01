#include "stdafx.h"
#include "resource.h"

#include <iostream>
#include <string>

#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

#include "PhysicalLightClient.h"
#include "PhysicalLightKinect.h"

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

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	PhysicalLightClient * client = PhysicalLightClient::Instance();
    CInfraredBasics application;
    application.Run(hInstance, nCmdShow);
	// Testing Maya
	//tester(client);
	//cout << "Press any key to exit";
	std::getchar();
}
