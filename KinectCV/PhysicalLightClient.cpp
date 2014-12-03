#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <stdlib.h>

extern "C" {
#include <mocapserver.h>
#include <mocaptcp.h>
}

#include <iostream>


#include "PhysicalLightClient.h"

using namespace std;
// Singleton
PhysicalLightClient* PhysicalLightClient::instance = nullptr;

PhysicalLightClient* PhysicalLightClient::Instance()
{
	if (instance == nullptr)
		instance = new PhysicalLightClient;
	return instance;
}

PhysicalLightClient::PhysicalLightClient()
{
}
PhysicalLightClient::~PhysicalLightClient()
{

}

// Static values
const string PhysicalLightClient::command = "setAttr";
const string PhysicalLightClient::lights_namespace = "lights:";

PhysicalLightClient::LightMap PhysicalLightClient::createLightMap()
{
	LightMap lights;
	lights[0] = "aiAreaLight_key";
	lights[1] = "aiAreaLight_fill";

	return lights;
}

const PhysicalLightClient::LightMap PhysicalLightClient::lights(PhysicalLightClient::createLightMap());

// Methods
void PhysicalLightClient::translate(const int id, const double x, const double y, const double z)
{
	if (!connected)
	{
		connect();
	}

	string attr_name = "translate";
	string light_name = lights.at(id);

	string commandX = command + " " + lights_namespace + light_name + "." + attr_name + "X " + to_string(x);
	send_command(commandX);
	string commandY = command + " " + lights_namespace + light_name + "." + attr_name + "Y " + to_string(y);
	send_command(commandY);
	string commandZ = command + " " + lights_namespace + light_name + "." + attr_name + "Z " + to_string(z);
	send_command(commandZ);

	return;
}

void PhysicalLightClient::rotate(const int id, const double x, const double y, const double z)
{
	if (!connected)
	{
		connect();
	}

	string attr_name = "rotate";
	string light_name = lights.at(id);

	string commandX = command + " " + lights_namespace + light_name + "." + attr_name + "X " + to_string(x);
	send_command(commandX);
	string commandY = command + " " + lights_namespace + light_name + "." + attr_name + "Y " + to_string(y);
	send_command(commandY);
	string commandZ = command + " " + lights_namespace + light_name + "." + attr_name + "Z " + to_string(z);
	send_command(commandZ);

	return;
}

int PhysicalLightClient::connect()
{
	cerr << "Contacting server " << server_name << endl;
	char *cstr = new char[server_name.length() + 1];
	strcpy(cstr, server_name.c_str());
	// do stuff
	connection = CapTcpOpen(cstr);
	delete [] cstr;
	if ( connection < 0 ) {
		cerr << "Couldn't connect to server " << server_name << endl;
		exit(-1);
	} else {
		cerr << "Connected to server " << server_name << endl;
	}

	connected = true;
	return 0;
}

int PhysicalLightClient::disconnect()
{
	cerr << "Closing connection" << endl;

	closesocket(connection);
	connected = false;
	return 0;
}

int PhysicalLightClient::send_command(std::string command)
{
	cerr << "Sending command: " + command << endl;
	send(connection,command.c_str(),command.length(),0);

	cerr << "awaiting reply..." << endl;

	char reply[5000];
	int red = recv(connection,reply,4096,0);

	cerr << "Recieved " << red << "bytes" << endl;

	if ( red > 0)
	{
		cerr << reply << endl;
	}
	else {
		cerr << "READ FAILED" << endl;
		return 1;
	}
	return 0;
}