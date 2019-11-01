#include <stdio.h>
#include <iostream>

#ifndef _NODEINFO_
#define _NODEINFO_
#define NumGW 1
#define NumED 100
#define SimIteration 100

int PRTswitch = 1;
int MODE_WtNfCheckMode = 0;
int TestOutsw = 0;

int PingSlotParameter = 1;

double TestVal = 0.95;
int testNumPck = 10000;

int loopcheck;
int UnitTime = 30;    //30ms
/*Termination time of simulation. (1h=120000*30ms).*/
const int EndTime2 = UnitTime * 1200000;
//const int EndTime2 = UnitTime * 3000000;
const int EndTime = EndTime2 * 2;
const int QueueLength = 160;
int QueueBlock;
double ArribalRateTable[2][19];
//double ArrivalRate = ArribalRateTable[1][0];    //[frame/ms], (1[frame/s] = 0.001[frame/ms])
//double ArrivalMean = ArrivalRate*(double)UnitTime;
double ArrivalRate;    //[frame/ms], (1[frame/s] = 0.001[frame/ms])
double ArrivalMean;
int ArrivalInterval;
int ArrivalTime;
double tempPoss[11];
int prmax;
const int NumSF = 7;    //Number of spreading factors
double PM_Table[4];    //Performance metric - Waiting time/# of frames in queue, Waiting time/# of frames in system.
int ServTrec;
int PM_count, PM_count2;
int iter2max;
int Prg;

/* Structure Setting */
struct GW {
	double GW_Loc_x; double GW_Loc_y;
	double GW_Cover;    //coverage, 26.551km
	int Queue[QueueLength];
	/*	Queue:	[Timing Stamp]	*/
	int NumFrameInQ;
};
struct ED {
	double ED_Loc_x; double ED_Loc_y;
	double DistanceToGW;
	double SNR;
	int SF_ID;
	int SF;
	int pingPeriod;
	int pingPeriod_slot;
	int pingSlot;
	int pingOffset_slot;
	int TotNumPing;
	int ValNumPing;
	double Econsm;
	double Estate;
};
double sig = -1;
double mean = 0;
double sigma = 3.57;
double GatewayCoverage = -20;
/*double DistSet[7] = { 3647.3,    6210.6,   14667.3,    8154.1,    8437.7,   15423.7,   11893.9};*/
double SNRth[7] = { -5, -7.5, -10, -12.5, -15, -17.5, -20};
double RefRange = 1000; // the reference distance[m]
double gamma = 2.08; // the path loss exponent[dB]
double PL0 = 127.41; //the path loss exponent at the referent distance d0[dB]
double xaxis = 15000;  // the maximum coverage distance
double yaxis = xaxis;
double xmin = sig * xaxis;
double xmax = xaxis;
double ymin = sig * yaxis;
double ymax = yaxis;
double Ptx = sig*16; // the transmission power[dB]
double GL = 0; // the gainand loss
double BW = 125;  // bandwidth[kHz]
#if 1
int pingTable[3][NumSF] = {
	{ 6,7,8,9,10,11,12 },//Spreading Factor(SF).
	{ 1,2,3,4,5,6,7 }, //Parameter to determine number of ping slots.
	{ 21,41,72,144,289,578,991 }    //Shortest Time-on-Air(ms).
};
#endif
double ESset[7] = { 31325, 15965, 8285, 4445, 2525, 1565, 1085};
int CHstatus;    //Channel status.
int CurrentTxFrameID;    //ID of destination end-device on current transmitting frame.
int RemainingToA;    //Remaining Time-on-Air.
int RemainingToA_slot;
int totArrPck;	//Total number of arrival packets.
int totDepPck;	//Total number of departure packets.
double TotalPingSlot;
double TotalValPingSlot;
double ValidPingRatio;
double Erx = 31.8;	//mA/ms
double Eidle = 0.045;	//mA/ms
double EperS;

#endif