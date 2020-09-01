#include "VDC.h"


void VDC::addVDC(Vehicle * car)
{
	vdcInventory.push_back(car);
}

void VDC::Calculate()
{
	vdcCost = totalnumofVehicles * handlingCostperVehicle + overflowVehiclesTimesDay * overflowDailyCostperVehicle + overflowVehicles * overflowShuttleCostperVehicle;
}

bool VDC::ShiporWait_VDCtoDealer(Heap* FEL,int clock)
{
	if (vdcInventory.size() >= 10)
		return true;
	if (vdcInventory.size() < 5)
		return false;
	if (FEL->top()->time == clock && FEL->top()->type==2 && FEL->top()->car->path.vdc == this)
		return false;
	else {
		// tsp ye gore implement edilecek. plant to vdc nin aynisi olacak.
		// cem demandli k means yapcak.
	}
}

VDC::VDC(int num,string n, int a, double x, double y, int RailAvail)
{
	number = num;
	name = n;
	capacity = a;
	locationx = x;
	locationy = y;
	railAvailability = RailAvail;
	vdcCost = 0;
	numofVehicles = 0;
	totalnumofVehicles = 0;
	overflowVehicles = 0;
	overflowVehiclesTimesDay = 0;
}

VDC::~VDC()
{
}

void Plant::addPlant(int x,Vehicle* car)
{
	plantInventory[x].push_back(car);
}

bool Plant::ShiporWait_PlanttoVDC(VDC* v,Vehicle** VehicleList) // if true, ship.
{
	if (plantInventory[v->number].empty())
		return false;
	else {
		vector<Vehicle*>::iterator itr;
		int i = 0;
		for (itr = plantInventory[v->number].begin(); itr != plantInventory[v->number].end(); itr++) {
			i++;
		}
		plantInventory[v->number].size();
		itr--;
		int clock = (*itr)->plant_Arrival_Time;
		int FC, VC;
		if ((*itr)->path.truck == 1) {
			FC = 200;
			VC = 4;
		}
		else {
			FC = 2000;
			VC = 3;
		}
		double breakevenMinute = 24 * 6 * (FC + VC * dist(locationx, locationy, v->locationx, v->locationy)) / (i*(i + 1));
		int nextVehicleTime = INT_MAX;
		for(int vehicleorder = (*itr)->name + 1; VehicleList[vehicleorder]->plant_Arrival_Time < clock + 4320; vehicleorder++) {
			if (VehicleList[vehicleorder]->path.vdc->number == v->number && VehicleList[vehicleorder]->path.plant->name == name && VehicleList[vehicleorder]->plant_Arrival_Time < nextVehicleTime)
				nextVehicleTime = VehicleList[vehicleorder]->plant_Arrival_Time;
		}
		if (nextVehicleTime < breakevenMinute)
			return false;
		else
			return true;
		
	}
}
