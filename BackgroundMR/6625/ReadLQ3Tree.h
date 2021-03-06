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
   double CaloJetPT[50];
   double CaloJetEta[50];
   double CaloJetPhi[50];
   double CaloJetCSVTag[50];
   double CaloJetCSVMTag[50];
   double CaloJetTCHPTag[50];
   double CaloJetTCHETag[50];
   int PFJetCount;
   int PFJetCount30;
   int PFJetCount50;
   int PFJetCount100;
   double PFJetPT[50];
   double PFJetEta[50];
   double PFJetPhi[50];
   double PFJetCSVTag[50];
   double PFJetCSVMTag[50];
   double PFJetTCHPTag[50];
   double PFJetTCHETag[50];
   int PrimaryVertexCount;
   double PrimaryVertexMaxSumPT;
   double CaloMET[2];
   double PFMET[2];
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
}




