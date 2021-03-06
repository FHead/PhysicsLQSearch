#include <iostream>
using namespace std;

#include "TGraph.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TLegend.h"

#include "PlotHelper2.h"
#include "SetStyle.h"

int main();
TGraph MakeGraph(double MR, double R);
TGraph MakeOrGraph(double MR[], double R[], int Count);

int main()
{
   SetStyle();

   // 1e33
   double MR1[4] = {200};
   double R1[4] = {0.20};
   double MR1B[4] = {200};
   double R1B[4] = {0.25};

   // 1.4e33, 2e33 and 3e33
   double MR2[4] = {200};
   double R2[4] = {0.25};
   double MR2B[4] = {200};
   double R2B[4] = {0.29};

   TGraph G1[4], G2[4], G1B[4], G2B[4];
   for(int i = 0; i < 1; i++)
   {
      G1[i] = MakeGraph(MR1[i], R1[i]);   G1[i].SetLineColor(1);   G1[i].SetLineWidth(2);
      G2[i] = MakeGraph(MR2[i], R2[i]);   G2[i].SetLineColor(2);   G2[i].SetLineWidth(2);
      G1B[i] = MakeGraph(MR1B[i], R1B[i]);   G1B[i].SetLineColor(1);   G1B[i].SetLineStyle(2);
      G2B[i] = MakeGraph(MR2B[i], R2B[i]);   G2B[i].SetLineColor(2);   G2B[i].SetLineStyle(2);
   }

   TGraph GAll1 = MakeOrGraph(MR1, R1, 1);
   TGraph GAll1B = MakeOrGraph(MR1B, R1B, 1);
   TGraph GAll2 = MakeOrGraph(MR2, R2, 1);
   TGraph GAll2B = MakeOrGraph(MR2B, R2B, 1);

   GAll1.SetNameTitle("GAll1", "1e33 triggers");
   GAll1B.SetNameTitle("GAll1B", "1e33 triggers, backup");
   GAll2.SetNameTitle("GAll2", "1.4e33 triggers");
   GAll2B.SetNameTitle("GAll2B", "1.4e33 triggers, backup");

   TH2D HWorld("HWorld", "Triggers, separately;M_{R};R^{2}", 100, 0, 1000, 100, 0, 0.5);
   HWorld.SetStats(0);
   
   PsFileHelper PsFile("TriggerPlane.ps");
   PsFile.AddTextPage("Trigger planes");

   TCanvas C;
   HWorld.Draw();
   for(int i = 0; i < 1; i++)
   {
      G1[i].Draw("l");
      G2[i].Draw("l");
      G1B[i].Draw("l");
      G2B[i].Draw("l");
   }
   PsFile.AddCanvas(C);

   PsFile.AddPlot(GAll1, "al");
   PsFile.AddPlot(GAll1B, "al");
   PsFile.AddPlot(GAll2, "al");
   PsFile.AddPlot(GAll2B, "al");

   TCanvas C2;
   HWorld.SetTitle("MR/R leg of triggers from each era, (muon and electron cross-triggers)");
   HWorld.Draw();
   GAll1.SetLineColor(1);    GAll1.SetLineWidth(2);    GAll1.Draw();
   GAll2.SetLineColor(2);    GAll2.SetLineWidth(2);    GAll2.Draw();
   GAll1B.SetLineColor(1);   GAll1B.SetLineStyle(2);   GAll1B.Draw();
   GAll2B.SetLineColor(2);   GAll2B.SetLineStyle(2);   GAll2B.Draw();

   TLegend legend(0.35, 0.85, 0.85, 0.55);
   legend.AddEntry(&GAll1, "10^{33}", "l");
   // legend.AddEntry(&GAll1B, "10^{33}, backup", "l");
   legend.AddEntry(&GAll2, "1.4 #times 10^{33} - 3 #times 10^{33}", "l");
   // legend.AddEntry(&GAll2B, "1.4 #times 10^{33} - 3 #times 10^{33}, backup", "l");
   legend.SetFillStyle(0);
   legend.SetBorderSize(0);
   legend.SetTextFont(42);
   legend.Draw();

   PsFile.AddCanvas(C2);
   
   TCanvas C2Square("C2Square", "C2", 1024, 1024);
   // HWorld.SetTitle("MR/R leg of triggers from each era, (muon and electron cross-triggers)");
   HWorld.SetTitle("");
   HWorld.Draw();
   GAll1.SetLineColor(1);    GAll1.SetLineWidth(2);    GAll1.Draw();
   GAll2.SetLineColor(2);    GAll2.SetLineWidth(2);    GAll2.Draw();
   // GAll1B.SetLineColor(1);   GAll1B.SetLineStyle(2);   GAll1B.Draw();
   // GAll2B.SetLineColor(2);   GAll2B.SetLineStyle(2);   GAll2B.Draw();
   legend.Draw();
   C2Square.SaveAs("LeptonTriggerCurves.png");
   C2Square.SaveAs("LeptonTriggerCurves.C");
   C2Square.SaveAs("LeptonTriggerCurves.eps");
   C2Square.SaveAs("LeptonTriggerCurves.pdf");

   PsFile.AddTimeStampPage();
   PsFile.Close();
}

TGraph MakeGraph(double MR, double R)
{
   TGraph NewGraph;

   NewGraph.SetPoint(0, MR, 1);
   NewGraph.SetPoint(1, MR, R * R);
   NewGraph.SetPoint(2, 1000, R * R);

   return NewGraph;
}

TGraph MakeOrGraph(double MR[], double R[], int Count)
{
   TGraph NewGraph;

   NewGraph.SetPoint(0, MR[0], 1);

   for(int i = 0; i < Count; i++)
   {
      NewGraph.SetPoint(i * 2 + 1, MR[i], R[i] * R[i]);
      NewGraph.SetPoint(i * 2 + 2, MR[i+1], R[i] * R[i]);
   }

   NewGraph.SetPoint(2 * Count, 1000, R[Count-1] * R[Count-1]);

   return NewGraph;
}





