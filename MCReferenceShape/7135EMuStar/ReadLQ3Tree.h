#include "TTree.h"

struct TreeRecord;

class TreeRecord
{
public:
   double RunNumber;
   double EventNumber;
   double BunchCrossing;
   double LumiSection;
   double Orbit;
   int CaloJetCount;
   int CaloJetCount30;
   int CaloJetCount50;
   int CaloJetCount100;
   double CaloJetE[100];
   double CaloJetPT[100];
   double CaloJetEta[100];
   double CaloJetPhi[100];
   double CaloJetCSVTag[100];
   double CaloJetCSVMTag[100];
   double CaloJetTCHPTag[100];
   double CaloJetTCHETag[100];
   int PFJetCount;
   int PFJetCount30;
   int PFJetCount50;
   int PFJetCount100;
   double PFJetE[100];
   double PFJetPT[100];
   double PFJetEta[100];
   double PFJetPhi[100];
   double PFJetCSVTag[100];
   double PFJetCSVMTag[100];
   double PFJetTCHPTag[100];
   double PFJetTCHETag[100];
   int PrimaryVertexCount;
   double PrimaryVertexMaxSumPT;
   double CaloMET[2];
   double PFMET[2];
   int GoodMuonCount;
   int GoodMuonCountTight;
   int GoodMuonCountLoose;
   double Muon1PT;
   double Muon1Eta;
   double Muon1Phi;
   bool Muon1PassTight;
   bool Muon1PassLoose;
   double Muon2PT;
   double Muon2Eta;
   double Muon2Phi;
   bool Muon2PassTight;
   bool Muon2PassLoose;
   double Muon3PT;
   double Muon3Eta;
   double Muon3Phi;
   bool Muon3PassTight;
   bool Muon3PassLoose;
   int GoodElectronCount;
   int GoodElectronCount95;
   int GoodElectronCount90;
   int GoodElectronCount85;
   int GoodElectronCount80;
   double Electron1PT;
   double Electron1Eta;
   double Electron1Phi;
   double Electron2PT;
   double Electron3PT;
   double Electron4PT;
   bool PassHLT;
   bool PassNoiseFilter;
   bool PassCaloJetID;
   bool PassCaloJet60;
   bool PassPFJetID;
   bool PassPFJet60;
public:
   TreeRecord() { Clear(); }
   ~TreeRecord() { Clear(); }
   void Clear();
   void SetBranchAddress(TTree *Tree);
};

void TreeRecord::Clear()
{
   RunNumber = 0;
   EventNumber = 0;
   BunchCrossing = 0;
   LumiSection = 0;
   Orbit = 0;

   CaloJetCount = 0;
   CaloJetCount30 = 0;
   CaloJetCount50 = 0;
   CaloJetCount100 = 0;
   for(int i = 0; i < 50; i++)
   {
      CaloJetE[i] = -10;
      CaloJetPT[i] = -10;
      CaloJetEta[i] = -10;
      CaloJetPhi[i] = -10;
      CaloJetCSVTag[i] = -10;
      CaloJetCSVMTag[i] = -10;
      CaloJetTCHPTag[i] = -10;
      CaloJetTCHETag[i] = -10;
   }

   PFJetCount = 0;
   PFJetCount30 = 0;
   PFJetCount50 = 0;
   PFJetCount100 = 0;
   for(int i = 0; i < 50; i++)
   {
      PFJetE[i] = -10;
      PFJetPT[i] = -10;
      PFJetEta[i] = -10;
      PFJetPhi[i] = -10;
      PFJetCSVTag[i] = -10;
      PFJetCSVMTag[i] = -10;
      PFJetTCHPTag[i] = -10;
      PFJetTCHETag[i] = -10;
   }

   PrimaryVertexCount = 0;
   PrimaryVertexMaxSumPT = 0;

   CaloMET[0] = 0;   CaloMET[1] = 0;
   PFMET[0] = 0;   PFMET[1] = 0;

   GoodMuonCount = 0;
   Muon1PT = 0;
   Muon1Eta = 0;
   Muon1Phi = 0;
   Muon2PT = 0;
   Muon2Eta = 0;
   Muon2Phi = 0;

   GoodElectronCount = 0;

   PassHLT = false;
   PassNoiseFilter = false;
   PassCaloJetID = false;
   PassCaloJet60 = false;
   PassPFJetID = false;
   PassPFJet60 = false;
}

void TreeRecord::SetBranchAddress(TTree *Tree)
{
   if(Tree == NULL)
      return;

   Tree->SetBranchAddress("RunNumber", &RunNumber);
   Tree->SetBranchAddress("EventNumber", &EventNumber);
   Tree->SetBranchAddress("BunchCrossing", &BunchCrossing);
   Tree->SetBranchAddress("LumiSection", &LumiSection);
   Tree->SetBranchAddress("Orbit", &Orbit);
   Tree->SetBranchAddress("CaloJetCount", &CaloJetCount);
   Tree->SetBranchAddress("CaloJetCount30", &CaloJetCount30);
   Tree->SetBranchAddress("CaloJetCount50", &CaloJetCount50);
   Tree->SetBranchAddress("CaloJetCount100", &CaloJetCount100);
   Tree->SetBranchAddress("CaloJetE", &CaloJetE);
   Tree->SetBranchAddress("CaloJetPT", CaloJetPT);
   Tree->SetBranchAddress("CaloJetEta", CaloJetEta);
   Tree->SetBranchAddress("CaloJetPhi", CaloJetPhi);
   Tree->SetBranchAddress("CaloJetCSVTag", CaloJetCSVTag);
   Tree->SetBranchAddress("CaloJetCSVMTag", CaloJetCSVMTag);
   Tree->SetBranchAddress("CaloJetTCHPTag", CaloJetTCHPTag);
   Tree->SetBranchAddress("CaloJetTCHETag", CaloJetTCHETag);
   Tree->SetBranchAddress("PFJetCount", &PFJetCount);
   Tree->SetBranchAddress("PFJetCount30", &PFJetCount30);
   Tree->SetBranchAddress("PFJetCount50", &PFJetCount50);
   Tree->SetBranchAddress("PFJetCount100", &PFJetCount100);
   Tree->SetBranchAddress("PFJetE", PFJetE);
   Tree->SetBranchAddress("PFJetPT", PFJetPT);
   Tree->SetBranchAddress("PFJetEta", PFJetEta);
   Tree->SetBranchAddress("PFJetPhi", PFJetPhi);
   Tree->SetBranchAddress("PFJetCSVTag", PFJetCSVTag);
   Tree->SetBranchAddress("PFJetCSVMTag", PFJetCSVMTag);
   Tree->SetBranchAddress("PFJetTCHPTag", PFJetTCHPTag);
   Tree->SetBranchAddress("PFJetTCHETag", PFJetTCHETag);
   Tree->SetBranchAddress("PrimaryVertexCount", &PrimaryVertexCount);
   Tree->SetBranchAddress("PrimaryVertexMaxSumPT", &PrimaryVertexMaxSumPT);
   Tree->SetBranchAddress("CaloMET", CaloMET);
   Tree->SetBranchAddress("PFMET", PFMET);
   Tree->SetBranchAddress("GoodMuonCount", &GoodMuonCount);
   Tree->SetBranchAddress("GoodMuonCountTight", &GoodMuonCountLoose);
   Tree->SetBranchAddress("GoodMuonCountLoose", &GoodMuonCountTight);
   Tree->SetBranchAddress("Muon1PT", &Muon1PT);
   Tree->SetBranchAddress("Muon1Eta", &Muon1Eta);
   Tree->SetBranchAddress("Muon1Phi", &Muon1Phi);
   Tree->SetBranchAddress("Muon1PassTight", &Muon1PassTight);
   Tree->SetBranchAddress("Muon1PassLoose", &Muon1PassLoose);
   Tree->SetBranchAddress("Muon2PT", &Muon2PT);
   Tree->SetBranchAddress("Muon2Eta", &Muon2Eta);
   Tree->SetBranchAddress("Muon2Phi", &Muon2Phi);
   Tree->SetBranchAddress("Muon2PassTight", &Muon2PassTight);
   Tree->SetBranchAddress("Muon2PassLoose", &Muon2PassLoose);
   Tree->SetBranchAddress("Muon3PT", &Muon3PT);
   Tree->SetBranchAddress("Muon3Eta", &Muon3Eta);
   Tree->SetBranchAddress("Muon3Phi", &Muon3Phi);
   Tree->SetBranchAddress("Muon3PassTight", &Muon3PassTight);
   Tree->SetBranchAddress("Muon3PassLoose", &Muon3PassLoose);
   Tree->SetBranchAddress("GoodElectronCount", &GoodElectronCount);
   Tree->SetBranchAddress("GoodElectronCount95", &GoodElectronCount95);
   Tree->SetBranchAddress("GoodElectronCount90", &GoodElectronCount90);
   Tree->SetBranchAddress("GoodElectronCount85", &GoodElectronCount85);
   Tree->SetBranchAddress("GoodElectronCount80", &GoodElectronCount80);
   Tree->SetBranchAddress("Electron1PT", &Electron1PT);
   Tree->SetBranchAddress("Electron1Eta", &Electron1Eta);
   Tree->SetBranchAddress("Electron1Phi", &Electron1Phi);
   Tree->SetBranchAddress("Electron2PT", &Electron2PT);
   Tree->SetBranchAddress("Electron3PT", &Electron3PT);
   Tree->SetBranchAddress("Electron4PT", &Electron4PT);
   Tree->SetBranchAddress("PassHLT", &PassHLT);
   Tree->SetBranchAddress("PassNoiseFilter", &PassNoiseFilter);
   Tree->SetBranchAddress("PassCaloJetID", &PassCaloJetID);
   Tree->SetBranchAddress("PassCaloJet60", &PassCaloJet60);
   Tree->SetBranchAddress("PassPFJetID", &PassPFJetID);
   Tree->SetBranchAddress("PassPFJet60", &PassPFJet60);
}




