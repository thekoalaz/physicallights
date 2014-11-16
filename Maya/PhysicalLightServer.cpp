#include "PhysicalLightServer.h"

PhysicalLightClient::PhysicalLightClient()
{ }
PhysicalLightClient::~PhysicalLightClient()
{ }


void PhysicalLightClient::PostConstructor()
{
	return;
}
MStatus PhysicalLightClient::compute(const MPlug& plug, MDataBlock& data)
{
	return 0;
}

void PhysicalLightClient::threadHandler(const char* serverName, const char* deviceName)
{
	return;
}

void PhysicalLightClient::threadShutdownHandler()
{
	return;
}


void* PhysicalLightClient::creator()
{
	return;
}

MStatus PhysicalLightClient::initialize()
{
	return 0;
}


/* New Functions */
void PhysicalLightClient::openConnection(const char* server, int port)
{
	return;
}

void PhysicalLightClient::closeConnection()
{
	return;
}


int PhysicalLightClient::send()
{
	return 0;
}

int	PhysicalLightClient::recv()
{
	return 0;
}

