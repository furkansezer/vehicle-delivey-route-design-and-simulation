#pragma once
#include <iostream>
#include "VDC.h"
#include "Dealer.h"
using namespace std;
class StaticPath
{
public:
	Plant* plant;
	Dealer* dealer;
	VDC* vdc;
	int truck;
	int rail;
	int isDirect;

	StaticPath(Plant* inPlant, Dealer* inDealer, VDC* inVDC, int inTruck, int inRail, int inIsDirect);
	StaticPath();
	~StaticPath();
};

