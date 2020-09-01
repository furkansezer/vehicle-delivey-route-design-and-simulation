#include "gurobi_c++.h"
#include "Vehicle.h"
#include "VDC.h"
#include "StaticPath.h"
#include "demand.h"
#include "math.h"
#include "sstream"
#include "Dealer.h"

using namespace std;
class demand;

void Path_determination(vector <Vehicle*> V, VDC** vdcs) {

	vector<Vehicle*> vehicles_of_today;
	vehicles_of_today = V; 
	vector<Plant*> Plants_of_today;
	vector <demand> demand_of_today;
	vector< Dealer*> dealers_of_today;

	int i = 0; int j = 0; int k = 0; int m = 0,int f=0; // m -> 0=rail, 1=truck 2=AV, f-> 0=AV and 1=Truck

	vector<Vehicle*>::iterator itr_v;
	vector<Plant*>::iterator itr_p;
	vector<demand>::iterator itr_d;
	vector<Dealer*>::iterator itr_dea;

	double speed[3] = { 10,30,30 }; //Rail, Truck, AV
	int fixed_transportation_cost[3] = { 2000,200,0};
	int variable_transportation_cost[3] = { 3,4,0.55};
	int mode_load_capacity[3] = { 20,10,1 };

	for (itr_v = vehicles_of_today.begin(); itr_v != vehicles_of_today.end(); itr_v++) {
		for (itr_p = Plants_of_today.begin(); itr_p != Plants_of_today.end(); itr_p++) {

			if ((*itr_v)->path.plant->number == (*itr_p)->number) {
				continue;
			}
			else
			{
				i++;
				if (i == Plants_of_today.size()) {
					Plants_of_today.push_back((*itr_v)->path.plant);
				}
			}
		}
		i = 0;
	}

	for (itr_v = vehicles_of_today.begin(); itr_v != vehicles_of_today.end(); itr_v++)
	{
		for (itr_d = demand_of_today.begin(); itr_d != demand_of_today.end(); itr_d++)
		{
			if ((*itr_v)->path.dealer == itr_d->dealer && (*itr_v)->path.plant == itr_d->plant)
			{
				itr_d->inventory.push_back((*itr_v));
			}
		}
	}

	for (itr_v = vehicles_of_today.begin(); itr_v != vehicles_of_today.end(); itr_v++) {
		for (itr_dea = dealers_of_today.begin(); itr_dea != dealers_of_today.end(); itr_p++) {
			if ((*itr_v)->path.dealer == (*itr_dea)) {
				continue;
			}
			else
			{
				i++;
				if (i == dealers_of_today.size()) {
					dealers_of_today.push_back((*itr_v)->path.dealer);
				}
			}
		}
		i = 0;
	}

	try
	{
		
		GRBModel Path_determination = GRBModel(env); 
		Path_determination.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
		Path_determination.set(GRB_IntParam_OutputFlag, false);

		GRBVar X[4][44][1400][3][2]; // Indirect shipment decision variable: Plant, VDC, Dealer, Transportation mode for first leg(0=rail, 1=truck 2=AV), Transportation mode for second leg( 0=AV and 1=Truck)
		GRBVar Y[4][1400][2]; //Direct shipment decision variable: Plant, Dealer, Transportation mode(0=AV and 1=Truck)



		for (itr_d = demand_of_today.begin(); itr_d != demand_of_today.end(); itr_d++)
		{
			for (j = 0; j < 44; j++)
			{
				for (m = 0; m < 3; m++) {
					for (f=0;f< 2; f++)
					{   
						if (f == 0) {
							X[itr_d->plant->number][j][itr_d->dealer->number][m][0] = Path_determination.addVar(0.0, 1.0, 40*itr_d->inventory.size() +(fixed_transportation_cost[m]+(variable_transportation_cost[m] + 10/(24*speed[m]))*dist(itr_d->plant->locationx,itr_d->plant->locationy,itr_d->dealer->locx,itr_d->dealer->locy))*itr_d->inventory.size()/mode_load_capacity[m] + itr_d->inventory.size()*dist(itr_d->dealer->locx,itr_d->dealer->locy,vdcs[j]->locationx,vdcs[j]->locationy)*(0.55+10/7200), GRB_BINARY);
						}
						
						else if (f == 1) { 
							X[itr_d->plant->number][j][itr_d->dealer->number][m][1] = Path_determination.addVar(0.0, 1.0, 40 * itr_d->inventory.size() + ceil(itr_d->inventory.size() / 10)*(dist(itr_d->dealer->locx, itr_d->dealer->locy, vdcs[j]->locationx, vdcs[j]->locationy)*(4 + 10 / (24 * 30)) + 200) + (fixed_transportation_cost[m]+dist(itr_d->plant->locationx,itr_d->plant->locationy,vdcs[j]->locationx,vdcs[j]->locationy)*(variable_transportation_cost[m]+(10/(24*speed[m]))))*itr_d->inventory.size()/mode_load_capacity[m], GRB_BINARY);
					    }
						
					}
				}
			}
		}

		for (itr_d = demand_of_today.begin(); itr_d != demand_of_today.end(); itr_d++)
		{
			for ( f = 0; f < 2; f++)
			{
				if (f == 0) {
					Y[itr_d->plant->number][itr_d->dealer->number][0] = Path_determination.addVar(0.0, 1.0, dist(itr_d->plant->locationx,itr_d->plant->locationy,itr_d->dealer->locx,itr_d->dealer->locy)*itr_d->inventory.size()*(0.55+ (10/7200))  , GRB_BINARY);
			    }
				else if (f==1)
				{
					Y[itr_d->plant->number][itr_d->dealer->number][1] = Path_determination.addVar(0.0, 1.0, ceil(itr_d->inventory.size() / 10)*((4 + 10 / (24 * 30))*dist(itr_d->plant->locationx, itr_d->plant->locationy, itr_d->dealer->locx, itr_d->dealer->locy) + 200), GRB_BINARY);
				}
			}
		}


		for (itr_p = Plants_of_today.begin(); itr_p != Plants_of_today.end(); itr_p++) {
			for (itr_dea = dealers_of_today.begin(); itr_dea != dealers_of_today.end(); itr_dea++)
			{
				GRBLinExpr Assignment = 0.0;
				for ( f = 0; f < 2; f++)
				{
					Assignment += Y[(*itr_p)->number][(*itr_dea)->number][f];

					for (j = 0; j < 44; j++) {
						for (m = 0; m < 2; m++) {
							Assignment += X[(*itr_p)->number][j][(*itr_dea)->number][m][f];

						}
					}
				}
				
				Path_determination.addConstr(Assignment, GRB_EQUAL, 1.0); //Assignment
			}
		}
		for (j = 0; j < 44; j++)
		{
			GRBLinExpr Capacity = 0.0;
			for (itr_d = demand_of_today.begin(); itr_d != demand_of_today.end(); itr_d++)
			{
				for (m = 0; m < 2; m++)
				{
					for ( f = 0; f < 2; f++)
					{
						Capacity += (X[itr_d->plant->number][j][itr_d->dealer->number][m][f] * itr_d->inventory.size());
					}
				}
			}
			Path_determination.addConstr(Capacity, GRB_LESS_EQUAL, vdcs[j]->capacity - vdcs[j]->vdcInventory.size()); //Capacity
		}

		for (itr_d = demand_of_today.begin(); itr_d != demand_of_today.end(); itr_d++)
		{
			for ( f = 0; f < 2; f++)
			{
				Path_determination.addConstr(X[itr_d->plant->number][1][itr_d->dealer->number][0][f], GRB_EQUAL, 0.0); //Rail not available 1
				Path_determination.addConstr(X[itr_d->plant->number][31][itr_d->dealer->number][0][f], GRB_EQUAL, 0.0); //Rail not available 31
				Path_determination.addConstr(X[itr_d->plant->number][41][itr_d->dealer->number][0][f], GRB_EQUAL, 0.0); //Rail not available 41
			}

		}

		for ( itr_d = demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++)
		{
			if (dist(itr_d->plant->locationx,itr_d->plant->locationy,itr_d->dealer->locx,itr_d->dealer->locy)<=(400/3))
			{
				Path_determination.addConstr(Y[itr_d->plant->number][itr_d->dealer->number][0], GRB_EQUAL, 1.0); //Direct Assignment
			}
		}

		for ( itr_d = demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++)
		{
			for ( j = 0; j < 44; j++)
			{
				Path_determination.addConstr(X[itr_d->plant->number][j][itr_d->dealer->number][2][0], GRB_EQUAL, 0.0); //Prevention_of_AV-AV
			}
		}

		/*Limiting_distance_travelled_first_leg(i,j,k,f)$(dist_vdc_to_vdc(j,i) ge 300)..X(i,j,k,"AV",f)=e=0 ;
          Limiting_distance_travelled_second_leg(i,j,k,m)$(dist_dealer_to_vdc(j,k) ge 300)..X(i,j,k,m,"AV")=e=0 ;
		*/

		for ( itr_d = demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++)
		{
			for ( j = 0; j < 44; j++)
			{
				for ( f = 0; f < 2; f++)
				{
					if (dist(itr_d->plant->locationx, itr_d->plant->locationy,vdcs[j]->locationx,vdcs[j]->locationy)>300)
					{
						Path_determination.addConstr(X[itr_d->plant->number][j][itr_d->dealer->number][2][f], GRB_EQUAL, 0.0);
					}
				}
			}
		}

		for (itr_d = demand_of_today.begin(); itr_d != demand_of_today.end(); itr_d++)
		{
			for (j = 0; j < 44; j++)
			{
				for (m = 0; m < 2; m++)
				{
					if (dist(itr_d->dealer->locationx, itr_d->dealer->locationy, vdcs[j]->locationx, vdcs[j]->locationy)>300)
					{
						Path_determination.addConstr(X[itr_d->plant->number][j][itr_d->dealer->number][m][0], GRB_EQUAL, 0.0);
					}
				}
			}
		}



		Path_determination.optimize();
		/*
		for (itr_v = vehicles_of_today.begin(); itr_v != vehicles_of_today.end(); itr_v++)
		{
			if (Y[(*itr_v)->path.plant->number][(*itr_v)->path.dealer->number][].get(GRB_DoubleAttr_X) == 1) // path AVfisrtLeg, AV second leg needed. 
			{
				(*itr_v)->path.vdc = nullptr;
				(*itr_v)->path.truck = 1;
				(*itr_v)->path.rail = 0;
			}
		}

		for (itr_v = vehicles_of_today.begin(); itr_v != vehicles_of_today.end(); itr_v++)
		{
			for (j = 0; j < 44; j++)
			{
				for (m = 0; m < 2; m++)
				{
					if (X[(*itr_v)->path.plant->number][j][(*itr_v)->path.dealer->number][m][].get(GRB_DoubleAttr_X) == 1) {
						(*itr_v)->path.vdc->number = j;
						if (m == 0) {
							(*itr_v)->path.rail = 1;
							(*itr_v)->path.truck = 0;
						}
						else
						{
							(*itr_v)->path.rail = 0;
							(*itr_v)->path.truck = 1;
						}
					}
				}
			}
		}
		*/
	}
	
	catch (GRBException e) {
		cout << "Error number: " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Error during optimization" << endl;
	}
}


