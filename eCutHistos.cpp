// C++ code to plot energy thresholds distributions for every ECAL ring (EB and EC)
// to compile: c++ -o eCutHistos `root-config --cflags --glibs` eCutHistos.cpp geometryUtils.cc
// to run: ./eCutHistos APDPN_list.txt alpha_list.txt IC_list.txt ADC_list.txt ChStatus_list.txt

using namespace std;

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <map>
#include "TFile.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TMath.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TTree.h"
#include "TStyle.h"
#include <vector>

#include "geometryUtils.h"

string uintToString (unsigned int);

int main(int argc, char** argv)
{

  //lists of input files
  char* inputLIST_APDPN = argv[1];
  char* inputLIST_alpha = argv[2];
  char* inputLIST_IC = argv[3];
  char* inputLIST_ADC = argv[4];
  char* inputLIST_ChStat = argv[5];
  
  std::string inputList_APDPN = std::string(inputLIST_APDPN);
  std::string inputList_alpha = std::string(inputLIST_alpha);
  std::string inputList_IC = std::string(inputLIST_IC);
  std::string inputList_ADC = std::string(inputLIST_ADC);
  std::string inputList_ChStat = std::string(inputLIST_ChStat);
   
  std::cout << "inputList_APDPN = " << inputList_APDPN << std::endl;
  std::cout << "inputList_alpha = " << inputList_alpha << std::endl;
  std::cout << "inputList_IC = " << inputList_IC << std::endl;
  std::cout << "inputList_ADC = " << inputList_ADC << std::endl;
  std::cout << "inputList_ChStat = " << inputList_ChStat << std::endl << std::endl;
  
  //files vectors for every list

  std::vector<std::string> inputFiles_APDPN;
  std::vector<std::string> inputFiles_alpha;
  std::vector<std::string> inputFiles_IC;
  std::vector<std::string> inputFiles_ADC;
  std::vector<std::string> inputFiles_ChStat;
 
  char dumps[500];
  FILE *f_dumps;

  f_dumps = fopen(inputList_APDPN.c_str(),"r");
  while(fscanf(f_dumps,"%s \n", dumps) !=EOF ){
    std::cout << "Reading file from APDPN_list: " << dumps << std::endl;
    std::string DUMPS = std::string(dumps);
    if(DUMPS.find("#") != std::string::npos) continue;
    inputFiles_APDPN.push_back(DUMPS);
  }  

  f_dumps = fopen(inputList_alpha.c_str(),"r");
  while(fscanf(f_dumps,"%s \n", dumps) !=EOF ){
    std::cout << "Reading file from alpha_list: " << dumps << std::endl;
    std::string DUMPS = std::string(dumps);
    if(DUMPS.find("#") != std::string::npos) continue;
    inputFiles_alpha.push_back(DUMPS);
  }  

  f_dumps = fopen(inputList_IC.c_str(),"r");
  while(fscanf(f_dumps,"%s \n", dumps) !=EOF ){
    std::cout << "Reading inputFile_IC: " << dumps << std::endl;
    std::string DUMPS = std::string(dumps);
    if(DUMPS.find("#") != std::string::npos) continue;
    inputFiles_IC.push_back(DUMPS);
  }  

  f_dumps = fopen(inputList_ADC.c_str(),"r");
  while(fscanf(f_dumps,"%s \n", dumps) !=EOF ){
    std::cout << "Reading file from ADC_list: " << dumps << std::endl;
    std::string DUMPS = std::string(dumps);
    if(DUMPS.find("#") != std::string::npos) continue;
    inputFiles_ADC.push_back(DUMPS);
  }  

  f_dumps = fopen(inputList_ChStat.c_str(),"r");
  while(fscanf(f_dumps,"%s \n", dumps) !=EOF ){
    std::cout << "Reading file from ChStatus_list: " << dumps << std::endl << std::endl;
    std::string DUMPS = std::string(dumps);
    if(DUMPS.find("#") != std::string::npos) continue;
    inputFiles_ChStat.push_back(DUMPS);
  }  

  
  //variables
  int ieta, iphi, iz, chStatus;
  long int rawid;
  float ADCAmp_b = 8.;
  float ADCAmp_e = 12.;
  float eCut_b = 0.;
  float eCut_e = 0.;
  static const int  kEndcEtaRings  = 39;
  static const int  kBarlRings  = 170;
  int cutChStatus = 3;
  TEndcapRegions* eeId = new TEndcapRegions();
  
  //make maps RawId --> ieta, iphi, iz  (ix, iy, iz)
  //make map RawId --> EB or EE (1 for EB, 0 for EE)
  std::map<long int,int> mapRawId_ieta; //ix for EE
  std::map<long int,int> mapRawId_iphi; //iy for EE
  std::map<long int,int> mapRawId_iz;
  std::map<long int,int> mapRawId_EB;
  
  char cieta[100], ciphi[100], ciz[100], crawid[100], cped12[100], crms12[100], cped6[100], crms6[100], cped1[100], crms1[100];
 
  std::ifstream infile("dump_EcalPedestals_hlt__since_00208206_till_00208206.dat");
  //  std::ifstream infile("/afs/cern.ch/work/f/fedmante/public/PhiSym_thr/dump_EcalPedestals_hlt__since_00208206_till_00208206.dat");
  int iline = 0;
  
  while(infile >> cieta >> ciphi >> ciz >> cped12 >> crms12 >> cped6 >> crms6 >> cped1 >> crms1 >> crawid) {

    iline++;
     
    if(iline > 1 && iline <= 61201){ //EB
      ieta = atoi(cieta);
      iphi = atoi(ciphi);
      iz = atoi(ciz);
      rawid = atol(crawid);
      
      mapRawId_ieta[rawid] = ieta;
      mapRawId_iphi[rawid] = iphi;
      mapRawId_iz[rawid] = iz;
      mapRawId_EB[rawid] = 1;    
    }
    
    else if(iline > 61201){ //EE
      ieta = atoi(cieta); //ix
      iphi = atoi(ciphi); //iy
      iz = atoi(ciz);
      rawid = atol(crawid);
      
      mapRawId_ieta[rawid] = ieta; //ix
      mapRawId_iphi[rawid] = iphi; //iy
      mapRawId_iz[rawid] = iz;
      mapRawId_EB[rawid] = 0;
    }
  }

  //DEBUG
  /*std::cout << "***test mapRawId_ieta" << std::endl;
  map<long int, int>::const_iterator p;
  for(p = mapRawId_ieta.begin(); p != mapRawId_ieta.end(); p++) {
    cout << p->first << endl;
    cout << p->second << endl;
    }*/
  
  std::cout << "Maps rawid --> coordinates, done" << std::endl;
  std::cout << "Map rawid --> EB, done" << std::endl << std::endl;
 
  std::cout << "LOOP OVER THE APDPN FILES LIST:" << std::endl;
  
  for (unsigned int ii = 0; ii < inputFiles_APDPN.size(); ii++) { //loop over the apdpn list
    
    std::cout << ">>>>>> Loop number: " << ii+1 << std::endl;    
    std::cout << "Reading inputFile_APDPN: " << inputFiles_APDPN.at(ii) << std::endl;
    std::cout << "Reading inputFile_alpha: " << inputFiles_alpha.at(ii) << std::endl;
    std::cout << "Reading inputFile_ADC: " << inputFiles_ADC.at(ii) << std::endl;
    std::cout << "Reading inputFile_IC: " << inputFiles_IC.at(ii) << std::endl;
    std::cout << "Reading inputFile_ChStat: " << inputFiles_ChStat.at(ii) << std::endl;
 
    //create the output file
    std::string str = uintToString(ii+1);
    TFile *outputFile = new TFile (("ESpectra_" + str + ".root").c_str(),"RECREATE");
    std::cout << "New file created: " << "ESpectra_" << str << ".root" << std::endl; 
  
    //create spectra
    std::vector<TH1F*> eCut_spectrum_b_histos;
    std::vector<TH1F*> eCut_spectrum_e_histos;

    eCut_spectrum_b_histos.resize(kBarlRings);
    eCut_spectrum_e_histos.resize(kEndcEtaRings);

    ostringstream t;
    for (int i=0; i<kBarlRings; i++) { //EB
      if ( i < 85) { //EB-
	t << "eCut_spectrum_b_" << i-85;
	eCut_spectrum_b_histos[i]=new TH1F(t.str().c_str(),";E_{CUT} [MeV]",50,0.,500.); //number of bins?
	t.str("");
      }

      if ( i >= 85) { //EB+
	t << "eCut_spectrum_b_" << i-84;
	eCut_spectrum_b_histos[i]=new TH1F(t.str().c_str(),";E_{CUT} [MeV]",50,0.,500.); //number of bins?
	t.str("");
      }
    }

    for (int i=0; i<kEndcEtaRings; i++) { //EE
      t << "eCut_spectrum_e_" << i+1;
      eCut_spectrum_e_histos[i]=new TH1F(t.str().c_str(),";E_{CUT} [MeV]",75,0.,1500.); //number of bins?
      t.str("");
    }
 
    std::cout << "Spectra created" << std::endl; 
     
    //read the Channel Status file and create ChStatus map: coord-->channel status
    std::map<int,std::map<int,std::map<int,int> > > ichStatus;
    std::ifstream infileChStatus(inputFiles_ChStat.at(ii).c_str());
    std::cout << "Reading Channel Status file" << std::endl;
    while(infileChStatus >> ieta >> iphi >> iz >> chStatus) {
      ichStatus[ieta][iphi][iz]=chStatus; //for EC ieta =ix, iphi=iy
    }
    
    //get ADCToGeV corrections (costants)
    FILE *fADC;
    fADC = fopen (inputFiles_ADC.at(ii).c_str(),"r");
    std::string s1,s2;
    float ADCToGeV_b, ADCToGeV_e;
    std::cout << "Reading ADCToGeV file:" << std::endl; 
    //fscanf(fADC,"%s %f  %s %f \n",&s1,&ADCToGeV_b,&s2,&ADCToGeV_e); // <<<<<----------------- PROBLEM HERE!
    ADCToGeV_b = 0.039610;
    ADCToGeV_e = 0.067230;
    std::cout << "ADCToGeV constant for EB = " <<  ADCToGeV_b << ", ADCToGeV constant for EE = " << ADCToGeV_e << std::endl;
   
    //read the IC file and create IC map: coord-->IC
    std::map<int,std::map<int,std::map<int,float> > > ICmap;
    std::ifstream infileIC(inputFiles_IC.at(ii).c_str());  
    float IC;
    while(infileIC >> ieta >> iphi >> iz >> IC) {
      ICmap[ieta][iphi][iz]=IC; //for EC ieta =ix, iphi=iy 
      //std::cout << "Filling the IC map: " << ieta << ", " << iphi << ", " << iz << " --> " << ICmap[ieta][iphi][iz] << std::endl;
    }
    
    //read the alpha file and create alpha map: coord --> alpha
    std::map<int,std::map<int,std::map<int,float> > > alphaMap; 
    std::ifstream infileAlpha(inputFiles_alpha.at(ii).c_str());
    float alpha;
    std::cout << "Reading alpha file" << std::endl;
    while(infileAlpha >> ieta >> iphi >> iz >> alpha) {
      alphaMap[ieta][iphi][iz]=alpha; //for EC ieta =ix, iphi=iy 
    }
    
    //read APDPN file and create apdpnratio map: rawid --> apdpnratio
    std::map<long int,float> apdpnMap;
    FILE *fAPDPN;
    fAPDPN = fopen (inputFiles_APDPN.at(ii).c_str(),"r");
    std::cout << "Reading APDPN file" << std::endl;
    int BUF = 5000;
    char line [ BUF ];
    int nline = 0;
    float apdpnratio;
    
    while ( fgets(line, sizeof line, fAPDPN) != NULL ) { //reading the file
    
      int n;
      int count = 0;
      char field [ BUF / 2 ], *ptr = strtok(line, "\n"); 
      int isT = 0;
           
      nline++;
      //std::cout << "line number " << nline << ": " << std::endl;
      //printf("line  = \"%s\"\n", line);
      //std::cout << "count = " << count << ", field = " << field << std::endl;
    
      if (nline > 75849 ) break; 
      while ( sscanf(ptr, "%1999[^ ]%n", field, &n) == 1 ) { //reading every line splitted into fields separated by space
	count++;
	ptr += n;
	isT = 0;

	if ( *ptr != ' ' ) break;
	if ( *field == 'T' ) {
	  isT = 1;	
	  break;
	}

	if (count == 2) {
	  rawid = atol(field); //get my rawid
	}
	if (count == 4) {
	  apdpnratio = atof(field); //get the apdnpratio coefficient
	}
	++ptr;
      }//reading the line
      
      if (isT == 0) { 
	apdpnMap[rawid] = apdpnratio;
      }
      
    }//reading the file 

                 
    //loop on the apdpn map --> get rawid and apdpnratio
    std::cout << ">>>>>> Beginning iteration over the crystals" << std::endl;
    for (std::map<long int,float>::iterator it=apdpnMap.begin(); it!=apdpnMap.end(); ++it) { //apdpn map iterator

      rawid = it->first;
      apdpnratio = it->second;
      
      //get coordinates for the rawid from the RawId map
      ieta = mapRawId_ieta.find(rawid)->second;
      iphi = mapRawId_iphi.find(rawid)->second;
      iz = mapRawId_iz.find(rawid)->second;
      
      float a = alphaMap[ieta][iphi][iz]; //get alpha from the map
      float LC = pow (apdpnratio, a); //get LC coefficient
      float IC = ICmap[ieta][iphi][iz]; //get IC coefficient from the map
  
      /*
      //DEBUG
      std::cout << "(ieta,iphi,iz) = " << ieta << ", " << iphi << ", " << iz << " --> a =  " << a << std::endl;
      std::cout << "(ieta,iphi,iz) = " << ieta << ", " << iphi << ", " << iz << " --> IC =  " << IC << std::endl;
      std::cout << "LC = (apdpnratio)^alpha = " << apdpnratio << "^" << a << " = " << LC << std::endl;
      */

      if(ichStatus[ieta][iphi][iz] > cutChStatus) continue;
      
      if ( mapRawId_EB[rawid]==1 ) { //EB
	eCut_b = ADCAmp_b * LC * IC * ADCToGeV_b;
	
	if (ieta < 0) { //EB- histo from 0 to 84
	  eCut_spectrum_b_histos[ieta+85]->Fill(eCut_b*1000.);
	  //std::cout << "EB- Filling histo number " << ieta+85 << std::endl; 
	}
	else if (ieta > 0) { //EB+ histo 85 to 169
	  eCut_spectrum_b_histos[ieta+84]->Fill(eCut_b*1000.); 
	  //std::cout << "EB+ Filling histo number " << ieta+84 << std::endl; 
	}      
      }

      else if ( mapRawId_EB[rawid]==0 ) { //EC
	eCut_e = ADCAmp_e * LC * IC * ADCToGeV_e;
	int iring = eeId->GetEndcapRing(ieta,iphi,iz); //actually ieta is ix and iphi is iy
	eCut_spectrum_e_histos[iring]->Fill(eCut_e*1000.);  
      }
      
    }//apdpn map iterator
    
    std::cout << ">>>>>> End of the iteration over the crystals" << std::endl;
      
    //write ouput root file
    outputFile->cd();
    for(int i=0;i<kBarlRings;i++){
      eCut_spectrum_b_histos[i]->Write();
    }
    for(int i=0;i<kEndcEtaRings;i++){
      eCut_spectrum_e_histos[i]->Write();
    }
    outputFile->Close();
    std::cout << "Output file written" << std::endl;
    
  }//loop over the list
 
    
}//main 

// function to convert unsigned int into string
string uintToString(unsigned int val)
{
  char buff[500];
  sprintf(buff, "%u", val);
  string str = buff;
  return(str);
}
