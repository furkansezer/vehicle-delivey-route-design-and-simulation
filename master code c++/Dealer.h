#pragma once

class Dealer
{
public:
	int number;
	double locx;
	double locy;

	Dealer(int k, double lx, double ly) : number(k), locx(lx), locy(ly) {};
	Dealer();
	~Dealer();
};

