#include <string>
#include <vector>
using namespace std;

#include "TH1D.h"
#include "TH2D.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include "DrawRandom.h"
#include "TauHelperFunctions.h"
#include "PlotHelper2.h"

int main(int argc, char *argv[]);
void DoToyAndAppendPlots(PsFileHelper &PsFile, const double HeavyMass, const double Energy);

int main(int argc, char *argv[])
{
   gROOT->SetStyle("Plain");

   const Int_t NRGBs = 5;
   const Int_t NCont = 255;

   Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
   Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
   Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
   Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
   TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
   gStyle->SetNumberContours(NCont);

   PsFileHelper PsFile("MRToy1_StaticGluonMasslessFinalStates.ps");
   PsFile.AddTextPage("Toys to understand MR better - part 1");

   vector<string> Explanations;
   Explanations.push_back("Setup: gluon with spatial momentum zero, decay into two heavy object");
   Explanations.push_back("of certain mass, each then further decays into two massless objects");
   Explanations.push_back("one of them visible, one invisible");
   Explanations.push_back("");
   Explanations.push_back("Assume perfect reconstruction of the visible final objects");
   Explanations.push_back("Check MR distribution changing energy of initial gluon");
   PsFile.AddTextPage(Explanations);

   DoToyAndAppendPlots(PsFile, 300, 600);
   DoToyAndAppendPlots(PsFile, 300, 600.25);
   DoToyAndAppendPlots(PsFile, 300, 600.5);
   DoToyAndAppendPlots(PsFile, 300, 600.75);
   DoToyAndAppendPlots(PsFile, 300, 601);
   DoToyAndAppendPlots(PsFile, 300, 602);
   DoToyAndAppendPlots(PsFile, 300, 603);
   DoToyAndAppendPlots(PsFile, 300, 604);
   DoToyAndAppendPlots(PsFile, 300, 605);
   DoToyAndAppendPlots(PsFile, 300, 606);
   DoToyAndAppendPlots(PsFile, 300, 607);
   DoToyAndAppendPlots(PsFile, 300, 608);
   DoToyAndAppendPlots(PsFile, 300, 609);
   DoToyAndAppendPlots(PsFile, 300, 610);
   DoToyAndAppendPlots(PsFile, 300, 620);
   DoToyAndAppendPlots(PsFile, 300, 630);
   DoToyAndAppendPlots(PsFile, 300, 640);
   DoToyAndAppendPlots(PsFile, 300, 650);
   DoToyAndAppendPlots(PsFile, 300, 700);

   PsFile.AddTimeStampPage();
   PsFile.Close();

   return 0;
}

void DoToyAndAppendPlots(PsFileHelper &PsFile, const double HeavyMass, const double Energy)
{
   TH1D HMR("HMR", Form("MR distribution, Mass %.2f, Energy %.2f", HeavyMass, Energy), 100, 0, HeavyMass * 2);
   TH1D HMRT("HMRT", Form("MRT distribution, Mass %.2f, Energy %.2f", HeavyMass, Energy), 100, 0, HeavyMass * 2);
   TH1D HR("HR", Form("R distribution, Mass %.2f, Energy %.2f", HeavyMass, Energy), 100, 0, 1.5);
   TH2D HMRVsR("HMRVsR", "MR vs R^{2};MR;R^{2}", 100, 0, HeavyMass * 2, 100, 0, 1.5);

   for(int iEntry = 0; iEntry < 1000000; iEntry++)
   {
      // Decay angles of the two heavy guys
      double Theta = DrawSine(0, PI);
      double Phi = DrawRandom(-PI, PI);
      double Gamma = Energy / 2 / HeavyMass;
      double Beta = GammaToBeta(Gamma);

      double HeavyParticle1Direction[4] = {1, sin(Theta) * cos(Phi), sin(Theta) * sin(Phi), cos(Theta)};
      double HeavyParticle2Direction[4] = {1, -sin(Theta) * cos(Phi), -sin(Theta) * sin(Phi), -cos(Theta)};

      // Inside CM of first heavy particle
      double Theta1 = DrawSine(0, PI);
      double Phi1 = DrawRandom(-PI, PI);
      double VisibleChild1Heavy1CM[4] = {1, sin(Theta1) * cos(Phi1), sin(Theta1) * sin(Phi1), cos(Theta1)};
      double InvisibleChild1Heavy1CM[4] = {1, -sin(Theta1) * cos(Phi1), -sin(Theta1) * sin(Phi1), -cos(Theta1)};

      for(int i = 0; i < 4; i++)
      {
         VisibleChild1Heavy1CM[i] = VisibleChild1Heavy1CM[i] * HeavyMass / 2;
         InvisibleChild1Heavy1CM[i] = InvisibleChild1Heavy1CM[i] * HeavyMass / 2;
      }
      
      // In CM of second heavy particle
      double Theta2 = DrawSine(0, PI);
      double Phi2 = DrawRandom(-PI, PI);
      double VisibleChild2Heavy2CM[4] = {1, sin(Theta2) * cos(Phi2), sin(Theta2) * sin(Phi2), cos(Theta2)};
      double InvisibleChild2Heavy2CM[4] = {1, -sin(Theta2) * cos(Phi2), -sin(Theta2) * sin(Phi2), -cos(Theta2)};
      
      for(int i = 0; i < 4; i++)
      {
         VisibleChild2Heavy2CM[i] = VisibleChild2Heavy2CM[i] * HeavyMass / 2;
         InvisibleChild2Heavy2CM[i] = InvisibleChild2Heavy2CM[i] * HeavyMass / 2;
      }

      // boost back to lab frame
      double VisibleChild1[4], InvisibleChild1[4];
      double VisibleChild2[4], InvisibleChild2[4];

      Boost(VisibleChild1Heavy1CM, VisibleChild1, HeavyParticle1Direction, Beta);
      Boost(InvisibleChild1Heavy1CM, InvisibleChild1, HeavyParticle1Direction, Beta);
      Boost(VisibleChild2Heavy2CM, VisibleChild2, HeavyParticle2Direction, Beta);
      Boost(InvisibleChild2Heavy2CM, InvisibleChild2, HeavyParticle2Direction, Beta);

      if(fabs(GetEta(VisibleChild1)) > 3)
         continue;
      if(fabs(GetEta(VisibleChild2)) > 3)
         continue;

      double InvisibleTotal[4];
      InvisibleTotal[0] = InvisibleChild1[0] + InvisibleChild2[0];
      InvisibleTotal[1] = InvisibleChild1[1] + InvisibleChild2[1];
      InvisibleTotal[2] = InvisibleChild1[2] + InvisibleChild2[2];
      InvisibleTotal[3] = InvisibleChild1[3] + InvisibleChild2[3];

      double MR = GetMR(VisibleChild1, VisibleChild2);
      double MRT = GetMRT(VisibleChild1, VisibleChild2, InvisibleTotal);
      double R = GetR(VisibleChild1, VisibleChild2, InvisibleTotal);

      HMR.Fill(MR);
      HMRT.Fill(MRT);
      HR.Fill(R);
      HMRVsR.Fill(MR, R * R);
   }

   TCanvas canvas;

   canvas.Divide(2, 2);

   canvas.cd(1);
   HMR.Draw();
   canvas.cd(1)->SetLogy();

   canvas.cd(2);
   HMRT.Draw();
   canvas.cd(2)->SetLogy();
   
   canvas.cd(3);
   HR.Draw();
   canvas.cd(3)->SetLogy();
   
   canvas.cd(4);
   HMRVsR.Draw("colz");

   PsFile.AddCanvas(canvas);
}





