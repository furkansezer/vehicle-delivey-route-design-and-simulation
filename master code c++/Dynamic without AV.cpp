#include "gurobi_c++.h"
#include "Vehicle.h"
#include "VDC.h"
#include "StaticPath.h"
#include "demand.h"
#include "math.h"
#include "sstream"
#include"Dealer.h"

using namespace std;

string itos(int i) { stringstream s; s << i; return s.str(); }

void Path_determination (vector <Vehicle*> V){

	vector<Vehicle*> vehicles_of_today; 
	vehicles_of_today = V;
	vector<Plant*> Plants_of_today; 
	vector <demand> demand_of_today;
	vector< Dealer*> dealers_of_today;
	
	int i = 0; int j = 0; int k = 0; int m =0;

	vector<Vehicle*>::iterator itr_v;
	vector<Plant*>::iterator itr_p;
	vector<demand>::iterator itr_d;
	vector<Dealer*>::iterator itr_dea;

	double speed[2] = {10,30}; //Rail, Truck
	int fixed_transportation_cost[2] = {2000,200};
	int variable_transportation_cost[2] = {3,4};
	int mode_load_capacity[2] = { 20,10 };

	for (itr_v = vehicles_of_today.begin(); itr_v != vehicles_of_today.end(); itr_v++) {
		for (itr_p = Plants_of_today.begin(); itr_p != Plants_of_today.end(); itr_p++){

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

	for (itr_v = vehicles_of_today.begin(); itr_v!=vehicles_of_today.end(); itr_v++)
	{
		for ( itr_d = demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++)
		{
			if ((*itr_v)->path.dealer==itr_d->dealer&& (*itr_v)->path.plant == itr_d->plant )
			{
				itr_d->inventory.push_back((*itr_v));
			}
		}
		
	}

	for (itr_v = vehicles_of_today.begin(); itr_v != vehicles_of_today.end(); itr_v++) {
		for (itr_dea = dealers_of_today.begin(); itr_dea != dealers_of_today.end(); itr_p++) {
			if ((*itr_v)->path.dealer ==(*itr_dea)) {
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
		GRBEnv env = GRBEnv();
		GRBModel Path_determination = GRBModel(env); //default sense of optimization is minimize. Therefore no need to set a sense of optimization.
		Path_determination.set(GRB_IntAttr_ModelSense,GRB_MINIMIZE);

		GRBVar X [4][44][1400][2];
		GRBVar Y [4][1400];
		GRBVar L [4][44][2];
		GRBVar S [4][44][2];
		//Should take all VDCs other than plants, plants and dealers which occur in these 3 day
		
		for (itr_d = demand_of_today.begin(); itr_d != demand_of_today.end(); itr_d++)
		{
			for (j = 0; j < 44; j++)
			{
				for (m = 0; m < 2; m++) {

					X[itr_d->plant->number][j][itr_d->dealer->number][m] = Path_determination.addVar(0.0, 1.0, 50 * itr_d->inventory.size() + ceil(itr_d->inventory.size() / 10)*(dist(itr_d->dealer->locx, itr_d->dealer->locy, vdcs[j]->locationx, vdcs[j]->locationy)*(4 + 10 / (24 * 30)) + 200), GRB_BINARY, "X" + itos(itr_d->plant) + "j" + itos(itr_d->dealer) + "m");
				}
			}
		}

		for ( itr_d = demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++)
		{
		  Y[itr_d->plant->number][itr_d->dealer->number]=Path_determination.addVar(0.0, 1.0, ceil(itr_d->inventory.size()/10)*((4+10/(24*30))*dist(itr_d->plant->locationx,itr_d->plant->locationy,itr_d->dealer->locx,itr_d->dealer->locy)+200), GRB_BINARY, "Y"+itos(itr_d->plant->number)+itos(itr_d->dealer->number));
		}

		for (itr_p=Plants_of_today.begin(); itr_p!= Plants_of_today.end(); itr_p++)
		{
			for (j = 0; j < 44; j++)
			{
				for (m = 0; m < 2; m++) {
					L[(*itr_p)->number][j][m] = Path_determination.addVar(0.0, 1 + vehicles_of_today.size() / 10, dist((*itr_p)->locationx, (*itr_p)->locationy, vdcs[j]->locationx, vdcs[j]->locationy)*(variable_transportation_cost[m] + 10 / 24 * speed[m]) + fixed_transportation_cost[m], GRB_INTEGER, "L" + itos((*itr_p)->number) + "jm");
				}
		    }
		}

		for (itr_p = Plants_of_today.begin(); itr_p != Plants_of_today.end(); itr_p++)
		{
			for (j = 0; j < 44; j++)
			{
				for (m = 0; m < 2; m++) {
				    S[(*itr_p)->number][j][m]=Path_determination.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "S"+ itos((*itr_p)->number)+"jkm");
				}
			}
		}
		
		for (itr_p = Plants_of_today.begin(); itr_p != Plants_of_today.end(); itr_p++) {
			for (itr_dea= dealers_of_today.begin(); itr_dea!=dealers_of_today.end(); itr_dea++)
			{
				GRBLinExpr Assignment= 0.0;
				Assignment += Y[(*itr_p)->number][((*itr_dea)->number];
				
				for (j = 0; j < 44; j++) {
					for (m = 0; m < 2; m++) {
						Assignment += X[(*itr_p)->number][j][(*itr_dea)->number][m];

					}
				}
				Path_determination.addConstr(Assignment, GRB_EQUAL, 1.0, "Assignment" + itos((*itr_p)->number) + itos((*itr_dea)->number));
			}
		}
		for ( j = 0; j < 44; j++)
		{
			GRBLinExpr Capacity = 0.0;
			for ( itr_d = demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++)
			{
				for (m = 0; m < 2; m++)
				{
					Capacity += (X[itr_d->plant->number][j][itr_d->dealer->number][m] * itr_d->inventory.size());
				}
			}
			Path_determination.addConstr(Capacity, GRB_LESS_EQUAL, vdcs[j]->capacity - vdcs[j]->vdcInventory.size(),"Capacity"+itos(j));
		}

		for ( itr_d= demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++)
		{
		
			Path_determination.addConstr(X[itr_d->plant->number][1][itr_d->dealer->number][0],GRB_EQUAL,0.0,"Rail not available 1");
			Path_determination.addConstr(X[itr_d->plant->number][31][itr_d->dealer->number][0],GRB_EQUAL,0.0,"Rail not available 31");
			Path_determination.addConstr(X[itr_d->plant->number][41][itr_d->dealer->number][0],GRB_EQUAL,0.0,"Rail not available 41");
			
		}

		for ( itr_d = demand_of_today.begin(); itr_d!=demand_of_today.end(); itr_d++) //need to sum over plants
		{
			for (j = 0; j < 44; j++) {
				for (m = 0; m < 2; m++) {
					GRBLinExpr Load_Equation = 0.0;
					Load_Equation += (L[itr_d->plant->number][j][m]+S[itr_d->plant->number][j][m]);
					for ( itr_p =Plants_of_today.begin() ; itr_p!=Plants_of_today.end();itr_p++)
					{
						Load_Equation +=(itr_d->inventory.size()* X[(*itr_p)->number][j][itr_d->dealer->number][m]/mode_load_capacity[m]);
					}
					Path_determination.addConstr(Load_Equation, GRB_EQUAL, 0,"Load_Equation"+itos(itr_d->plant->number)+itos(j)+itos(m));
				}
			}
		}

		Path_determination.optimize();

		for ( itr_v = vehicles_of_today.begin(); itr_v!=vehicles_of_today.end(); itr_v++)
		{
			if (Y[(*itr_v)->path.plant->number][(*itr_v)->path.dealer->number].get(GRB_DoubleAttr_X)==1)
			{
				(*itr_v)->path.vdc = nullptr;
				(*itr_v)->path.truck = 1;
				(*itr_v)->path.rail = 0;
			}
		}
		
		for ( itr_v = vehicles_of_today.begin();itr_v!= vehicles_of_today.end(); itr_v++)
		{   
			for (j=0;j<44;j++)
			{
				for ( m = 0; m < 2; m++)
				{
					if (X[(*itr_v)->path.plant->number][j][(*itr_v)->path.dealer->number][m].get(GRB_DoubleAttr_X) == 1) {
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
    }

	catch (GRBException e) {
		 cout << "Error number: " << e.getErrorCode() << endl;
		 cout << e.getMessage() << endl;
	 }
	 catch (...) {
		 cout << "Error during optimization" << endl;
	 }
}


