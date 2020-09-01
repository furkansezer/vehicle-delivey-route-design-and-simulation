#pragma once

#include <iostream>
#include <cmath>
#include <queue>
using namespace std;

#define M_PI 3.14159265358979323846

#include "Vehicle.h"
#include "Event.h"
double dist(double x1, double y1, double x2, double y2)
{
	double latrad1 = x1, latrad2 = x2, longrad1 = y1, longrad2 = y2;
	if (latrad1 < 0)
		latrad1 = latrad1 + 360;
	if (latrad2 < 0)
		latrad2 = latrad2 + 360;
	if (longrad1 < 0)
		longrad1 = longrad1 + 360;
	if (longrad2 < 0)
		longrad2 = longrad2 + 360;

	latrad1 = M_PI * latrad1 / 180;
	latrad2 = M_PI * latrad2 / 180;
	longrad1 = M_PI * longrad1 / 180;
	longrad2 = M_PI * longrad2 / 180;

	return 1.2 * 2 * 3959 * asin(sqrt(pow(sin((latrad1 - latrad2) / 2), 2) + cos(latrad1)*cos(latrad2)*pow(sin((longrad1 - longrad2) / 2), 2)));
}

#define overflowShuttleCostperVehicle 30
#define overflowDailyCostperVehicle 4
#define handlingCostperVehicle 50

class VDC
{
public:
	int number;
	string name;
	int capacity;
	double locationx;
	double locationy;
	int railAvailability;
	double vdcCost;
	int numofVehicles;
	int totalnumofVehicles;
	int overflowVehicles;
	int overflowVehiclesTimesDay;
	vector<Vehicle*> vdcInventory;
	//priority_queue<int> nextArrivalTimeHeap;

	void addVDC(Vehicle* car);
	void Calculate();
	bool ShiporWait_VDCtoDealer(Heap* FEL,int clock);
	VDC(int num,string name, int a,double x,double y, int RailAvail);
	~VDC();
};

class Plant : public VDC {
public:
	vector<Vehicle*> plantInventory[44] ; // 44 : number of vdcs, 20 : max capacity of rail. 0th is the dealers inventory
	void addPlant(int x,Vehicle* car);
	bool ShiporWait_PlanttoVDC(VDC* v, Vehicle** VehicleList);
};
