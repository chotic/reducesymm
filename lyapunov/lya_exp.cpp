#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define cutoffNum 128
/* define cuttoff number for the Fourier modes */

#define stepSize 0.001
/* define the step size of the simulation */

#define maxTime 1000
/* define the max time step of the simulation */

#define L 11/3.1415926
/* period of x in KS equation */

class complx
{
	public:
		double real;
		double img;
		complx(double real_, double img_)
		{
			real = real_;
			img = img_;
		}
		complx()
		{
			real = 0;
			img = 0;
		}
		complx(complx &c)
		{
			real = c.real;
			img = c.img;
		}
		complx operator +( complx &c)
		{
			return complx(real + c.real, img + c.img);
		}
		complx operator -( complx &c)
		{
			return complx(real - c.real, img - c.img);
		}
		complx operator *( complx &c)
		{
			return complx(real * c.real - img * c.img, real * c.img + img * c.real);
		}
		complx operator ~()
		{
			return complx(real, - img);
		}
		complx operator *(double m)
		{
			return complx(real * m, img * m);
		}
		complx operator /(double l)
		{
			return complx(real / l, img / l);
		}
};
/* complex number */

class cVector
{
	public:
		complx terms[cutoffNum];
		cVector operator +(cVector &v)
		{
			for(int i = 0; i < cutoffNum; i++)
				terms[i] = terms[i] + v.terms[i];
			return *this;
		}
		cVector operator -(cVector &v)
		{
			for(int i = 0; i < cutoffNum; i++)
				terms[i] = terms[i] - v.terms[i];
			return *this;
		}
		complx operator *(cVector &v)
		{
			complx * result = new complx(0,0);
			for(int i = 0; i < cutoffNum; i++)
				*result = *result + terms[i] * (~(v.terms[i]));
			return *result;
		}
		cVector operator *(complx c)
		{
			for (int i = 0; i < cutoffNum; i++)
				this->terms[i] = (this->terms[i]) * c;
			return *this;
		}
		double abs()
		{
			double absolute = 0;
			for(int i = 0; i < cutoffNum; i++)
				absolute += (this->terms[i]*(~this->terms[i])).real;
			return sqrt(absolute);
		}
		cVector operator /(double l)
		{
			for(int i = 0; i < cutoffNum; i++)
				terms[i] = terms[i] / l;
			return *this;
		}
		void copy(const cVector &v)
		{
			for(int i = 0; i < cutoffNum; i++)
				terms[i] = v.terms[i];
		}
};
/* complex vector of GSVs and the vector under simulation */

cVector crntGSVs[cutoffNum];  
/* define the current GSVs */

int fwdTime;
/* set the forward time of the simulation */
 
double sumLypExpo[cutoffNum];
/* cumulated logrithsm of the diagonal term of matrix R in QR decomposition */

complx k[cutoffNum][5];
/* k coefficients of the Runge-Kutta algorithms */

complx step(stepSize,0);
/* stepsize in complex form */

complx bchrTab[6][6] = {{complx(1/4, 0)},
						{complx(3/32, 0), complx(9/32, 0)},
						{complx(1932/2197, 0), complx(-7200/2197, 0), complx(7296/2197, 0)},
						{complx(439/216, 0), complx(-8, 0), complx(3680/513, 0), complx(-854/4104, 0)},
						{complx(-8/27, 0), complx(2, 0), complx(-3544/2565, 0), complx(1859/4104, 0), complx(-11/40, 0)},
						{complx(16/135, 0), complx(0, 0), complx(6656/12825, 0), complx(28561/56430, 0), complx(-9/50, 0), complx(2/55, 0)}};
/* butcher tableau in 4-th order Runge-Kutta */

void init()
/* initiate the GSV to be the basis vectors, also initiate fwdTime and sumLypExpo */
{
	fwdTime = 0;
	for(int i = 0; i < cutoffNum; i++)
	{
		sumLypExpo[i] = 0;
		for(int j = 0; j < cutoffNum; j++)
		{
			crntGSVs[i].terms[j].img = 0;
			if(i == j)
				crntGSVs[i].terms[j].real = 1;
			else
				crntGSVs[i].terms[j].real = 0;
		}
	}
}

complx fODE(int h, int j, int i)
/* the ode function to calculate k coefficient */
{
	cVector temp;
	temp.copy(crntGSVs[h]);
	for(int m = 0; m < j; m++)
		for(int n = 0; n < cutoffNum; n++)
			temp.terms[n] = temp.terms[n] + bchrTab[j][m] * k[n][m];
	complx sigma(0, 0), a, b;
	for(int x = -cutoffNum+1; x < cutoffNum; x++)
	{
		if(abs(h - x) < cutoffNum)
		{
			if(x < 0)
				a = ~temp.terms[- x];
			else
				a = temp.terms[x];
			if((h - x) < 0)
				b = ~temp.terms[x - h];
			else
				b = temp.terms[h - x];
			sigma = sigma + a * b;
		}
	}
	return (temp.terms[i] * (pow(i / L, 2) - pow(i / L , 4))) - ((complx(0, 1) * (i / L * 0.5)) * sigma);
}

void fwdSim()
/* the forward of the dynamic system */
{
	while(fwdTime++ < maxTime)
	{
		for(int h = 0; h < cutoffNum; h++)
		{
			for(int j = 0; j < 5; j++)
				for(int i = 0; i < cutoffNum; i++)
					k[i][j] = step * fODE(h, j, i);
			for(int i = 0; i < cutoffNum; i++)
				for(int c = 0; c < 5; c++)
					crntGSVs[h].terms[i] = crntGSVs[h].terms[i] + k[i][c] * bchrTab[5][c];
		}
		/* G_prime matrix, then QR decomposition */

		for(int n = 0; n < cutoffNum; n++)
		{
			cVector temp;
			temp.copy(crntGSVs[n]);
			for(int m = 0; m < n; m++)
				crntGSVs[n] = crntGSVs[n] - crntGSVs[m] * (temp * crntGSVs[m]);
			crntGSVs[n] = crntGSVs[n] / crntGSVs[n].abs();
			sumLypExpo[n] = sumLypExpo[n] + log((temp * crntGSVs[n]).real);
		}

		printf("check the time: %d\n", fwdTime);
	}
}

void lyaExpo()
/* to get the lyapunov exponent of the system */
{
	for(int i = 0; i < cutoffNum; i++)
		printf("The ith LE: %f/n", sumLypExpo[i] / fwdTime);
}

int main()
{
	init();
	fwdSim();
	lyaExpo();
}
