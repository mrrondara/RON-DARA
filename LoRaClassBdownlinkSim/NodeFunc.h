#include <stdio.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "NodeInfo.h"
#include "ProbLib.h"

#ifndef _NODEFUNC_
#define _NODEFUNC_

FILE *pTestOutWq;
FILE *pTestOutLq;
FILE *pTestOutW;
FILE *pTestOutL;
int pTestOutsw = TestOutsw;
FILE *pResSF;    //[1] Spreading factor.
int pResSFsw = 0;
FILE *pResWtNf1;    //[2] Waiting time and Number of frames.
FILE *pResWtNf2;
FILE *pResWtNf3;
FILE *pResWtNf4;
FILE *pResWtNf5;
FILE *pResWtNf6;
FILE *pResWtNf7;
FILE *pResWtNf8;
FILE *pResWtNf9;
FILE *pResWtNf10;
FILE *pResWtNf11;
FILE *pResWtNf12;
FILE *pResWtNf13;
FILE *pResWtNf14;
FILE *pResWtNf15;
FILE *pResWtNf16;
FILE *pResWtNf17;
FILE *pResWtNf18;
FILE *pResWtNf19;
FILE *pRewWtNfCheck;
int pResWtNfsw;

void UpdateArrivalRate(int iter2, int pingPa) {
	int i; int j;
	double ExServTime = ESset[pingPa];
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 19; j++) {
			if (i == 0) {
				ArribalRateTable[i][j] = 0.05*(j+1);
			}
			else {
				ArribalRateTable[i][j] = ArribalRateTable[0][j] / ExServTime;
			}
		}
	}
	if (MODE_WtNfCheckMode == 0) {
		ArrivalRate = ArribalRateTable[1][iter2];    //[frame/ms], (1[frame/s] = 0.001[frame/ms])
		pResWtNfsw = iter2 + 1;
		printf("> Simulation of traffic intensity = %.2f ...ing...", 0.05*(iter2+1));
	}
	else {
		ArrivalRate = TestVal / ExServTime;
		pResWtNfsw = 999;
		printf("> Simulation of traffic intensity = %f ...ing...", ArrivalRate*ExServTime);
	}
	ArrivalMean = ArrivalRate*(double)UnitTime;
}

void InitializeGW(GW *p_GW[NumGW], int iter2) {
	int i; int j;
	for (i = 0; i < NumGW; i++) {
		p_GW[i]->GW_Cover = GatewayCoverage;
		p_GW[i]->GW_Loc_x = 0; p_GW[i]->GW_Loc_y = 0;    //(0,0)
		for (j = 0; j < QueueLength; j++) {
			p_GW[i]->Queue[j] = 0;
		}
	}
	QueueBlock = 0;
}

double RandomLocation(void) {
	double rv; int rv2; double sign; double temp;
	rv = ((double)rand() / (double)RAND_MAX)*GatewayCoverage; // (0,1)*GatewayCoverage
	do { rv2 = (double)rand() / (double)RAND_MAX; } 
	while (rv2 == 0.5);
	if (rv2 < 0.5) sign = 1;
	else sign = -1 ;
	temp = rv*sign;
	return temp;
}

void InitializeED(ED *p_ED[NumED], int iter2) {
	int i;
	if (pResSFsw == 1)pResSF = fopen("D:\\[14] Revision\\outputs\\[1]SF.txt", "a");
	for (i = 0; i < NumED; i++) {
		do {
			double Pnoise = -174 + 10 * log10(BW);
			double Norm = normrnd(mean, sigma);
			double x = xmin + ((xmax - xmin)* ((double)rand() / (double)RAND_MAX));
			double y = ymin + ((ymax - ymin)* ((double)rand() / (double)RAND_MAX));
			double Range = sqrt((x*x) + (y*y));
			double PL = PL0 + ((10*gamma*log10(Range/RefRange)) + Norm);
			double Prx = Ptx + GL - PL;
			p_ED[i]->SNR = Prx - Pnoise;
			/*p_ED[i]->ED_Loc_x = RandomLocation();
			p_ED[i]->ED_Loc_y = RandomLocation();
			p_ED[i]->DistanceToGW = sqrt((p_ED[i]->ED_Loc_x*p_ED[i]->ED_Loc_x + p_ED[i]->ED_Loc_y*p_ED[i]->ED_Loc_y));
			*/
		} while (p_ED[i]->SNR <= GatewayCoverage);

		if (p_ED[i]->SNR >= SNRth[0]) p_ED[i]->SF_ID = 0;
		else if (p_ED[i]->SNR >= SNRth[1] && p_ED[i]->SNR < SNRth[0]) p_ED[i]->SF_ID = 1;
		else if (p_ED[i]->SNR >= SNRth[2] && p_ED[i]->SNR < SNRth[1]) p_ED[i]->SF_ID = 2;
		else if (p_ED[i]->SNR >= SNRth[3] && p_ED[i]->SNR < SNRth[2]) p_ED[i]->SF_ID = 3;
		else if (p_ED[i]->SNR >= SNRth[4] && p_ED[i]->SNR < SNRth[3]) p_ED[i]->SF_ID = 4;
		else if (p_ED[i]->SNR >= SNRth[5] && p_ED[i]->SNR < SNRth[4]) p_ED[i]->SF_ID = 5;
		else if (p_ED[i]->SNR >= SNRth[6] && p_ED[i]->SNR < SNRth[5]) p_ED[i]->SF_ID = 6;
		else printf("error\n");
		p_ED[i]->SF = pingTable[0][p_ED[i]->SF_ID];
		if (pResSFsw == 1) fprintf(pResSF, "%d\n", p_ED[i]->SF);
		//p_ED[i]->TotNumPing = 0;
		//p_ED[i]->ValNumPing = 0;
		p_ED[i]->Econsm = 0;
		p_ED[i]->Estate = 0;
	}
	if (pResSFsw == 1)fclose(pResSF);
}

void InitializeETC() {
	int i;
	for (i = 0; i < 4; i++) {
		PM_Table[i] = 0;
	}
	PM_count = 0;
	PM_count2 = 0;
	CHstatus = 0;
	CurrentTxFrameID = -999;
	RemainingToA_slot = -999;
	if (pTestOutsw != 1) {
		totArrPck = 0;
		totDepPck = 0;
	}
}

void AllocatePingslot(ED *p_ED[NumED], int iter) {
	int i; double temp;
	for (i = 0; i < NumED; i++) {
		p_ED[i]->pingPeriod_slot = pow(2, (12 - pingTable[1][iter]));
		p_ED[i]->pingPeriod = UnitTime * p_ED[i]->pingPeriod_slot;
		temp = (double)rand() / (double)RAND_MAX;
		temp *= (double)(p_ED[i]->pingPeriod_slot - 1);
		p_ED[i]->pingOffset_slot = (int)temp;
		p_ED[i]->pingSlot = UnitTime * p_ED[i]->pingOffset_slot;
	}
}

void PacketGeneration(GW *p_GW[NumGW], int t) {
	int i; int j;
	double temp; double temp2;
	if (t == 0) {
		do {
			temp2 = (double)rand() / (double)RAND_MAX;
		} while (temp2 == 1. || temp2 == 0.);
		do {
			temp = -log(1.0 - temp2) / ArrivalRate;
		} while (temp < 1.);
		temp = temp / 30;
		ArrivalTime = (int)temp*30;
		if (ArrivalTime == 0)ArrivalTime = 30;
	}
	for (i = 0; i < NumGW; i++) {
		if (t == ArrivalTime && QueueBlock == 0) {
			totArrPck++;
			j = 0;
			do {
				if (p_GW[i]->Queue[j] == 0) {
					p_GW[i]->Queue[j] = t;
					j = -99;
				}
				j++;
			} while (j > 0 && j < QueueLength);
			p_GW[i]->NumFrameInQ = 0;
			for (j = 0; j < QueueLength; j++) {
				if (p_GW[i]->Queue[j] > 0)p_GW[i]->NumFrameInQ++;
			}
			if (p_GW[i]->NumFrameInQ == QueueLength) {
				printf("\nQueue is full. Please reallocate size of queue.\n");
				system("PAUSE");
				QueueBlock = 1;
				PRTswitch = 0;
			}
			do {
				temp2 = (double)rand() / (double)RAND_MAX;
			} while (temp2 == 1. || temp2 == 0.);
			do {
				temp = -log(1.0 - temp2) / ArrivalRate;
			} while (temp < 1.);
			temp = temp / 30;
			ArrivalInterval = (int)temp * 30;
			if (ArrivalInterval == 0)ArrivalInterval = 30;
			ArrivalTime += ArrivalInterval;
		}
	}
}

void FrameTx(GW *p_GW[NumGW], ED *p_ED[NumED], int t) {
	int i; int j;
	double temp;
	for (i = 0; i < NumGW; i++) {
		if (CHstatus == 1) {
			RemainingToA_slot--;
			if (RemainingToA_slot == 0) {
				PM_Table[2] += (double)(t - ServTrec);
				PM_count2++;
				if (pTestOutsw == 1) {
					pTestOutW = fopen("D:\\[14] Revision\\outputs\\[1]W.txt", "a");
					fprintf(pTestOutW, "%d\n", (t - ServTrec));
					fclose(pTestOutW);
				}
				if (RemainingToA_slot == 1)p_ED[CurrentTxFrameID]->Econsm += Erx;
				p_ED[CurrentTxFrameID]->Estate = 0;
				CHstatus = 0;
				CurrentTxFrameID = -999;
				RemainingToA = -999;
				RemainingToA_slot = -999;
				totDepPck++;
			}
		}
		else if (CHstatus == 0 && CurrentTxFrameID != -999) {
			if (t == p_ED[CurrentTxFrameID]->pingSlot) {
				//p_ED[CurrentTxFrameID]->ValNumPing++;
				p_ED[CurrentTxFrameID]->Estate = 1;
				RemainingToA = pingTable[2][p_ED[CurrentTxFrameID]->SF_ID];
				RemainingToA_slot = ceil((double)RemainingToA / (double)UnitTime);
				if (RemainingToA_slot == 0) RemainingToA_slot = 1;
				CHstatus = 1;
				RemainingToA_slot--;
				if (RemainingToA_slot == 0) {
					PM_Table[2] += (double)(t - ServTrec);
					PM_count2++;
					if (pTestOutsw == 1) {
						pTestOutW = fopen("D:\\[14] Revision\\outputs\\[1]W.txt", "a");
						fprintf(pTestOutW, "%d\n", (t - ServTrec));
						fclose(pTestOutW);
					}
					if (RemainingToA_slot == 1)p_ED[CurrentTxFrameID]->Econsm += Erx;
					p_ED[CurrentTxFrameID]->Estate = 0;
					CHstatus = 0;
					CurrentTxFrameID = -999;
					RemainingToA = -999;
					RemainingToA_slot = -999;
					totDepPck++;
				}
			}
		}
		else if (p_GW[i]->Queue[0] > 0 && CHstatus == 0 && CurrentTxFrameID == -999) {
			temp = NumED;
			while (temp == NumED) {
				temp = (double)rand() / (double)RAND_MAX*(double)NumED;
			}
			CurrentTxFrameID = (int)temp;
			ServTrec = p_GW[i]->Queue[0];
			PM_Table[0] += (double)(t - p_GW[i]->Queue[0]);
			PM_count++;
			if (pTestOutsw == 1) {
				pTestOutWq = fopen("D:\\[14] Revision\\outputs\\[1]Wq.txt", "a");
				fprintf(pTestOutWq, "%d\n", (t - p_GW[i]->Queue[0]));
				fclose(pTestOutWq);
			}
			for (j = 0; j < QueueLength; j++) {
				if (j < (QueueLength - 1)) p_GW[i]->Queue[j] = p_GW[i]->Queue[j + 1];
				else p_GW[i]->Queue[j] = 0;
				loopcheck++;
				if (loopcheck > 1000)printf("\nLooooooop");
			}
			if (p_GW[i]->NumFrameInQ == QueueLength) QueueBlock = 0;
			
			if (t == p_ED[CurrentTxFrameID]->pingSlot) {
				//p_ED[CurrentTxFrameID]->ValNumPing++;
				p_ED[CurrentTxFrameID]->Estate = 1;
				RemainingToA = pingTable[2][p_ED[CurrentTxFrameID]->SF_ID];
				RemainingToA_slot = ceil((double)RemainingToA / (double)UnitTime);
				if (RemainingToA_slot == 0) RemainingToA_slot = 1;
				CHstatus = 1;
				RemainingToA_slot--;
				if (RemainingToA_slot == 0) {
					PM_Table[2] += (double)(t-ServTrec);
					PM_count2++;
					if (pTestOutsw == 1) {
						pTestOutW = fopen("D:\\[14] Revision\\outputs\\[1]W.txt", "a");
						fprintf(pTestOutW, "%d\n", (t - ServTrec));
						fclose(pTestOutW);
					}
					if (RemainingToA_slot == 1)p_ED[CurrentTxFrameID]->Econsm += Erx;
					p_ED[CurrentTxFrameID]->Estate = 0;
					CHstatus = 0;
					CurrentTxFrameID = -999;
					RemainingToA = -999;
					RemainingToA_slot = -999;
					totDepPck++;
				}
			}
		}
		else {
			/* Do not anything */
		}
	}
}

void Energy(ED *p_ED[NumED], int t) {
	int i;
	for (i = 0; i < NumED; i++) {
		if (p_ED[i]->Estate == 1 || t == p_ED[i]->pingSlot) {
			p_ED[i]->Econsm += Erx;
		}
		else {
			p_ED[i]->Econsm += Eidle;
		}
	}
}

void UpdatePingSlot(ED *p_ED[NumED], int t) {
	int i;
	for (i = 0; i < NumED; i++) {
		if (t == p_ED[i]->pingSlot) {
			p_ED[i]->pingSlot += p_ED[i]->pingPeriod;
		}
	}
}

void PerformanceMetric(GW *p_GW[NumGW], int t, int iter, ED *p_ED[NumED]) {
	int i; int j; int cnt=0; double rmFrame=0;
	int Qstate; int tempQ; int tempS;
	 /* Number of frames in queue */
	for (i = 0; i < NumGW; i++) {
		tempQ = 0;
		tempS = 0;
		for (j = 0; j < QueueLength; j++) {
			if (p_GW[i]->Queue[j] > 0 && t <= EndTime2) {
				PM_Table[1]++;
				PM_Table[3]++;
				tempQ++;
				tempS++;
			}
		}
	}
	if (CHstatus == 1 || CurrentTxFrameID != -999) {
		if (t <= EndTime2) {
			PM_Table[3]++;
			tempS++;
		}
	}

	if (pTestOutsw == 1) {
		if (t == 0 || iter == 0) {
			printf("\n0%%		  100%%\n");
			//printf("бс");
			Prg = testNumPck / 10;
		}
		if (totDepPck == Prg) {
			printf("бс");
			Prg += testNumPck / 10;
		}
	}

	/*pTestOutL = fopen("D:\\[14] Revision\\outputs\\[1]L.txt", "a");
	fprintf(pTestOutL, "%d\n", tempS);
	if (totDepPck >= testNumPck) {
		fprintf(pTestOutL, "simulation time: %d\n", t);
	}
	fclose(pTestOutL);*/

	if (totDepPck >= testNumPck) {
		if (pTestOutsw == 1) {
			//printf("\nSimulation Time: %d", t);
			//printf("\nTotal Number of Frames in Queue: %f", PM_Table[1]);
			//printf("\nTotal Number of Frames in System: %f", PM_Table[3]);
			pTestOutLq = fopen("D:\\[14] Revision\\outputs\\[1]Lq.txt", "a");
			fprintf(pTestOutLq, "%f %f\n", PM_Table[1] / (double)t, PM_Table[3] / (double)t);
			fclose(pTestOutLq);
			system("pause");
		}
	}

	if (t == EndTime) {
		Qstate = 0;
		for (i = 0; i < NumGW; i++) {
			for (j = 0; j < QueueLength; j++) {
				if (p_GW[i]->Queue[j] == 1) {
					Qstate++;
					printf("Not empty\n");
				}
			}
		}
		//TotalPingSlot=0;
		//TotalValPingSlot=0;
		EperS = 0;
		for (i = 0; i < NumED; i++) {
			//TotalPingSlot += p_ED[i]->TotNumPing;
			//TotalValPingSlot += p_ED[i]->ValNumPing;
			EperS += p_ED[i]->Econsm;

		}
		EperS /= (double)NumED;
		EperS /= (double)(EndTime / 30);
		EperS *= 30;
		//printf("\n%f %f", TotalValPingSlot, TotalPingSlot);
		//ValidPingRatio = TotalValPingSlot / TotalPingSlot;
		if (PRTswitch == 1 && Qstate == 0) {
			if (pResWtNfsw == 1)pResWtNf1 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.05.txt", "a");
			else if (pResWtNfsw == 2)pResWtNf2 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.10.txt", "a");
			else if (pResWtNfsw == 3)pResWtNf3 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.15.txt", "a");
			else if (pResWtNfsw == 4)pResWtNf4 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.20.txt", "a");
			else if (pResWtNfsw == 5)pResWtNf5 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.25.txt", "a");
			else if (pResWtNfsw == 6)pResWtNf6 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.30.txt", "a");
			else if (pResWtNfsw == 7)pResWtNf7 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.35.txt", "a");
			else if (pResWtNfsw == 8)pResWtNf8 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.40.txt", "a");
			else if (pResWtNfsw == 9)pResWtNf9 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.45.txt", "a");
			else if (pResWtNfsw == 10)pResWtNf10 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.50.txt", "a");
			else if (pResWtNfsw == 11)pResWtNf11 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.55.txt", "a");
			else if (pResWtNfsw == 12)pResWtNf12 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.60.txt", "a");
			else if (pResWtNfsw == 13)pResWtNf13 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.65.txt", "a");
			else if (pResWtNfsw == 14)pResWtNf14 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.70.txt", "a");
			else if (pResWtNfsw == 15)pResWtNf15 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.75.txt", "a");
			else if (pResWtNfsw == 16)pResWtNf16 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.80.txt", "a");
			else if (pResWtNfsw == 17)pResWtNf17 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.85.txt", "a");
			else if (pResWtNfsw == 18)pResWtNf18 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.90.txt", "a");
			else if (pResWtNfsw == 19)pResWtNf19 = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_0.95.txt", "a");
			else pRewWtNfCheck = fopen("D:\\[14] Revision\\outputs\\[2]WtNf_check.txt", "a");
			/* Waiting time in queue */
			if (PM_Table[0] > 0) {
				PM_Table[0] = PM_Table[0] / (double)PM_count / 1000.0;
			}
			else PM_Table[0] = 0;
			//printf("\n%.4f ", PM_Table[0]);
			/* Number of frames in queue */
			PM_Table[1] = PM_Table[1] / (double)(EndTime2/30);
			//printf("%.2f ", PM_Table[1]);
			/* Waiting time in system */
			if (PM_Table[2] > 0) {
				PM_Table[2] = PM_Table[2] / (double)PM_count2 / 1000.0;
			}
			else PM_Table[2] = 0;
			//printf("%.4f ", PM_Table[2]);
			/* Number of frames in queue */
			PM_Table[3] = PM_Table[3] / (double)(EndTime2/30);
			//printf("%.2f ", PM_Table[3]);
			//printf("\n");
			if (pResWtNfsw == 1) {
				fprintf(pResWtNf1, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf1);
			}
			else if (pResWtNfsw == 2) {
				fprintf(pResWtNf2, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf2);
			}
			else if (pResWtNfsw == 3) {
				fprintf(pResWtNf3, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf3);
			}
			else if (pResWtNfsw == 4) {
				fprintf(pResWtNf4, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf4);
			}
			else if (pResWtNfsw == 5) {
				fprintf(pResWtNf5, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf5);
			}
			else if (pResWtNfsw == 6) {
				fprintf(pResWtNf6, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf6);
			}
			else if (pResWtNfsw == 7) {
				fprintf(pResWtNf7, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf7);
			}
			else if (pResWtNfsw == 8) {
				fprintf(pResWtNf8, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf8);
			}
			else if (pResWtNfsw == 9) {
				fprintf(pResWtNf9, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf9);
			}
			else if (pResWtNfsw == 10) {
				fprintf(pResWtNf10, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf10);
			}
			else if (pResWtNfsw == 11) {
				fprintf(pResWtNf11, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf11);
			}
			else if (pResWtNfsw == 12) {
				fprintf(pResWtNf12, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf12);
			}
			else if (pResWtNfsw == 13) {
				fprintf(pResWtNf13, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf13);
			}
			else if (pResWtNfsw == 14) {
				fprintf(pResWtNf14, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf14);
			}
			else if (pResWtNfsw == 15) {
				fprintf(pResWtNf15, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf15);
			}
			else if (pResWtNfsw == 16) {
				fprintf(pResWtNf16, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf16);
			}
			else if (pResWtNfsw == 17) {
				fprintf(pResWtNf17, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf17);
			}
			else if (pResWtNfsw == 18) {
				fprintf(pResWtNf18, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf18);
			}
			else if (pResWtNfsw == 19) {
				fprintf(pResWtNf19, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pResWtNf19);
			}
			else {
				fprintf(pRewWtNfCheck, "%f %f %f %f %f\n", PM_Table[0], PM_Table[1], PM_Table[2], PM_Table[3], EperS);
				fclose(pRewWtNfCheck);
			}
			PRTswitch = 1;
		}
		//printf("%d %d %d\n", totArrPck, PM_count, PM_count2);
	}
}

#endif