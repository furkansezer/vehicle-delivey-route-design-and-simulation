
#include "Event.h"

bool Event::operator<(Event arg) const
{
	if (this->time < arg.time)
		return true;
	else return false;
}


Event::Event(int a, int b, Vehicle * p)
{
	type = a;
	time = b;
	car = p;
}

Event::~Event()
{
	car = nullptr;
}

void Event::ArrivaltoPlant(Heap *FEL,Vehicle** VehicleList)
{
	if (car->path.isDirect) {
		Event b(2, car->plant_Arrival_Time, car);
		car->path.vdc = car->path.plant;
		b.ArrivaltoVDC();
	}
	else {
		int time_elapsed = 0;
		int k = car->path.vdc->number; // from 0 to 43.
		car->path.plant->addPlant(k, car);
		if (car->path.plant->ShiporWait_PlanttoVDC(car->path.vdc, VehicleList)) {
			vector<Vehicle*>::iterator itr;
			double distance = dist(car->path.plant->locationx, car->path.plant->locationy, car->path.vdc->locationx, car->path.vdc->locationy);
			if (car->path.rail == 1)
				time_elapsed = distance / railspeed;
			else
				time_elapsed = distance / truckspeed;
			for (itr = car->path.plant->plantInventory[k].begin(); itr != car->path.plant->plantInventory[k].end(); itr++) {
				(*itr)->plant_Departure_Time = time;
				Event a(2, time + time_elapsed, *itr);
				FEL->push(a);
			}
			while (!car->path.plant->plantInventory[k].empty())
				car->path.plant->plantInventory[k].pop_back();
		}
	}
	
}

void Event::ArrivaltoVDC(Heap* FEL)
{
	VDC* p = car->path.vdc;
	car->vdc_Arrival_Time = time;
	p->totalnumofVehicles++;
	p->numofVehicles++;
	p->addVDC(car);
	if (p->capacity < p->numofVehicles) {
		p->overflowVehicles++;
		p->overflowVehiclesTimesDay++;
	}
	if (p->ShiporWait_VDCtoDealer()) {
		// omer bora hocamin yeri
	}
}
