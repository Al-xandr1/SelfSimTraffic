// PacketAnalyser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment(lib, "wpcap.lib")
#include <pcap/pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <conio.h>
using namespace std;

FILE *outputBigDumpFP = NULL;
FILE *outputFragDumpFP = NULL;// output fragment dump file
int MAX_ROW_COUNT = -1;

void dispatcher_handler(u_char *, const struct pcap_pkthdr *, const u_char *);

int main(int argc, char** argv)
{
	if(argc != 3)
	{	
		printf("Wrong count of parameters.\n");
		return -1;
	}

	//---------Чтение параметров из командной строки-------
	char* inputDumpFileName = argv[1];
	char outputDumpFileName[256];
	strcat(strcpy(outputDumpFileName, inputDumpFileName), ".TL");
	MAX_ROW_COUNT = atoi(argv[2]);
	printf("\ninput_Filename ='%s'\n", inputDumpFileName);
	printf("output_Filename='%s'\n", outputDumpFileName);

	//---------------------------------------------------
	pcap_t *inputDumpFP;
	char errbuf[PCAP_ERRBUF_SIZE];
	
	/* Open the capture file */
	if ((inputDumpFP = pcap_open_offline(inputDumpFileName, errbuf)) == NULL)
	{
		printf("\nUnable to open the file '%S'.\n", inputDumpFileName);
		printf("\nReason: %s\n", errbuf);
		return -2;
	}

	outputBigDumpFP = fopen(outputDumpFileName, "w");
	int eof = 1;	
	int numberOfPart = 1;
	char strEnd[5] = {'0', '0', '0', '0', '0'};

	/* read and dispatch packets until EOF is reached */
	while (eof == 1)
	{
		strcat(_itoa(numberOfPart, strEnd, 10), ".TL");
		outputFragDumpFP = fopen(strcat(strcpy(outputDumpFileName, inputDumpFileName), strEnd), "w");
		for (int i = 0; i < MAX_ROW_COUNT && eof == 1; i++)
		{
			eof = pcap_dispatch(inputDumpFP, 1, dispatcher_handler, NULL);
		}
		fclose(outputFragDumpFP);
		numberOfPart++;
	}

	pcap_close(inputDumpFP);
	fclose(outputBigDumpFP);
	return 0;
}

void dispatcher_handler(u_char *temp1, const struct pcap_pkthdr *header, const u_char *pkt_data)
{	
	time_t local_tv_sec;
	struct tm ltime;
	char timestr[16];
	// convert the timestamp to readable format 
    local_tv_sec = header->ts.tv_sec;
    localtime_s(&ltime, &local_tv_sec);
    strftime(timestr, sizeof timestr, "%S", &ltime);
    fprintf(outputBigDumpFP, "%s.%.6d %d\n", timestr, header->ts.tv_usec, header->len);
    fprintf(outputFragDumpFP, "%s.%.6d %d\n", timestr, header->ts.tv_usec, header->len);
}