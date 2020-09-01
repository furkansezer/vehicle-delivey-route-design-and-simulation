/* Copyright 2018, Gurobi Optimization, LLC */

/* Solve a traveling salesman problem on a randomly generated set of
points using lazy constraints.   The base MIP model only includes
'degree-2' constraints, requiring each node to have exactly
two incident edges.  Solutions to this model may contain subtours -
tours that don't visit every node.  The lazy constraint callback
adds new constraints to cut them off. */
#define _USE_MATH_DEFINES
#include "gurobi_c++.h"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <math.h> 
#include <sstream>
using namespace std;

string itos(int i) { stringstream s; s << i; return s.str(); }
double GreatCircleDistance(double x1, double y1, double x2, double y2);
void findsubtour(int n, double** sol, int* tourlenP, int* tour);

// Subtour elimination callback.  Whenever a feasible solution is found,
// find the smallest subtour, and add a subtour elimination constraint
// if the tour doesn't visit every node.

class subtourelim : public GRBCallback
{
public:
	GRBVar** vars;
	int n;
	subtourelim(GRBVar** xvars, int xn) {
		vars = xvars;
		n = xn;
	}
protected:
	void callback() {
		try {
			if (where == GRB_CB_MIPSOL) {
				// Found an integer feasible solution - does it visit every node?
				double **x = new double*[n];
				int *tour = new int[n];
				int i, j, len;
				for (i = 0; i < n; i++)
					x[i] = getSolution(vars[i], n);

				findsubtour(n, x, &len, tour);

				if (len < n) {
					// Add subtour elimination constraint
					GRBLinExpr expr = 0;
					for (i = 0; i < len; i++)
						for (j = i + 1; j < len; j++)
							expr += vars[tour[i]][tour[j]];
					addLazy(expr <= len - 1);
				}

				for (i = 0; i < n; i++)
					delete[] x[i];
				delete[] x;
				delete[] tour;
			}
		}
		catch (GRBException e) {
			cout << "Error number: " << e.getErrorCode() << endl;
			cout << e.getMessage() << endl;
		}
		catch (...) {
			cout << "Error during callback" << endl;
		}
	}
};

// Given an integer-feasible solution 'sol', find the smallest
// sub-tour.  Result is returned in 'tour', and length is
// returned in 'tourlenP'.

void findsubtour(int n, double** sol, int* tourlenP, int* tour)
{
	bool* seen = new bool[n];
	int bestind, bestlen;
	int i, node, len, start;

	for (i = 0; i < n; i++)
		seen[i] = false;

	start = 0;
	bestlen = n + 1;
	bestind = -1;
	node = 0;
	while (start < n) {
		for (node = 0; node < n; node++)
			if (!seen[node])
				break;
		if (node == n)
			break;
		for (len = 0; len < n; len++) {
			tour[start + len] = node;
			seen[node] = true;
			for (i = 0; i < n; i++) {
				if (sol[node][i] > 0.5 && !seen[i]) {
					node = i;
					break;
				}
			}
			if (i == n) {
				len++;
				if (len < bestlen) {
					bestlen = len;
					bestind = start;
				}
				start += len;
				break;
			}
		}
	}

	for (i = 0; i < bestlen; i++)
		tour[i] = tour[bestind + i];
	*tourlenP = bestlen;

	delete[] seen;
}

// Big circle distance between points 'i' and 'j'.

double GreatCircleDistance(double x1, double y1, double x2, double y2)
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

/*Updated according to big circle distance method.*/

double TSPdistance(double*x, double*y, int i, int j) { return GreatCircleDistance(x[i], y[i], x[j], y[j]); }

int*
TSP(int* demand,double* x,double* y,int n)
{
	int i;
	
/*	X ve Y koordinatlar. Bunlarin ilk elemani VDCnin locationi olacak. */
	
	
	
	GRBEnv *env = NULL;
	GRBVar **vars = NULL;

	vars = new GRBVar*[n];
	for (i = 0; i < n; i++)
		vars[i] = new GRBVar[n];

	try {
		int j;

		env = new GRBEnv();
		GRBModel model = GRBModel(*env);

		// Must set LazyConstraints parameter when using lazy constraints

		model.set(GRB_IntParam_LazyConstraints, 1);

		// Create binary decision variables

		for (i = 0; i < n; i++) {
			for (j = 0; j <= i; j++) {
				vars[i][j] = model.addVar(0.0, 1.0, (demand[i]+demand[j])*distance(x, y, i, j),
					GRB_BINARY, "x_" + itos(i) + "_" + itos(j));
				vars[j][i] = vars[i][j];
			}
		}

		// Degree-2 constraints

		for (i = 0; i < n; i++) {
			GRBLinExpr expr = 0;
			for (j = 0; j < n; j++)
				expr += vars[i][j];
			model.addConstr(expr == 2, "deg2_" + itos(i));
		}

		// Forbid edge from node back to itself

		for (i = 0; i < n; i++)
			vars[i][i].set(GRB_DoubleAttr_UB, 0);

		// Set callback function

		subtourelim cb = subtourelim(vars, n);
		model.setCallback(&cb);

		// Optimize model

		model.optimize();

		// Extract solution

		if (model.get(GRB_IntAttr_SolCount) > 0) {
			double **sol = new double*[n];
			for (i = 0; i < n; i++)
				sol[i] = model.get(GRB_DoubleAttr_X, vars[i], n);

			int* tour = new int[n];
			int len;

			findsubtour(n, sol, &len, tour);
			assert(len == n);

			return tour;

			for (i = 0; i < n; i++)
				delete[] sol[i];
			delete[] sol;
		}

	}
	catch (GRBException e) {
		cout << "Error number: " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Error during optimization" << endl;
	}

	for (i = 0; i < n; i++)
		delete[] vars[i];
	delete[] vars;
	delete env;
	return 0;
}