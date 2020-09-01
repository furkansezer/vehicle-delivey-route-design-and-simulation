#define _USE_MATH_DEFINES
#define cap 10

#include <iostream>
#include <queue>
#include <cmath>
#include "time.h"
#include "VDC.h"
#include "Dealer.h"
using namespace std;

void bubblesort(double* alfa, int amount, int* sortid)
{
	double alfatemp;
	int idtemp;
	bool sort = false;

	for (int i = 0; i < amount; i++)
		sortid[i] = i;

	while (!sort)
	{
		sort = true;
		for (int i = 1; i < amount; i++)
			if (alfa[i] < alfa[i - 1])
			{
				sort = false;
				alfatemp = alfa[i];
				alfa[i] = alfa[i - 1];
				alfa[i - 1] = alfatemp;
				idtemp = sortid[i];
				sortid[i] = sortid[i - 1];
				sortid[i - 1] = idtemp;
			}
	}
}

void Sweep(VDC* p)
{
	//Assumed vehicle capacity is 10
	//Assumed max. number of dealers is 10
	double sortid[10], sortdemand[10], tempx, tempy, dealeralfa[10], sortalfa[10];
	double alfa;
	int sortindex[10], clusterid[10], c, capacity;

	//Calculating alfa values
	vector<Vehicle*>::iterator itr;
	vector<Dealer*> uniqueDealers;
	vector<Dealer*>::iterator itr2;
	for (itr = p->vdcInventory.begin(); itr != p->vdcInventory.end(); itr++) {
		if (uniqueDealers.empty())
			uniqueDealers.push_back((*itr)->path.dealer);
		bool c = true;
		for (itr2 = uniqueDealers.begin(); itr2 != uniqueDealers.end(); itr2++) {
			if ((*itr)->path.dealer == *itr2)
				c = false;
		}
		if(c == true)
			uniqueDealers.push_back((*itr)->path.dealer);
	}

	int dealeramount = uniqueDealers.size();
	int i = 0;

	for (itr2 = uniqueDealers.begin(); itr2 != uniqueDealers.end(); itr2++)
	{
		tempx = (*itr2)->locx - p->locationx;
		tempy = (*itr2)->locy - p->locationy;
		if (tempy > 0)
			dealeralfa[i] = acos(tempx / sqrt(pow(tempx, 2) + pow(tempy, 2)));
		else
			dealeralfa[i] = 2 * M_PI - acos(tempx / sqrt(pow(tempx, 2) + pow(tempy, 2)));
		i++;
	}

	alfa = 2 * M_PI*(rand() % 100) / 100.0;
	for (int i = 0; i < dealeramount; i++)
	{
		if (dealeralfa[i] > alfa)
			sortalfa[i] = dealeralfa[i] - alfa;
		else
			sortalfa[i] = 2 * M_PI + dealeralfa[i] - alfa;
	}
	bubblesort(sortalfa, dealeramount, sortindex);

	for (int i = 0; i < dealeramount; i++)
	{
		sortid[sortindex[i]] = dealerid[i];
		sortdemand[sortindex[i]] = dealerdemand[i];
	}
	capacity = cap;
	c = 0;
	for (int i = 0; i < dealeramount; i++)
	{
		if (sortdemand[i] < capacity)
		{
			clusterid[i] = c;
			capacity = capacity - sortdemand[i];
		}
		else
		{
			capacity = cap;
			c = c + 1;
			clusterid[i] = c;
			capacity = capacity - sortdemand[i];
		}
	}
	for (int i = 0; i < dealeramount; i++)
	{
		cout << sortid[i] << "   " << clusterid[i] << endl;
	}
	cout << endl;
	//TSP??
}

int main()
{
	//srand(time(NULL));
	double depotx = 5, depoty = 5, id[5], demand[5], x[5], y[5];
	int n = 4;
	id[0] = 100;
	id[1] = 200;
	id[2] = 305;
	id[3] = 400;
	demand[0] = 3;
	demand[1] = 4;
	demand[2] = 7;
	demand[3] = 2;
	x[0] = 8;
	x[1] = 5;
	x[2] = 0;
	x[3] = 2;
	y[0] = 5;
	y[1] = 3;
	y[2] = 0;
	y[3] = 7.5;
	Sweep(depotx, depoty, n, id, demand, x, y);
    return 0;
}

