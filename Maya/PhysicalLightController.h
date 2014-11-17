#pragma once
#include <string>

const std::string LIGHTS_NAMESPACE = "lights";

class PhysicalLightController
{
public:
					PhysicalLightController();
	virtual			~PhysicalLightController();

	void			moveLight(const unsigned int id, const double x, const double y, const double z);
	void			rotateLight(const unsigned int id, const double x, const double y, const double z);

private:
	void tester();
};
