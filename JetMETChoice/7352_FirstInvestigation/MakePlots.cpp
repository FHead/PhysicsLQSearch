#include <iostream>
using namespace std;

#include "TH1D.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "PlotHelper2.h"

int main();
void CompareHistograms(PsFileHelper &PsFile, TH1D &H1, TH1D &H2, TH1D &H3, bool Save = true);

int main()
{
   TFile F("OutputFile.root");

   TNtuple *Tree = (TNtuple *)F.Get("OutputTree");

   PsFileHelper PsFile("Comparison.ps");
   PsFile.AddTextPage("Compare different Jet/MET MR/R distributions");

   vector<string> Explanations;
   Explanations.push_back("Compare MR/R distributions for the following:");
   Explanations.push_back("      1. Calo Jet + Calo MET");
   Explanations.push_back("      2. Calo Jet + PF MET");
   Explanations.push_back("      3. PF Jet + PF MET");
   Explanations.push_back("Hopefully the difference is not huge");
   Explanations.push_back("");
   Explanations.push_back("Note that MR is only dependent on jet type, not MET");
   PsFile.AddTextPage(Explanations);

   float CaloMR, PFMR;
   float CaloCaloR, CaloPFR, PFPFR;
   Tree->SetBranchAddress("CaloMR", &CaloMR);
   Tree->SetBranchAddress("PFMR", &PFMR);
   Tree->SetBranchAddress("CaloCaloR", &CaloCaloR);
   Tree->SetBranchAddress("CaloPFR", &CaloPFR);
   Tree->SetBranchAddress("PFPFR", &PFPFR);

   TH1D HCaloMR_CaloCaloR2014("HCaloMR_CaloCaloR2014", "Calo MR, Calo-Calo R^{2} > 0.14;MR", 40, 400, 1000);
   TH1D HCaloMR_CaloPFR2014("HCaloMR_CaloPFR2014", "Calo MR, Calo-PF R^{2} > 0.14;MR", 40, 400, 1000);
   TH1D HPFMR_PFPFR2014("HPFMR_PFPFR2014", "PF MR, PF-PF R^{2} > 0.14;MR", 40, 400, 1000);
   
   TH1D HCaloMR_CaloCaloR2020("HCaloMR_CaloCaloR2020", "Calo MR, Calo-Calo R^{2} > 0.20;MR", 40, 400, 1000);
   TH1D HCaloMR_CaloPFR2020("HCaloMR_CaloPFR2020", "Calo MR, Calo-PF R^{2} > 0.20;MR", 40, 400, 1000);
   TH1D HPFMR_PFPFR2020("HPFMR_PFPFR2020", "PF MR, PF-PF R^{2} > 0.20;MR", 40, 400, 1000);

   TH1D HCaloMR_CaloCaloR2025("HCaloMR_CaloCaloR2025", "Calo MR, Calo-Calo R^{2} > 0.25;MR", 40, 400, 1000);
   TH1D HCaloMR_CaloPFR2025("HCaloMR_CaloPFR2025", "Calo MR, Calo-PF R^{2} > 0.25;MR", 40, 400, 1000);
   TH1D HPFMR_PFPFR2025("HPFMR_PFPFR2025", "PF MR, PF-PF R^{2} > 0.25;MR", 40, 400, 1000);
   
   TH1D HCaloMR_CaloCaloR2030("HCaloMR_CaloCaloR2030", "Calo MR, Calo-Calo R^{2} > 0.30;MR", 40, 400, 1000);
   TH1D HCaloMR_CaloPFR2030("HCaloMR_CaloPFR2030", "Calo MR, Calo-PF R^{2} > 0.30;MR", 40, 400, 1000);
   TH1D HPFMR_PFPFR2030("HPFMR_PFPFR2030", "PF MR, PF-PF R^{2} > 0.30;MR", 40, 400, 1000);
   
   TH1D HCaloMR_CaloCaloR2035("HCaloMR_CaloCaloR2035", "Calo MR, Calo-Calo R^{2} > 0.35;MR", 40, 400, 1000);
   TH1D HCaloMR_CaloPFR2035("HCaloMR_CaloPFR2035", "Calo MR, Calo-PF R^{2} > 0.35;MR", 40, 400, 1000);
   TH1D HPFMR_PFPFR2035("HPFMR_PFPFR2035", "PF MR, PF-PF R^{2} > 0.35;MR", 40, 400, 1000);
   
   TH1D HCaloCaloR2_CaloMR400("HCaloCaloR2_CaloMR400", "Calo-Calo R^{2}, Calo MR > 400;R^{2}", 40, 0.14, 1);
   TH1D HCaloPFR2_CaloMR400("HCaloPFR2_CaloMR400", "Calo-PF R^{2}, Calo MR > 400;R^{2}", 40, 0.14, 1);
   TH1D HPFPFR2_PFMR400("HPFPFR2_PFMR400", "PF-PF R^{2}, PF MR > 400;R^{2}", 40, 0.14, 1);
   
   TH1D HCaloCaloR2_CaloMR500("HCaloCaloR2_CaloMR500", "Calo-Calo R^{2}, Calo MR > 500;R^{2}", 40, 0.14, 1);
   TH1D HCaloPFR2_CaloMR500("HCaloPFR2_CaloMR500", "Calo-PF R^{2}, Calo MR > 500;R^{2}", 40, 0.14, 1);
   TH1D HPFPFR2_PFMR500("HPFPFR2_PFMR500", "PF-PF R^{2}, PF MR > 500;R^{2}", 40, 0.14, 1);
   
   TH1D HCaloCaloR2_CaloMR600("HCaloCaloR2_CaloMR600", "Calo-Calo R^{2}, Calo MR > 600;R^{2}", 40, 0.14, 1);
   TH1D HCaloPFR2_CaloMR600("HCaloPFR2_CaloMR600", "Calo-PF R^{2}, Calo MR > 600;R^{2}", 40, 0.14, 1);
   TH1D HPFPFR2_PFMR600("HPFPFR2_PFMR600", "PF-PF R^{2}, PF MR > 600;R^{2}", 40, 0.14, 1);
   
   int EntryCount = Tree->GetEntries();
   for(int iEntry = 0; iEntry < EntryCount; iEntry++)
   {
      Tree->GetEntry(iEntry);

      double CaloCaloR2 = CaloCaloR * CaloCaloR;
      double CaloPFR2 = CaloPFR * CaloPFR;
      double PFPFR2 = PFPFR * PFPFR;

      if(CaloCaloR2 > 0.14)
         HCaloMR_CaloCaloR2014.Fill(CaloMR);
      if(CaloPFR2 > 0.14)
         HCaloMR_CaloPFR2014.Fill(CaloMR);
      if(PFPFR2 > 0.14)
         HPFMR_PFPFR2014.Fill(PFMR);

      if(CaloCaloR2 > 0.20)
         HCaloMR_CaloCaloR2020.Fill(CaloMR);
      if(CaloPFR2 > 0.20)
         HCaloMR_CaloPFR2020.Fill(CaloMR);
      if(PFPFR2 > 0.20)
         HPFMR_PFPFR2020.Fill(PFMR);

      if(CaloCaloR2 > 0.25)
         HCaloMR_CaloCaloR2025.Fill(CaloMR);
      if(CaloPFR2 > 0.25)
         HCaloMR_CaloPFR2025.Fill(CaloMR);
      if(PFPFR2 > 0.25)
         HPFMR_PFPFR2025.Fill(PFMR);

      if(CaloCaloR2 > 0.30)
         HCaloMR_CaloCaloR2030.Fill(CaloMR);
      if(CaloPFR2 > 0.30)
         HCaloMR_CaloPFR2030.Fill(CaloMR);
      if(PFPFR2 > 0.30)
         HPFMR_PFPFR2030.Fill(PFMR);

      if(CaloCaloR2 > 0.35)
         HCaloMR_CaloCaloR2035.Fill(CaloMR);
      if(CaloPFR2 > 0.35)
         HCaloMR_CaloPFR2035.Fill(CaloMR);
      if(PFPFR2 > 0.35)
         HPFMR_PFPFR2035.Fill(PFMR);

      if(CaloMR > 400)
         HCaloCaloR2_CaloMR400.Fill(CaloCaloR2);
      if(CaloMR > 400)
         HCaloPFR2_CaloMR400.Fill(CaloPFR2);
      if(PFMR > 400)
         HPFPFR2_PFMR400.Fill(PFPFR2);

      if(CaloMR > 500)
         HCaloCaloR2_CaloMR500.Fill(CaloCaloR2);
      if(CaloMR > 500)
         HCaloPFR2_CaloMR500.Fill(CaloPFR2);
      if(PFMR > 500)
         HPFPFR2_PFMR500.Fill(PFPFR2);

      if(CaloMR > 600)
         HCaloCaloR2_CaloMR600.Fill(CaloCaloR2);
      if(CaloMR > 600)
         HCaloPFR2_CaloMR600.Fill(CaloPFR2);
      if(PFMR > 600)
         HPFPFR2_PFMR600.Fill(PFPFR2);
   }

   CompareHistograms(PsFile, HCaloMR_CaloCaloR2014, HCaloMR_CaloPFR2014, HPFMR_PFPFR2014);
   CompareHistograms(PsFile, HCaloMR_CaloCaloR2020, HCaloMR_CaloPFR2020, HPFMR_PFPFR2020);
   CompareHistograms(PsFile, HCaloMR_CaloCaloR2025, HCaloMR_CaloPFR2025, HPFMR_PFPFR2025);
   CompareHistograms(PsFile, HCaloMR_CaloCaloR2030, HCaloMR_CaloPFR2030, HPFMR_PFPFR2030);
   CompareHistograms(PsFile, HCaloMR_CaloCaloR2035, HCaloMR_CaloPFR2035, HPFMR_PFPFR2035);
   CompareHistograms(PsFile, HCaloCaloR2_CaloMR400, HCaloPFR2_CaloMR400, HPFPFR2_PFMR400);
   CompareHistograms(PsFile, HCaloCaloR2_CaloMR500, HCaloPFR2_CaloMR500, HPFPFR2_PFMR500);
   CompareHistograms(PsFile, HCaloCaloR2_CaloMR600, HCaloPFR2_CaloMR600, HPFPFR2_PFMR600);

   PsFile.AddTimeStampPage();
   PsFile.Close();

   F.Close();
}

void CompareHistograms(PsFileHelper &PsFile, TH1D &H1, TH1D &H2, TH1D &H3, bool Save)
{
   TCanvas Canvas;

   H1.SetTitle(TString(H1.GetTitle()).ReplaceAll("Calo ", "").ReplaceAll("Calo-", ""));

   H1.SetStats(0);
   H2.SetStats(0);
   H3.SetStats(0);

   H1.SetLineWidth(2);
   H2.SetLineWidth(2);
   H3.SetLineWidth(2);

   H1.SetLineColor(kBlack);
   H2.SetLineColor(kRed);
   H3.SetLineColor(kGreen);

   H1.Draw();
   H2.Draw("same");
   H3.Draw("same");

   TLegend legend(0.5, 0.8, 0.9, 0.6);
   legend.SetFillStyle(0);
   legend.SetBorderSize(0);
   legend.SetTextFont(42);
   legend.AddEntry(&H1, "Calo Jet - Calo MET", "l");
   legend.AddEntry(&H2, "Calo Jet - PF MET", "l");
   legend.AddEntry(&H3, "PF Jet - PF MET", "l");
   legend.Draw();

   Canvas.SetLogy();
   PsFile.AddCanvas(Canvas);

   if(Save == false)
      return;

   static int PlotIndex = 0;
   PlotIndex = PlotIndex + 1;

   TCanvas Canvas2("C2", "", 1024, 1024);
   
   H1.Draw();
   H2.Draw("same");
   H3.Draw("same");
   legend.Draw();

   Canvas2.SetLogy();
   Canvas2.SaveAs(Form("Plot_%d.png", PlotIndex));
   Canvas2.SaveAs(Form("Plot_%d.C", PlotIndex));
   Canvas2.SaveAs(Form("Plot_%d.eps", PlotIndex));
   Canvas2.SaveAs(Form("Plot_%d.pdf", PlotIndex));
}






