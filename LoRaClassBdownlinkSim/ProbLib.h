#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef _PROBLIB_
#define _PROBLIB_

#define A 	16807L        		/* multiplier (7**5) for 'ranf' */
#define M 	2147483647L       	/* modulus (2**31-1) for 'ranf' */

static long SeedTable[16] = { 0L,    	/* seeds for streams 1 thru 15  */
1973272912L,  747177549L,    20464843L,  640830765L, 1098742207L,
78126602L,   84743774L,   831312807L,  124667236L, 1172177002L,
1124933064L, 1223960546L,  1878892440L, 1449793615L,  553303732L };

static int strm = 1;      			/* index of current stream */

/******************************************************************************
UNIFORM  [0,  1]  RANDOM NUMBER GENERATOR

This implementation is for Intel 8086/8 and 80286/386 CPUs using
C compilers with 16-bit short integers and 32-bit long integers.
******************************************************************************/
double ranf(void)
{
	short *p, *q, k; long Hi, Lo;
	p = (short *)&SeedTable[strm]; Hi = *(p)*A;     /* 16807*H->Hi  */
	*(p) = 0; Lo = SeedTable[strm] * A;              	/* 16807*L->Lo  */
	p = (short *)&Lo; Hi += *(p);     				/* add high-order bits of Lo to Hi */
	q = (short *)&Hi;            					/* low-order bits of Hi->Lo  */
	*(p) = *(q + 1) & 0x7fff;                  		/* clear sign bit  */
	k = *(q) << 1; if (*(q + 1) & 0x8000)  k++; 		/* Hi bits 31-45->K  */
														/* form Z + K [- M] (where Z=Lo) : presubtract M to avoid overflow  */
	Lo -= M; Lo += k; if (Lo<0)  Lo += M;
	SeedTable[strm] = Lo;
	return((double)Lo*4.656612875E-10);    		/* Lo x 1/(2**31-1)  */
}

/******************************************************************************
UNIFORM [a, b] RANDOM VARIATE GENERATOR

'uniform' returns a pseudo-random variate from a uniform
distribution with lower bound a and upper bound b.
******************************************************************************/
double uniform(double a, double b)
{
	if (a>b) printf("uniform Argument Error: a > b");
	return(a + (b - a)*ranf());
}

/******************************************************************************
gamma Function

Returns the value ln(Gamma(xx)) for xx >0.
ref. Numerical Recipes in C
******************************************************************************/
double gammln(double xx)
{
	double	x, y, tmp, ser;
	static double cof[6] = { 76.18009172947146, -86.50532032941677,
		24.01409824083091, -1.231739572450155,
		0.128650973866179e-2, -0.5395239384953e-5 };
	int	j;

	y = x = xx;
	tmp = x + 5.5;
	tmp -= (x + 0.5)*log(tmp);
	ser = 1.000000000190015;
	for (j = 0; j <= 5; j++) ser += cof[j] / ++y;

	return -tmp + log(2.5066282746310005*ser / x);
}

/******************************************************************************
random function

"Minimal" random number generator of Park and Miller with Bays-Durham
shuffle and added safeguards. Returns a uniform random deviated between 0.0
and 1.0 (exclusive of the endpoint values).
Call with idum a negative integer to initialize; thereafter, do not alter
idum between successive deviates in a sequence. RNMX should approximate the
largest floating value that is less than 1.
******************************************************************************/
/* #define	IA 		16807 */
/* #define	IM		2147483647 */
/* #define	AM 		(1.0/IM) */
/* #define	IQ		127773 */
/* #define	IR		2836 */
#define	NTAB	32 
/* #define	NDIV	(1+(IM-1)/NTAB); */
/* #define	EPS		1.2e-7 */
/* #define	RNMX	(1.0-EPS)	 */

double	ran1(long *idum)
{
	long	IA = 16807;
	long	IM = 2147483647;
	double	AM = (1.0 / IM);
	long	IQ = 127773;
	long	IR = 2836;
	/* int		NTAB	=32;  */
	long	NDIV = (1 + (IM - 1) / NTAB);
	double	EPS = 1.2e-7;
	double	RNMX = (1.0 - EPS);

	int		j;
	long	k;
	static long	iy = 0;
	static long iv[NTAB];
	double	temp;

	if (*idum <= 0 || !iy) {
		if (-(*idum) < 1) 	*idum = 1;
		else 				*idum = -(*idum);
		for (j = NTAB + 7; j >= 0; j--) {
			k = (*idum) / IQ;
			*idum = IA*(*idum - k*IQ) - IR*k;
			if (*idum < 0)	*idum += IM;
			if (j < NTAB)	iv[j] = *idum;
		}
		iy = iv[0];
	}

	k = (*idum) / IQ;
	*idum = IA*(*idum - k*IQ) - IR*k;
	if (*idum < 0)	*idum += IM;
	j = iy / NDIV;
	iy = iv[j];
	iv[j] = *idum;
	if ((temp = AM*iy) > RNMX)	return RNMX;
	else						return temp;
}

/******************************************************************************
EXPONENTIAL RANDOM VARIATE GENERATOR

'expntl' returns a psuedo-random variate from a negative
exponential distribution with mean x.
******************************************************************************/
double expntl(double mean)
{
	double temp;
	temp = ranf();
	temp = (-1.)*mean*log(temp);
	return((temp));
}

/******************************************************************************
Poisson Deviates

Returns as a double number an integer value that is a random deviate drawn
from a Poisson distribution of mean xm, using ran1(idum) as a source of
uniform random deviates.
******************************************************************************/
#define	PI	3.141592654

double	poidev(double xm, long *idum)
{
	/* double	gammln(double xx); */
	/* double	ran1(long *idum); */
	static double	sq, alxm, g, oldm = (-1.0);
	double		em, t, y;

	if (xm < 12.0) {
		if (xm != oldm) {
			oldm = xm;
			g = exp(-xm);
		}
		em = -1;
		t = 1.0;
		do {
			++em;
			t *= ran1(idum);
		} while (t>g);
	}
	else {
		if (xm != oldm) {
			oldm = xm;
			sq = sqrt(2.0*xm);
			alxm = log(xm);
			g = xm*alxm - gammln(xm + 1.0);
		}
		do {
			do {
				y = tan(PI*ran1(idum));
				em = sq*y + xm;
			} while (em < 0.0);
			em = floor(em);
			t = 0.9*(1.0 + y*y)*exp(em*alxm - gammln(em + 1.0) - g);
		} while (ran1(idum) > t);
	}
	return	em;
}

double	factorial(int n)
{
	double product;

	if (n<0) printf("Negative factorial in routine factorial");
	if (n == 1)	return	1.0;
	for (product = 1.0; n>1; n--) 	product *= n;

	return product;

	/*
	if(n<=1)    return 1;
	else        return (n * factorial(n-1));
	*/

}

/* Normal randod variable */
double normrnd(double mean, double stdDev) {
	double u, v, s;
	do {
		u = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
		v = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
		s = u * u + v * v;
	} while (s >= 1 || s == 0);
	double mul = sqrt(-2.0 * log(s) / s);
	return mean + stdDev * u * mul;
}

/******************************************************************************
Poisson prob. (mean,k)

Returns the Poisson distribution. as a double number
******************************************************************************/
double  poisson(double mean, int k)
{
	return (pow(mean, k)*exp(-mean) / factorial(k));
}

#endif