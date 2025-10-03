//Gate QDC .evtln Event Data Based on 2D Histogram Plot of Y-Data vs X-Data
//Program by: Cade Saugier, INPP EAL Ohio University

/*
This C++ code is open source.
Do whatever you want with it.
I don't mind.

For support: Go to room 115 EAL or E-mail cs192623@ohio.edu
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include "include/histxy.h"

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

int find(string name)
{
	int send;
	if(name=="short") {send=1;}
	else if(name=="long") {send=2;}
	else if(name=="psd") {send=3;}
	else if(name=="tof") {send=4;}
	else if(name=="chan") {send=5;}
	return send;
}


/////////////////////////////////////// Build Main ///////////////////////////////////////
int main(int argc, char* argv[])
{
	cout << "\n<><><><><><><><><><> 2D HISTOGRAM GATE <><><><><><><><><><>\n";
	cout << "\t--->Version 1.3.3\n";
	cout << "\t--->By: Cade S.\n\n";
	
	//Record Inputs
		string fileLoc, yName, xName;
		fileLoc=argv[1];
		yName=argv[2];
		xName=argv[3];
		int xID=find(xName);
		int yID=find(yName);
	
	//Get File Name
		string fileName;
		int fileMarker, lastDir=0, val=0;
		for(int i=0; i<fileLoc.length(); i++)
		{
			if(fileLoc[i]=='/') {lastDir=i; val=1;}
			if(fileLoc[i]=='.') {fileMarker=i;}
		}
		fileName=fileLoc.substr(lastDir+val,fileMarker-lastDir-val);
	
	//Display
		cout << "\n\n>>> Gating File " << fileName << " via " << yName << " vs " << xName << endl;
	
	//Ask for .xyz File
		int cut=256;
		int check=system(("./include/build_xyz "+fileLoc+" "+yName+" "+xName+" "+to_string(cut)).c_str());
		if(check!=0) {cout << ">>> Build Error!\n"; exit(0);}
	
	//Ask for 2D Gating
		cout << "\n\n>>> Please Select Region of Interest...\n";
		system(("python3 ./include/grab2d.py ./output_"+fileName+"/"+fileName+"_"+yName+"-"+xName+".xyz "+yName+" "+xName+" "+fileName).c_str());
	
	//Build Truth Matrix
		cout << "\n>>> Building Truth Table in RAM...\n";
		ifstream truthFile;
		truthFile.open("./output_"+fileName+"/xy.truth");
		bool** truthMat=new bool*[65536];
		for(int i=0; i<65536; i++)
		{
			truthMat[i]=new bool[65536];
		}
		for(int i=0; i<65536; i++)
		{
			for(int j=0; j<65536; j++)
			{
				truthMat[i][j]=false;
			}
		}
		string read;
		int a, b;
		a=0;
		b=0;
		cout << ">>> Organizing Truth Table...\n";
		while(!truthFile.eof())
		{
			truthFile >> read;
			if(read=="-") {b++; a=0;}
			else
			{
				a=stoi(read);
				for(int w=0; w<cut; w++)
				{
					truthMat[a][b*cut+w]=true;
				}
			}
		}
		truthFile.close();
	
	//Build Histogram Objects
		histo longHist, shortHist, psdHist, tofHist, chanHist;
	
	//Replay Data
		cout << ">>> Filtering Data...\n\n";
		ifstream dataFile;
		dataFile.open(fileLoc);
		ofstream eventLine;
		eventLine.open("./output_"+fileName+"/"+fileName+"_"+yName+"-"+xName+".evtln");
		int save=0, abvsat=0, total=0;
		dataFile >> read;
		eventLine << read;
		dataFile >> read;
		eventLine << " " << read << "\n";
		dataFile >> read;
		int longcheck=get(read,2);
		while(!dataFile.eof())
		{
			if(longcheck>=65536)
			{
				abvsat++;
				total++;
				dataFile >> read;
				longcheck=get(read,2);
				continue;
			}
			if(truthMat[get(read,xID)][get(read,yID)])
			{
				longHist.plug(get(read,2));
				shortHist.plug(get(read,1));
				psdHist.plug(get(read,3));
				tofHist.plug(get(read,4));
				chanHist.plug(get(read,5));
				eventLine << read << "\n";
				save++;
			}
			total++;
			dataFile >> read;
			longcheck=get(read,2);
		}
		eventLine.close();
		dataFile.close();
		cout << "Selected Events\t=== " << save << endl;
		cout << "Above Saturation=== " << abvsat << endl;
		cout << "Total Events\t=== " << total << endl;
	
	//Build .xy Files
		cout << "\n>>> Building Histogram .xy Files...";
		ofstream longFile, shortFile, psdFile, tofFile, chanFile;
		longFile.open("./output_"+fileName+"/long_gated-"+yName+"-"+xName+".xy");
		shortFile.open("./output_"+fileName+"/short_gated-"+yName+"-"+xName+".xy");
		psdFile.open("./output_"+fileName+"/psd_gated-"+yName+"-"+xName+".xy");
		tofFile.open("./output_"+fileName+"/tof_gated-"+yName+"-"+xName+".xy");
		chanFile.open("./output_"+fileName+"/chan_gated-"+yName+"-"+xName+".xy");
		for(int k=0; k<65536; k++)
		{
			longFile << to_string(k) << "  " << to_string(longHist.get(k)) << "\n";
			shortFile << to_string(k) << "  " << to_string(shortHist.get(k)) << "\n";
			psdFile << to_string(k) << "  " << to_string(psdHist.get(k)) << "\n";
			tofFile << to_string(k) << "  " << to_string(tofHist.get(k)) << "\n";
			chanFile << to_string(k) << "  " << to_string(chanHist.get(k)) << "\n";
		}
		longFile.close();
		shortFile.close();
		psdFile.close();
		tofFile.close();
		chanFile.close();
	
	//Clean Up
		cout << "\n\n>>> Cleaning Up...\n";
		system(("rm ./output_"+fileName+"/*.truth").c_str());
		for(int i=0; i<65536; i++)
		{
			delete[] truthMat[i];
			truthMat[i]=nullptr;
		}
		delete[] truthMat;
		truthMat=nullptr;
	
	cout << ">>> Done! ^_^\n";
	
	return 0;
}
