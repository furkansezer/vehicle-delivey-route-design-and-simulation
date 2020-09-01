// Informs Project main.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
using namespace std;

#include "Vehicle.h"
#include "Event.h"
#include "VDC.h"

int CLOCK;

int numofVDCs = 44;
int numofPlants = 4;
int totalnumofVehicles;
int totalnumofStaticPaths; // total number of static paths for plant-dealers

Plant* PlantMapping(string l, Plant** plants) {
	for (int i = 0; i < numofPlants; i++) {
		if (plants[i]->name == l)
			return plants[i];
	}
}

VDC* VDCMapping(string l, VDC** vdcs) {
	for (int i = 0; i < numofVDCs; i++) {
		if (vdcs[i]->name == l)
			return vdcs[i];
	}
}


int main() {
	CLOCK = 0;
	Plant** plants = new Plant*[numofPlants];
	VDC** vdcs = new VDC*[numofVDCs];
	ifstream plantinfo("plant_info.txt");
	ifstream vdcinfo("vdc_info.txt");
	string l;
	int cap, railavail;
	double locx, locy;
	for (int i = 0; i < numofPlants; i++) {
		plantinfo >> l;
		plantinfo >> cap;
		plantinfo >> locx;
		plantinfo >> locy;
		plantinfo >> railavail;
		plants[i] = new Plant(i,l, cap, locx, locy, 1);
	}
	for (int i = 0; i < numofVDCs; i++) {
		vdcinfo >> l;
		vdcinfo >> cap;
		vdcinfo >> locx;
		vdcinfo >> locy;
		vdcinfo >> railavail;
		vdcs[i] = new VDC(i,l, cap, locx, locy, railavail);
	}
	

	ifstream vsr("cpp_input_vsr.txt"); // vehicle shipment requirement: VINID Plant Dealer VDC Plant_Arrival_Time Truck Rail IsDirect. (Plant_Arrival_Time in minutes from 1 Jan 2015.)
	ofstream outfile("output.txt");

	Vehicle** VehicleList = new Vehicle*[totalnumofVehicles];

	int name, dealer, plantarrivaltime, truck, rail, isDirect;
	string plant,vdc;
	for (int i = 0; i < totalnumofVehicles; i++) {
		vsr >> name;
		vsr >> plant;
		vsr >> dealer;
		vsr >> vdc;
		vsr >> plantarrivaltime;
		vsr >> truck;
		vsr >> rail;
		vsr >> isDirect;
		VehicleList[i] = new Vehicle(name, PlantMapping(plant,plants), dealer, VDCMapping(vdc,vdcs), plantarrivaltime, truck, rail, isDirect);
	}
	
	typedef priority_queue<Event, vector<Event>, comparisonFunc > Heap;
	Heap FEL;
	

	// push FEL

	int day = 0;
	int newday = 0;

	while (!FEL.empty()) {
		newday = CLOCK / 1440;
		if (newday > day) {
			day++;
			for (int i = 0; i < numofVDCs; i++) {
				int k = vdcs[i]->numofVehicles - vdcs[i]->capacity;
				if(k>0)
					vdcs[i]->overflowVehiclesTimesDay += k;
			}
		}
		Event p = FEL.top();
		CLOCK = p.time;
		switch (p.type) {
		case 1: p.ArrivaltoPlant(); break;
		case 2: p.ArrivaltoVDC(); break;
		}
		FEL.pop();
	}


	delete[] plants;
	delete[] vdcs;
	delete[] VehicleList;
	return 0;
}