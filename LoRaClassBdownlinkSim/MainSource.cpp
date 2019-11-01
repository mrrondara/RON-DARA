/*****************************************************************************************************

# LoRaWAN Class B mode Simulation #
Researcher : Lee Chan-jae
E-mail : cjleeno27@gmail.com

*****************************************************************************************************/

#include <stdio.h>
#include <iostream>
#include <time.h>
#include "NodeInfo.h"
#include "ProbLib.h"
#include "NodeFunc.h"

void main(void)
{
	int i; int t; int iter; int iter2;
	/* Structure setting */
	struct GW GW[NumGW];    //Gateway
	struct GW *p_GW[NumGW];
	/*int j;
	for (j = 0; j < 1000; j++) {
		double Norm = normrnd(mean, sigma);
		FILE* fnrm = fopen("E:\\normrnd\\normrnd.txt", "a");
		fprintf(fnrm, "%lf\n", Norm);
		fclose(fnrm);
	}*/
	for (i = 0; i<NumGW; i++) {
		p_GW[i] = &GW[i];
	}
	struct ED ED[NumED];    //End-device
	struct ED *p_ED[NumED];
	for (i = 0; i < NumED; i++) {
		p_ED[i] = &ED[i];
	}
	srand((unsigned)time(NULL));
	/* Simulation */     
	if (MODE_WtNfCheckMode == 0)iter2max = 19;
	else iter2max = 1;
	for (iter2 = 0; iter2 < iter2max; iter2++) {
		UpdateArrivalRate(iter2, PingSlotParameter);
		totArrPck = 0;
		totDepPck = 0;
		for (iter = 0; iter < SimIteration; iter++) {
			printf("%i\n", iter);
			/* Initializing */
			InitializeGW(p_GW, iter2);
			InitializeED(p_ED, iter2);
			InitializeETC();
			AllocatePingslot(p_ED, PingSlotParameter);
			/* Operation over time */
			for (t = 0; t <= EndTime; t+=UnitTime) {
				loopcheck = 0;
				if (t <= EndTime2)PacketGeneration(p_GW, t);
				FrameTx(p_GW, p_ED, t);
				Energy(p_ED, t);
				UpdatePingSlot(p_ED, t);
				PerformanceMetric(p_GW, t, iter, p_ED);
			}
		}
		printf("Finished\n");
	}
	printf("Simulation is terminated.\n");
	system("pause>null");
}
