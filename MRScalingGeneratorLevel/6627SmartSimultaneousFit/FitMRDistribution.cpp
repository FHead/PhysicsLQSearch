#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TColor.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TCanvas.h"

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooTwoSideGaussianWithAnExponentialTail.h"
#include "RooPlot.h"
#include "RooArgSet.h"
#include "RooFormulaVar.h"
#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooSameAs.h"
#include "RooAtLeast.h"
#include "RooAbsPdf.h"
#include "RooExponential.h"
using namespace RooFit;

#include "PlotHelper2.h"

#define Strategy_Normal 100
#define Strategy_IgnoreLeft 101
#define Strategy_IgnoreGaussian 102

struct SingleFitResult;
int main(int argc, char *argv[]);
SingleFitResult FitWithRCut(PsFileHelper &PsFile, string Filename, double RCut, int Strategy = Strategy_Normal);
void FitWithRCut(PsFileHelper &PsFile, string Filename, vector<double> RCuts);

struct SingleFitResult
{
   double X0;
   double SigmaL;
   double SigmaR;
   double S;
   double SError;
   int Strategy;
};

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

   PsFileHelper PsFile("DevelopFit.ps");
   PsFile.AddTextPage("Developing MR scaling fit (simultaneous)");

   vector<string> Explanations;
   Explanations.push_back("Fitting MR tail exponent varying R cut");
   Explanations.push_back("The first part fits each candidate R cut individually");
   Explanations.push_back("Results are then feed into a master simultaneous fit, which");
   Explanations.push_back("      attempts to fit everything together (to get good error)");
   Explanations.push_back("Simultaneous fit assumes scaling relation \"s = a + b * (R cut)^2\"");
   Explanations.push_back("      and fit for parameters \"a\" and \"b\"");
   Explanations.push_back("");
   Explanations.push_back("This is the improved version of simultaneous fit.  For each R cut,");
   Explanations.push_back("      distribution is modelled by a two-sided Gaussian with exponential tail,");
   Explanations.push_back("      normal Gaussian with exponential tail, or simply an exponential.");
   Explanations.push_back("First fit with the most complex case, reduce if necessary (automatic)");
   PsFile.AddTextPage(Explanations);

   vector<double> CutsToTry;
   CutsToTry.push_back(0.20);
   CutsToTry.push_back(0.30);
   CutsToTry.push_back(0.40);
   CutsToTry.push_back(0.50);
   CutsToTry.push_back(0.60);
   
   FitWithRCut(PsFile, "TTbar_MRTree.root", CutsToTry);

   PsFile.AddTimeStampPage();
   PsFile.Close();
}

SingleFitResult FitWithRCut(PsFileHelper &PsFile, string Filename, double RCut, int Strategy)
{
   TFile F(Filename.c_str());
   TTree *Tree = (TTree *)F.Get("MRTree");
   if(Tree == NULL)
      return SingleFitResult();

   cout << RCut << " --- " << Strategy << endl;

   if(Strategy != Strategy_Normal && Strategy != Strategy_IgnoreLeft && Strategy != Strategy_IgnoreGaussian)
      return SingleFitResult();
   
   double MRLowerBound = 150;

   RooRealVar MR("MR", "Chris' kinematic variable", MRLowerBound, 1500, "GeV");
   RooRealVar R("R", "Chris' another kinematic variable", -200, 200);
   RooArgSet TreeVarSet(MR, R);

   RooDataSet Dataset("Dataset", "MR Dataset", Tree, TreeVarSet, Form("R > %f", RCut));

   RooRealVar X0("X0", "gaussian mean", 200, 0, 5000, "GeV");
   RooRealVar SigmaL("SigmaL", "left-hand side gaussian width", 50, 0, 5000, "GeV");
   RooRealVar SigmaR("SigmaR", "right-hand side gaussian width", 50, 0, 5000, "GeV");
   RooRealVar S("S", "exponent of the tail", 0.01, 0, 0.5);
   RooFormulaVar NegativeS("NegativeS", "Negative S", "-S", RooArgList(S));
   RooAbsPdf *Model = NULL;
   if(Strategy == Strategy_Normal)
      Model = new RooTwoSideGaussianWithAnExponentialTail("Model", "Model", MR, X0, SigmaL, SigmaR, S);
   else if(Strategy == Strategy_IgnoreLeft)
      Model = new RooTwoSideGaussianWithAnExponentialTail("Model", "Model", MR, X0, SigmaL, SigmaR, S);
   else if(Strategy == Strategy_IgnoreGaussian)
      Model = new RooExponential("Model", "Model", MR, NegativeS);

   if(Model == NULL)
   {
      cerr << "Something terribly wrong has happened.  Model is NULL.  Please check." << endl;
      return SingleFitResult();
   }

   RooFitResult *FitResult = Model->fitTo(Dataset, Save(true));

   stringstream Subtitle;
   Subtitle << "Fitting with R cut = " << RCut << ", ";
   if(Strategy == Strategy_Normal)
      Subtitle << "Normal";
   else if(Strategy == Strategy_IgnoreLeft)
      Subtitle << "Ignore left";
   else if(Strategy == Strategy_IgnoreGaussian)
      Subtitle << "Exponential";
   PsFile.AddTextPage(Subtitle.str());

   RooPlot *ParameterPlot = MR.frame(Bins(50), Name("FitParameters"), Title("Fit parameters"));
   Model->paramOn(ParameterPlot, Format("NELU", AutoPrecision(3)), Layout(0.1, 0.4, 0.9));
   PsFile.AddPlot(ParameterPlot, "", false);

   RooPlot *FinalResultPlot = MR.frame(Bins(50), Name("FitResults"), Title("MR"), AutoRange(Dataset));
   Dataset.plotOn(FinalResultPlot, MarkerSize(0.9));
   Model->plotOn(FinalResultPlot, LineColor(kBlue));
   PsFile.AddPlot(FinalResultPlot, "", true);

   SingleFitResult result;

   if(Strategy == Strategy_Normal)
   {
      double XC = S.getVal() * SigmaR.getVal() * SigmaR.getVal() + X0.getVal();

      if(MRLowerBound < X0.getVal())
      {
         result.X0 = X0.getVal();
         result.SigmaL = SigmaL.getVal();
         result.SigmaR = SigmaR.getVal();
         result.S = S.getVal();
         result.SError = S.getError();
         result.Strategy = Strategy;
      }

      if(MRLowerBound < XC && MRLowerBound >= X0.getVal())
         return FitWithRCut(PsFile, Filename, RCut, Strategy_IgnoreLeft);
      if(MRLowerBound >= XC)
         return FitWithRCut(PsFile, Filename, RCut, Strategy_IgnoreGaussian);
   }

   if(Strategy == Strategy_IgnoreLeft)
   {
      double XC = S.getVal() * SigmaR.getVal() * SigmaR.getVal() + X0.getVal();

      if(MRLowerBound < XC)
      {
         result.X0 = X0.getVal();
         result.SigmaL = SigmaR.getVal();
         result.SigmaR = SigmaR.getVal();
         result.S = S.getVal();
         result.SError = S.getError();
         result.Strategy = Strategy;
      }

      if(MRLowerBound >= XC)
         return FitWithRCut(PsFile, Filename, RCut, Strategy_IgnoreGaussian);
   }

   if(Strategy == Strategy_IgnoreGaussian)
   {
      result.X0 = -1000;
      result.SigmaL = -1000;
      result.SigmaR = -1000;
      result.S = -NegativeS.getVal();
      result.SError = S.getError();
      result.Strategy = Strategy;
   }

   if(Model != NULL)
      delete Model;

   return result;
}

void FitWithRCut(PsFileHelper &PsFile, string Filename, vector<double> RCuts)
{
   if(RCuts.size() == 0)
      return;
   if(RCuts.size() == 1)
   {
      cout << "If you want to fit with just one R-cut, use the easy version!" << endl;
      return;
   }

   sort(RCuts.begin(), RCuts.end());

   vector<SingleFitResult> SingleFitResults;
   for(int i = 0; i < (int)RCuts.size(); i++)
      SingleFitResults.push_back(FitWithRCut(PsFile, Filename, RCuts[i], Strategy_Normal));

   PsFile.AddTextPage("Putting everything together");

   TGraphErrors SingleCentralValues;
   SingleCentralValues.SetNameTitle("SingleCentralValues", "Central values from single fits");
   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      SingleCentralValues.SetPoint(i, RCuts[i] * RCuts[i], SingleFitResults[i].S);
      SingleCentralValues.SetPointError(i, 0, SingleFitResults[i].SError);
   }

   TF1 InitialParameterValues("InitialParameterValues", "pol1");
   SingleCentralValues.Fit(&InitialParameterValues);
   InitialParameterValues.SetLineWidth(1);

   double ParameterAValue = InitialParameterValues.GetParameter(0);
   double ParameterBValue = InitialParameterValues.GetParameter(1);

   SingleCentralValues.SetLineWidth(1);
   SingleCentralValues.SetMarkerStyle(8);
   PsFile.AddPlot(SingleCentralValues, "apx");
   
   SingleCentralValues.SetLineWidth(2);
   SingleCentralValues.SetMarkerStyle(1);
   PsFile.AddPlot(SingleCentralValues, "ap");
   
   vector<string> SingleFitParameterExplanation;
   stringstream ParameterAString;
   ParameterAString << "Parameter A from single fit: " << ParameterAValue << " +- "
      << InitialParameterValues.GetParError(0);
   SingleFitParameterExplanation.push_back(ParameterAString.str());
   stringstream ParameterBString;
   ParameterBString << "Parameter B from single fit: " << ParameterBValue << " +- "
      << InitialParameterValues.GetParError(1);
   SingleFitParameterExplanation.push_back(ParameterBString.str());
   SingleFitParameterExplanation.push_back("Friendly reminder: s = a + b * (R cut)^2");
   SingleFitParameterExplanation.push_back("Exponential part in the fit is exp(-s * MR)");
   PsFile.AddTextPage(SingleFitParameterExplanation);

   TFile F(Filename.c_str());
   TTree *Tree = (TTree *)F.Get("MRTree");
   if(Tree == NULL)
      return;

   RooRealVar MR("MR", "Chris' kinematic variable", 150, 1500, "GeV");
   RooRealVar R("R", "Chris' another kinematic variable", -200, 200);
   RooArgSet TreeVarSet(MR, R);

   RooDataSet Dataset("Dataset", "MR Dataset", Tree, TreeVarSet, Form("R > %f", RCuts[0]));

   RooRealVar ParameterA("ParameterA", "s = \"a\" + b R^2", ParameterAValue, 0, 0.1);
   RooRealVar ParameterB("ParameterB", "s = a + \"b\" R^2", ParameterBValue, 0, 1);

   vector<RooRealVar *> X0;
   vector<RooRealVar *> SigmaL;
   vector<RooRealVar *> SigmaR;
   vector<RooFormulaVar *> S;
   vector<RooRealVar *> Yields;
   vector<RooFormulaVar *> NegativeYields;
   vector<RooFormulaVar *> NormalizedYields;
   vector<RooFormulaVar *> NormalizedNegativeYields;

   vector<RooAbsPdf *> Models;

   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      double GuessYield = Dataset.reduce(Cut(Form("R > %f", RCuts[i])))->sumEntries();

      if(SingleFitResults[i].Strategy == Strategy_Normal)
      {
         X0.push_back(new RooRealVar(Form("X0_%d", i),
            Form("gaussian mean, R > %f", RCuts[i]), SingleFitResults[i].X0, 0, 500, "GeV"));
         SigmaL.push_back(new RooRealVar(Form("SigmaL_%d", i),
            Form("left-hand side gaussian width, R > %f", RCuts[i]), SingleFitResults[i].SigmaL, 0, 500, "GeV"));
         SigmaR.push_back(new RooRealVar(Form("SigmaR_%d", i),
            Form("right-hand side gaussian width, R > %f", RCuts[i]), SingleFitResults[i].SigmaR, 0, 500, "GeV"));
         S.push_back(new RooFormulaVar(Form("S_%d", i),
            Form("exponent of the tail, R > %f", RCuts[i]),
            Form("@0 + %f * @1", RCuts[i] * RCuts[i]), RooArgList(ParameterA, ParameterB)));
         Yields.push_back(new RooRealVar(Form("Yield_%d", i),
            Form("Yield with R above %f", RCuts[i]), GuessYield));
         NegativeYields.push_back(new RooFormulaVar(Form("NYield_%d", i),
            "Negative yield", "-1 * @0", RooArgList(*Yields[i])));

         Models.push_back(new RooTwoSideGaussianWithAnExponentialTail(Form("Model_%d", i),
            Form("Model for R > %f", RCuts[i]), MR, *X0[i], *SigmaL[i], *SigmaR[i], *S[i]));
      }
      else if(SingleFitResults[i].Strategy == Strategy_IgnoreLeft)
      {
         X0.push_back(new RooRealVar(Form("X0_%d", i),
            Form("gaussian mean, R > %f", RCuts[i]), SingleFitResults[i].X0, 0, 500, "GeV"));
         SigmaL.push_back(NULL);
         SigmaR.push_back(new RooRealVar(Form("SigmaR_%d", i),
            Form("right-hand side gaussian width, R > %f", RCuts[i]), SingleFitResults[i].SigmaR, 0, 5000, "GeV"));
         S.push_back(new RooFormulaVar(Form("S_%d", i),
            Form("exponent of the tail, R > %f", RCuts[i]),
            Form("@0 + %f * @1", RCuts[i] * RCuts[i]), RooArgList(ParameterA, ParameterB)));
         Yields.push_back(new RooRealVar(Form("Yield_%d", i),
            Form("Yield with R above %f", RCuts[i]), GuessYield));
         NegativeYields.push_back(new RooFormulaVar(Form("NYield_%d", i),
            "Negative yield", "-1 * @0", RooArgList(*Yields[i])));

         Models.push_back(new RooTwoSideGaussianWithAnExponentialTail(Form("Model_%d", i),
            Form("Model for R > %f", RCuts[i]), MR, *X0[i], *SigmaR[i], *SigmaR[i], *S[i]));
      }
      else   // if SingleFitResults[i].Strategy == Strategy_IgnoreGaussian
      {
         X0.push_back(NULL);
         SigmaL.push_back(NULL);
         SigmaR.push_back(NULL);
         S.push_back(new RooFormulaVar(Form("S_%d", i),
            Form("exponent of the tail, R > %f", RCuts[i]),
            Form("-1 * @0 - %f * @1", RCuts[i] * RCuts[i]), RooArgList(ParameterA, ParameterB)));
         Yields.push_back(new RooRealVar(Form("Yield_%d", i),
            Form("Yield with R above %f", RCuts[i]), GuessYield));
         NegativeYields.push_back(new RooFormulaVar(Form("NYield_%d", i),
            "Negative yield", "-1 * @0", RooArgList(*Yields[i])));

         Models.push_back(new RooExponential(Form("Model_%d", i), Form("Model for R > %f", RCuts[i]),
            MR, *S[i]));
      }
   }

   for(int i = 0; i < (int)RCuts.size() - 1; i++)
   {
      NormalizedYields.push_back(new RooFormulaVar(Form("NormalizedYield_%d", i),
         "Yield", "@0 / (@0 - @1)", RooArgList(*Yields[i], *Yields[i+1])));
      NormalizedNegativeYields.push_back(new RooFormulaVar(Form("NormalizedNegativeYield_%d", i),
         "Yield", "-@1 / (@0 - @1)", RooArgList(*Yields[i], *Yields[i+1])));
      
      // NormalizedYields.push_back(new RooFormulaVar(Form("NormalizedYield_%d", i),
      //    "Yield", "@0", RooArgList(*Yields[i], *Yields[i+1])));
      // NormalizedNegativeYields.push_back(new RooFormulaVar(Form("NormalizedNegativeYield_%d", i),
      //    "Yield", "-@1", RooArgList(*Yields[i], *Yields[i+1])));
   }

   vector<RooAbsPdf *> ModelBeforeConstraint;
   vector<RooAbsPdf *> Constraint;
   vector<RooAbsPdf *> TopLevelModels;
   vector<RooAbsReal *> TopLevelYields;

   RooArgList ModelList;
   RooArgList YieldList;

   for(int i = 0; i < (int)RCuts.size(); i++)   // ps. last bin is special
   {
      if(i == RCuts.size() - 1)
         ModelBeforeConstraint.push_back(Models[i]);
      else
         ModelBeforeConstraint.push_back(new RooAddPdf(Form("ModelBeforeConstraint_%d", i),
            Form("Model before constraint (bin %d)", i), RooArgList(*Models[i], *Models[i+1]),
            RooArgList(*NormalizedYields[i], *NormalizedNegativeYields[i])));

      if(i == RCuts.size() - 1)
         Constraint.push_back(new RooAtLeast(Form("Constraint_%d", i), "Last bin constraint", R, RCuts[i]));
      else
         Constraint.push_back(new RooSameAs(Form("Constraint_%d", i),
            Form("Constraint R = %f - %f", RCuts[i], RCuts[i+1]), R,
            (RCuts[i+1] + RCuts[i]) / 2, (RCuts[i+1] - RCuts[i]) / 2));

      if(i == RCuts.size() - 1)
         TopLevelModels.push_back(new RooProdPdf(Form("TopLevelModel_%d", i),
            Form("Top level model for bin with R > %f", RCuts[i]),
            RooArgList(*ModelBeforeConstraint[i], *Constraint[i])));
      else
         TopLevelModels.push_back(new RooProdPdf(Form("TopLevelModel_%d", i),
            Form("Top level model for bin with R = %f - %f", RCuts[i], RCuts[i+1]),
            RooArgList(*ModelBeforeConstraint[i], *Constraint[i])));

      if(i == RCuts.size() - 1)
         TopLevelYields.push_back(Yields[i]);
      else
         TopLevelYields.push_back(new RooFormulaVar(Form("BinYield_%d", i), "Bin yield variable",
            "@0 - @1", RooArgList(*Yields[i], *Yields[i+1])));
      
      // if(i == RCuts.size() - 1)
      //    TopLevelYields.push_back(Yields[i]);
      // else
      //    TopLevelYields.push_back(new RooRealVar(Form("DummyYield_%d", i), "Dummy variable", 1));

      ModelList.add(*TopLevelModels[i]);
      YieldList.add(*TopLevelYields[i]);
   }

   RooAddPdf FinalModel("Final model", "Final model!", ModelList, YieldList);

   RooFitResult *FitResult = FinalModel.fitTo(Dataset, Save(true));

   PsFile.AddTextPage("Simultaneous fit!");

   RooArgSet SigmaLList, SigmaRList, X0List;

   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      if(SigmaL[i] != NULL)
         SigmaLList.add(*SigmaL[i]);
      if(SigmaR[i] != NULL)
         SigmaRList.add(*SigmaR[i]);
      if(X0[i] != NULL)
         X0List.add(*X0[i]);
   }

   // fit result - sigma l
   RooPlot *ParameterPlotSigmaL = MR.frame(Bins(50), Name("FitParametersSigmaL"),
      Title("Fit parameters - SigmaL"));
   FinalModel.paramOn(ParameterPlotSigmaL, Format("NELU", AutoPrecision(3)), Layout(0.1, 0.4, 0.9),
      Parameters(SigmaLList));
   PsFile.AddPlot(ParameterPlotSigmaL, "", false);
   
   // fit result - sigma r
   RooPlot *ParameterPlotSigmaR = MR.frame(Bins(50), Name("FitParametersSigmaR"),
      Title("Fit parameters - SigmaR"));
   FinalModel.paramOn(ParameterPlotSigmaR, Format("NELU", AutoPrecision(3)), Layout(0.1, 0.4, 0.9),
      Parameters(SigmaRList));
   PsFile.AddPlot(ParameterPlotSigmaR, "", false);
   
   // fit result - x0
   RooPlot *ParameterPlotX0 = MR.frame(Bins(50), Name("FitParametersX0"),
      Title("Fit parameters - X0"));
   FinalModel.paramOn(ParameterPlotX0, Format("NELU", AutoPrecision(3)), Layout(0.1, 0.4, 0.9),
      Parameters(X0List));
   PsFile.AddPlot(ParameterPlotX0, "", false);

   // Total model vs. total data
   RooPlot *FinalResultPlot = MR.frame(Bins(50), Name("FitResults"), Title("MR"));
   Dataset.plotOn(FinalResultPlot, MarkerSize(0.9));
   FinalModel.plotOn(FinalResultPlot, LineColor(kBlue));
   PsFile.AddPlot(FinalResultPlot, "", true);

   // Each bin, model vs. data
   RooPlot *FinalResultPlot2 = MR.frame(Bins(50), Name("FitResults2"), Title("MR"));
   Dataset.plotOn(FinalResultPlot2, MarkerSize(0.9), MarkerColor(1));
   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      RooArgList ComponentsToAdd;
      for(int j = i; j < (int)RCuts.size(); j++)
         ComponentsToAdd.add(*TopLevelModels[j]);
      FinalModel.plotOn(FinalResultPlot2, LineColor(kBlue), Components(ComponentsToAdd), LineColor(i + 1));
   }
   for(int i = 1; i < (int)RCuts.size(); i++)
      Dataset.reduce(Cut(Form("R > %f", RCuts[i])))->plotOn(FinalResultPlot2, MarkerSize(0.9), MarkerColor(i + 1));

   TCanvas FinalResultCanvas2;

   FinalResultPlot2->Draw();

   vector<TH1D *> DummyHistograms;   
   TLegend legend(0.65, 0.95, 0.95, 0.75);
   legend.SetFillColor(0);
   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      DummyHistograms.push_back(new TH1D(Form("DummyHistogram_%d", i), "Dummy!", 2, 0, 1));
      DummyHistograms[i]->SetLineColor(i + 1);
      DummyHistograms[i]->SetLineWidth(2);
      legend.AddEntry(DummyHistograms[i], Form("R > %f", RCuts[i]), "l");
   }
   legend.Draw();

   FinalResultCanvas2.SetLogy();

   PsFile.AddCanvas(FinalResultCanvas2);

   for(int i = 0; i < (int)RCuts.size(); i++)
      delete DummyHistograms[i];
   DummyHistograms.clear();
   
   // simultaneous fit vs. normal fit
   TGraphErrors ABBand;
   ABBand.SetNameTitle("ABBand", "Comparison between simple fit and simultaneous fit");
   int ABBandCounter = 0;
   double R2Begin = RCuts[0] * RCuts[0] - 0.1;
   double R2End = RCuts[RCuts.size()-1] * RCuts[RCuts.size()-1] + 0.1;
   for(double x = R2Begin; x <= R2End; x = x + (R2End - R2Begin) / 100)
   {
      double Correlation = FitResult->correlation("ParameterA", "ParameterB");
      double Value = ParameterA.getVal() + ParameterB.getVal() * x;
      double Error = sqrt(ParameterA.getError() * ParameterA.getError()
         + ParameterB.getError() * ParameterB.getError() * x * x
         + ParameterA.getError() * ParameterB.getError() * x * Correlation);

      ABBand.SetPoint(ABBandCounter, x, Value);
      ABBand.SetPointError(ABBandCounter, 0, Error);
      ABBandCounter = ABBandCounter + 1;
   }

   TCanvas ABBandCanvas;
   ABBand.SetFillColor(kYellow);
   ABBand.Draw("a3");
   SingleCentralValues.Draw("px");
   PsFile.AddCanvas(ABBandCanvas);
   
   TCanvas ABBandCanvas2;
   ABBand.SetFillColor(kYellow);
   ABBand.Draw("a3");
   SingleCentralValues.Draw("p");
   PsFile.AddCanvas(ABBandCanvas2);

   // parameters a and b
   vector<string> FinalResultExplanation;
   stringstream FitParameterAStream;
   FitParameterAStream << "Parameter A from simultaneous fit = "
      << ParameterA.getVal() << " +- " << ParameterA.getError();
   FinalResultExplanation.push_back(FitParameterAStream.str());
   stringstream FitParameterBStream;
   FitParameterBStream << "Parameter B from simultaneous fit = "
      << ParameterB.getVal() << " +- " << ParameterB.getError();
   FinalResultExplanation.push_back(FitParameterBStream.str());
   stringstream FitParameterCorrelationStream;
   FitParameterCorrelationStream << "Correlation = " << FitResult->correlation("ParameterA", "ParameterB");
   FinalResultExplanation.push_back(FitParameterCorrelationStream.str());
   FinalResultExplanation.push_back("");
   FinalResultExplanation.push_back(ParameterAString.str());
   FinalResultExplanation.push_back(ParameterBString.str());
   PsFile.AddTextPage(FinalResultExplanation);
   
   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      delete TopLevelModels[i];
      if(i != (int)RCuts.size() - 1)
         delete ModelBeforeConstraint[i];
      delete Constraint[i];
      if(i != (int)RCuts.size() - 1)
         delete TopLevelYields[i];

      if(i != (int)RCuts.size() - 1)
      {
         delete NormalizedYields[i];
         delete NormalizedNegativeYields[i];
      }

      if(Models[i] != NULL)   delete Models[i];
      if(Yields[i] != NULL)   delete Yields[i];
      if(S[i] != NULL)        delete S[i];
      if(SigmaR[i] != NULL)   delete SigmaR[i];
      if(SigmaL[i] != NULL)   delete SigmaL[i];
      if(X0[i] != NULL)       delete X0[i];
   }

   TopLevelModels.clear();
   ModelBeforeConstraint.clear();
   Constraint.clear();
   TopLevelYields.clear();

   NormalizedYields.clear();
   NormalizedNegativeYields.clear();

   Models.clear();
   Yields.clear();
   S.clear();
   SigmaR.clear();
   SigmaL.clear();
   X0.clear();
}





