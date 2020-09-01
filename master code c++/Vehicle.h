#pragma once

#include <iostream> // string i tanimasi icin gerek
#include "StaticPath.h"
using namespace std;

class Vehicle
{
public:
	int name;
	int isAV;
	StaticPath path;
	int plant_Arrival_Time;
	int plant_Departure_Time;
	int vdc_Arrival_Time;
	int vdc_Departure_Time;
	int dealer_Arrival_Time;

	Vehicle(int inname,Plant* inplant, int indealer, VDC* invdc, int inplantarrivaltime, int intruck, int inrail, int inisDirect);
	Vehicle();
	~Vehicle();
};