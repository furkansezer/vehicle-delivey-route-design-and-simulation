#pragma once
#include "Vehicle.h"
#include <queue>

const double railspeed = 1.0 / 6.0;
const double truckspeed = 0.5;

class comparisonFunc { public: bool operator()(Event* p1, Event* p2) { return !(p1->time < p2->time); } };
typedef priority_queue<Event, vector<Event>, comparisonFunc > Heap;

class Event
{
public:
	int type;  // 1 arrivaltoplant, 2 arrivaltovdc
	int time;
	Vehicle* car;
	bool operator<(Event arg) const;
	Event(int a,int b,Vehicle* p);
	~Event();
	void ArrivaltoPlant(Heap* FEL, Vehicle** VehicleList);
	void ArrivaltoVDC(Heap* FEL);
};

