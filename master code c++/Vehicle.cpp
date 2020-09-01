
#include "Vehicle.h"


Vehicle::Vehicle(int inname, Plant* inplant, int indealer, VDC* invdc, int inplantarrivaltime, int intruck, int inrail, int inisDirect)
{
	name = inname;
	path = StaticPath(inplant, indealer, invdc, intruck, inrail, inisDirect);
	plant_Arrival_Time = inplantarrivaltime;
	isAV = 0; // for now.
	int plant_Departure_Time = 0;
	int vdc_Arrival_Time = 0;
	int vdc_Departure_Time = 0;
	int dealer_Arrival_Time = 0;
}

Vehicle::Vehicle()
{
}

Vehicle::~Vehicle()
{
}