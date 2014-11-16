#include <iostream>
#include <maya/MArgList.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include "PhysicalLightServer.h"

class startServer : public MPxCommand
{
public:
	MStatus doIt(const MArgList& args);
	static void* creator();
};

MStatus startServer::doIt(const MArgList& args) {
	cout << "Hello " << args.asString(0).asChar() << endl;
	return MS::kSuccess;
}

void* startServer::creator() {
	return new startServer;
}

MStatus initializePlugin(MObject obj) {
	MFnPlugin plugin(obj, "Kevin Lim", "1.0", "Any");
	plugin.registerCommand("startServer", startServer::creator);
	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);
	plugin.deregisterCommand("hello");
	return MS::kSuccess;
}
