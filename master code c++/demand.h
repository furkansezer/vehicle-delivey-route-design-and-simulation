#pragma once
#include "Vehicle.h"
#include "Dealer.h"
class demand
{
public:
	vector<Vehicle*> inventory;
	Dealer* dealer;
	Plant* plant;
	demand();
	~demand();
};

