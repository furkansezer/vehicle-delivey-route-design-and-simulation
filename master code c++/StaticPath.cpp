#include "StaticPath.h"


StaticPath::StaticPath(Plant* inPlant, Dealer* inDealer ,VDC* inVDC, int inTruck, int inRail, int inIsDirect)
{
	plant = inPlant;
	dealer = inDealer;
	vdc = inVDC;
	truck = inTruck;
	rail = inRail;
	isDirect = inIsDirect;
}

StaticPath::StaticPath() 
{
}

StaticPath::~StaticPath()
{
}
