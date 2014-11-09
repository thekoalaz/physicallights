#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPxClientDeviceNode.h>

#pragma once
class PhysicalLightClient : MPxClientDeviceNode {
public:

	/* Inherited Functions */
						PhysicalLightClient();
	virtual				~PhysicalLightClient();

	virtual void		PostConstructor();
	virtual MStatus     compute(const MPlug& plug, MDataBlock& data);
	virtual void        threadHandler(const char* serverName, const char* deviceName);
	virtual void        threadShutdownHandler();
	
	static void*		creator();
	static MStatus		initialize();

	/* New Functions */
	void				openConnection(const char* server, int port);
	void				closeConnection();

	int					send();
	int					recv();

private:
};
