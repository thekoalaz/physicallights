#include <iostream>

#include <iostream>
#include <string>

#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <opencv2/opencv.hpp>

#include "PhysicalLightClient.h"
#include "PhysicalLightKinect.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    PhysicalLightKinect client;
    client.Run();
}
