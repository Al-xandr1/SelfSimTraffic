// HistogramCreator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fstream"
#include "iostream"
#include "string"
#include "math.h"
#include "TrafficHistogram.cpp"
using namespace std;


int main(int argc, char** argv)
{
	if(argc != 4)
	{	
		printf("Wrong count of parameters.\n");
		return -1;
	}

	//---------Чтение параметров из командной строки-------
	char* inFileName = argv[1];
	char outFileName[256];

	strcat(strcpy(outFileName, inFileName), ".HIST.xml");
	
	//построение гистограммы в диапазоне [0, MAX_TRAFFIC_VAL].
	//MAX_TRAFFIC_VAL - кол-во byte
	double MAX_TRAFFIC_VAL = atoi(argv[2]);
	//RANGES_COUNT - число интервалов на отрезке
	int RANGES_COUNT = atoi(argv[3]);

	printf("\ninput_Filename ='%s'\n", inFileName);
	printf("output_Filename='%s'\n", outFileName);

	//-------Чтение из файла TL-----------------------------------
	ifstream inFileTL;//входной файл *.TL

	inFileTL.open(inFileName);
	if (!inFileTL.is_open()) {
		printf("Unable to open input TL file\n");
		return -1;
	}
	
	TrafficHistogram histogram(RANGES_COUNT, MAX_TRAFFIC_VAL);
	double time = -1;
	long trafficVal = -1;

	//ДУБЛИРУЕТСЯ ПОСЛЕДНЯЯ СТРОКА!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while(!inFileTL.eof()){
		inFileTL >> time;
		inFileTL >> trafficVal;
		
		histogram.putInHist(trafficVal);
    }
	inFileTL.close();

	//------Вывод в XML файл------------------------------------------
	ofstream outFileXML;//выходной файл *.TL.xml

	outFileXML.open("Histogram.xml");
	if (!outFileXML.is_open()) {
		printf("Unable to open or create output XML file\n");
		return -1;
	}

	outFileXML <<"<!--  Source file: " << inFileName << "  -->  "<<endl<<endl;
	outFileXML <<"<?xml version=\'1.0' ?>"<<endl<<endl;	
    outFileXML <<"<HISTOGRAM>"<<endl;
	outFileXML <<"  <ALLOWED-MAX-TRAFFIC-VALUE> "<< histogram.getAllowedMaxTrafficValue() <<" </ALLOWED-MAX-TRAFFIC-VALUE>"<<endl;
	outFileXML <<"  <NUM-HIST-CELLS-FOR-TRAFFIC> "<< histogram.getNumHistCells() <<" </NUM-HIST-CELLS-FOR-TRAFFIC>"<<endl;
	outFileXML <<"  <WIDTH-OF-CELL> "<< histogram.getWidthOfCell() <<" </WIDTH-OF-CELL>"<<endl;
	outFileXML <<"  <HIST-POINTS>";
	for (int i = 0; i < RANGES_COUNT ; i++){
		outFileXML << histogram.getHistValue(i) << "  ";
	}
	outFileXML <<endl<<"</HIST-POINTS>"<<endl;
	outFileXML <<"  <CHECK-SUM> "<< histogram.calculateCheckSum() <<" </CHECK-SUM>"<<endl;
	outFileXML <<"  <OVERFLOW-PACKETS> "<< histogram.getOverflowPackets() <<" </OVERFLOW-PACKETS>"<<endl;
	outFileXML <<"  <UNDERFLOW-PACKETS> "<< histogram.getUnderflowPackets() <<" </UNDERFLOW-PACKETS>"<<endl;
    outFileXML <<"</HISTOGRAM>"<<endl;
    outFileXML.close();

	return 0;
}


