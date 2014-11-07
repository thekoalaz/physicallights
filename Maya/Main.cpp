#include <maya/MSimple.h>
#include <maya/MIOStream.h>
DeclareSimpleCommand(helloWorld, "Autodesk", "2014");
MStatus helloWorld::doIt(const MArgList&)
{
	cout << "Hello World\n" << endl;
	return MS::kSuccess;
}
