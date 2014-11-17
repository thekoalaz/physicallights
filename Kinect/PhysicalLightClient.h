#include <string>
#include <map>
#pragma once

class PhysicalLightClient {
public:
	static PhysicalLightClient* Instance();
	void translate(const int id, const double x, const double y, const double z);
	void rotate(const int id, const double x, const double y, const double z);

private:
	PhysicalLightClient();
	~PhysicalLightClient();
	int	connect();
	int	disconnect();
	int	send_command(std::string command);

public:


private:
	int connection;
	bool verbose = false;
	bool connected = false;

	static PhysicalLightClient* instance;
	std::string	server_name = "mayaCommand";
	static const std::string command;
	static const  std::string lights_namespace;

	typedef std::map<int, std::string> LightMap;
	static LightMap createLightMap();
	static const LightMap lights;
};
