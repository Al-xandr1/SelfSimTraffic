#include "math.h"

class TrafficHistogram {
	private:
		double* histogram;				//�������� �����������
		int numHistCellsForTraffic;		//���������� ���������� ��� �����������
		int allowedMaxTrafficValue;		//������������ ����������� �������� ������� (� ������)
		double widthOfCell;				//������ ������
		long packetCount;				//���������� �������, �������� � ��������

		long overflowPackets;			//���������� �������, �������� �� ������������ ��������
		long underflowPackets;			//���������� ������, �������� �� ������������ �������� (������������ ����� ������)

		//��������� ��������� �������� (value) � ���������� �������� [leftIndex, leftIndex + widthOfCell). 
		int isIntoRange(int leftIndex, double value){
			int rightIndex = leftIndex + 1;

			double leftEnd = leftIndex * widthOfCell;
			double rightEnd = rightIndex * widthOfCell;
	
			bool rightBound;
			if (rightIndex == numHistCellsForTraffic){//�������� ������ ����� ������� ��� ���
				rightBound = value <= rightEnd;
			} else{
				rightBound = value < rightEnd;
			}

			if ( leftEnd <= value  &&  rightBound ){
				return 0;
			} else if (value < leftEnd){
				return -1;
			} else if (rightEnd <= value){
				return 1;
			}
		};
		//����� �������� � �����������. return 0, ���� ����� ���������������� � �����������, � -1 �����
		int recursivePut(int trafficVal, int index){
			if (index < 0){
				underflowPackets++;
				return -1;
			}
			if (index > numHistCellsForTraffic-1){
				overflowPackets++;
				return -1;
			}

			int res = isIntoRange(index, trafficVal);
			if (res == 0){
				histogram[index]++;
				packetCount++;
				return index;
			} else if(res < 0){//��� �����				
				return recursivePut(trafficVal, index--);//index-1
			} else{//��� ������
				return recursivePut(trafficVal, index++);//index+1
			}
		};

		int initializeStratIndex(long trafficVal){
			return (int) floor(1.0 * trafficVal / widthOfCell);
		}
		
	public:
		TrafficHistogram(int numHistCells, int maxTrafficValue){
			numHistCellsForTraffic = numHistCells;
			allowedMaxTrafficValue = maxTrafficValue;
			widthOfCell = 1.0 * allowedMaxTrafficValue / numHistCellsForTraffic;
			packetCount = 0;
			overflowPackets = 0;
			underflowPackets = 0;

			histogram = new double[numHistCellsForTraffic];
			for (int i=0 ; i<numHistCellsForTraffic ; i++){
				histogram[i] = 0;
			}
		};

		~TrafficHistogram(){
			delete[] histogram;
		};

		//return 0 ���� ����� ���������������� � �����������, -1 �����
		int putInHist(int trafficVal){
			return recursivePut(trafficVal, initializeStratIndex(trafficVal));
		};

		double calculateCheckSum(){
			double checkSum = 0;
			for(int i=0; i<numHistCellsForTraffic; checkSum += getHistValue(i++));
			return checkSum;
		};

		double getHistValue(int index) {return histogram[index] / packetCount;}
		int getNumHistCells() {return numHistCellsForTraffic;} 
		int getAllowedMaxTrafficValue() {return allowedMaxTrafficValue;}
		double getWidthOfCell() {return widthOfCell;}
		long getOverflowPackets() {return overflowPackets;}
		long getUnderflowPackets() {return underflowPackets;}
};