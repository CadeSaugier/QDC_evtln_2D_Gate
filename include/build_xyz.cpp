//Build .xyz Histogram File from .evtln Data File.
//Program by: Cade Saugier, INPP EAL Ohio University

/*
This C++ code is open source.
Do whatever you want with it.
I don't mind.

For support: Go to room 115 EAL.
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using std::ifstream;
using std::ofstream;
using std::string;
using std::to_string;
using std::stoi;
using std::cout;
using std::endl;

///////////////////////////////////// Build Functions ////////////////////////////////////
int get(string line, int stop)
{
	int i=0, k=0, last=0;
	while(k<stop and k!=4)
	{
		if(line[i]==',' and k!=stop-1) {k++; last=i;}
		else if(line[i]==',') {k++;}
		i++;
	}
	int adj=0;
	if(stop>1) {adj+=1;}
	return stoi(line.substr(last+adj,i-last-1-adj));
}

/////////////////////////////////////// Build Main ///////////////////////////////////////
int main(int argc, char* argv[])
{
	//Record Input Values >>File Y vs X Cut
	int pos=1;
	string read;
	string dataFilePath = argv[pos];
	if(dataFilePath.substr(dataFilePath.length()-6,6)!=".evtln")
	{
		cout << "--->File Type Not .evtln!\n";
		return -1;
		exit(0);
	}
	ifstream dataFile;
	dataFile.open(dataFilePath);
	if(dataFile.is_open())
	{
		cout << "\n--->File Open at " << dataFilePath << endl;
	}
	else
	{
		cout << "\n--->Error Opening File!\n";
		exit(0);
	}
	string dataFileName;
	int fileMarker, lastDir=0, val=0;
	for(int i=0; i<dataFilePath.length(); i++)
	{
		if(dataFilePath[i]=='/') {lastDir=i; val=1;}
		if(dataFilePath[i]=='.') {fileMarker=i;}
	}
	dataFileName=dataFilePath.substr(lastDir+val,fileMarker-lastDir-val);
	pos++;
	string xDataName, yDataName;
	yDataName=argv[pos];
	pos++;
	xDataName=argv[pos];
	pos++;
	read=argv[pos];
	int cut=stoi(read);
	
	//Determine Requested Argument Values
	int xStop, yStop;
	if(xDataName=="short") {xStop=1;}
	else if(xDataName=="long") {xStop=2;}
	else if(xDataName=="psd") {xStop=3;}
	else if(xDataName=="tof") {xStop=4;}
	else if(xDataName=="chan") {xStop=5;}
	if(yDataName=="short") {yStop=1;}
	else if(yDataName=="long") {yStop=2;}
	else if(yDataName=="psd") {yStop=3;}
	else if(yDataName=="tof") {yStop=4;}
	else if(yDataName=="chan") {yStop=5;}
	cout << "\n" << "--> Y-Data: " << yDataName << " ==>> " << yStop << endl;
	cout << "--> X-Data: " << xDataName << " ==>> " << xStop << endl;
	
	//Build Dynamic Memory
	int size=65536/cut;
	cout << "\n--->Building Dynamic Memory For Matrix Size " << size << "x" << size << "...\n";
	int** dataMat=new int*[size];
	for(int i=0; i<size; i++)
	{
		dataMat[i]=new int[size];
	}
	cout << "--->Initializing Zero Matrix...\n";
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<size; j++)
		{
			dataMat[i][j]=0;
		}
	}
	
	//Main Loop
	int valueX, valueY;
	dataFile >> read;
	dataFile >> read;
	cout << "--->Replaying Event Line File...\n";
	while(!dataFile.eof())
	{
		dataFile >> read;
		valueX=static_cast<int>(get(read,xStop)/cut);
		valueY=static_cast<int>(get(read,yStop)/cut);
		dataMat[valueY][valueX]++;
	}
	
	//Close Data File
	dataFile.close();
	
	//Write Output File
	system(("mkdir ./output_"+dataFileName).c_str());
	cout << "--->Building XYZ File...\n";
	ofstream outFile;
	outFile.open("./output_"+dataFileName+"/"+dataFileName+"_"+yDataName+"-"+xDataName+".xyz");
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<size; j++)
		{
			outFile << to_string(i) << " " << to_string(j) << " " << to_string(dataMat[i][j]) << "\n";
		}
	}
	outFile.close();
	
	//Clean Up
	cout << "--->Cleaning Up Dynamic Memory...\n\n"; 
	for(int i=0; i<size; i++)
	{
		delete[] dataMat[i];
		dataMat[i]=nullptr;
	}
	delete[] dataMat;
	dataMat=nullptr;
	
	return 0;
}
