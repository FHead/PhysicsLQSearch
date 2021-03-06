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
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TH2D.h"
#include "TH1D.h"

#include "RooRealVar.h"
#include "RooDataSet.h"
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
#include "RooEllipse.h"
using namespace RooFit;

#include "PlotHelper2.h"
#include "DrawRandom.h"
#include "SetStyle.h"
#include "DataHelper.h"

int main(int argc, char *argv[]);
void FitWithRCut(PsFileHelper &PsFile, string FileName, vector<double> RCuts,
   string WResult, string TResult, string QCDResult);

int main(int argc, char *argv[])
{
   SetStyle();

   vector<double> CutsToTry;
   // CutsToTry.push_back(sqrt(0.20));
   CutsToTry.push_back(sqrt(0.25));
   CutsToTry.push_back(sqrt(0.30));
   CutsToTry.push_back(sqrt(0.35));
   CutsToTry.push_back(sqrt(0.38));
   CutsToTry.push_back(sqrt(0.42));
   CutsToTry.push_back(sqrt(0.50));

   PsFileHelper PsFile("OverlayWithData.ps");
   PsFile.AddTextPage("Overlay with data!");

   FitWithRCut(PsFile, "DataPlots.root", CutsToTry, "WFitResult.dh", "TTbarFitResult.dh", "QCDFitResult.dh");

   PsFile.AddTimeStampPage();
   PsFile.Close();
}

void FitWithRCut(PsFileHelper &PsFile, string FileName, vector<double> RCuts,
   string WResult, string TResult, string QCDResult)
{
   DataHelper WDHFile(WResult);
   DataHelper TDHFile(TResult);
   DataHelper QCDDHFile(QCDResult);

   sort(RCuts.begin(), RCuts.end());

   TFile F(FileName.c_str());
   TTree *Tree = (TTree *)F.Get("MRTree");
   if(Tree == NULL)
      return;

   double MRLowerBound = 400;
   double MRUpperBound = 3500;

   string MRString = "MR";
   string RString = "R";

   RooRealVar MR(MRString.c_str(), "M_{R}", MRLowerBound, MRUpperBound, "GeV");
   RooRealVar R(RString.c_str(), "R", 0, 1.5);
   RooArgSet TreeVarSet(MR, R);

   RooDataSet Dataset("Dataset", "MR Dataset", Tree, TreeVarSet, Form("%s > %f && MR > 400", RString.c_str(), RCuts[0]));

   RooRealVar ParameterA1("ParameterA1", "s1 = \"a1\" + b1 R^2", QCDDHFile["CentralValue"]["ParameterA1"].GetDouble());
   RooRealVar ParameterB1("ParameterB1", "s1 = a1 + \"b1\" R^2", QCDDHFile["CentralValue"]["ParameterB1"].GetDouble());
   RooRealVar ParameterA2("ParameterA2", "s2 = \"a2\" + b2 R^2", QCDDHFile["CentralValue"]["ParameterA2"].GetDouble());
   RooRealVar ParameterB2("ParameterB2", "s2 = a2 + \"b2\" R^2", QCDDHFile["CentralValue"]["ParameterB2"].GetDouble());

   RooRealVar WParameterA1("WParameterA1", "s1 = \"a1\" + b1 R^2", WDHFile["CentralValue"]["ParameterA1"].GetDouble());
   RooRealVar WParameterB1("WParameterB1", "s1 = a1 + \"b1\" R^2", WDHFile["CentralValue"]["ParameterB1"].GetDouble());
   RooRealVar WParameterA2("WParameterA2", "s2 = \"a2\" + b2 R^2", WDHFile["CentralValue"]["ParameterA2"].GetDouble());
   RooRealVar WParameterB2("WParameterB2", "s2 = a2 + \"b2\" R^2", WDHFile["CentralValue"]["ParameterB2"].GetDouble());

   RooRealVar TParameterA1("TParameterA1", "s1 = \"a1\" + b1 R^2", TDHFile["CentralValue"]["ParameterA1"].GetDouble());
   RooRealVar TParameterB1("TParameterB1", "s1 = a1 + \"b1\" R^2", TDHFile["CentralValue"]["ParameterB1"].GetDouble());
   RooRealVar TParameterA2("TParameterA2", "s2 = \"a2\" + b2 R^2", TDHFile["CentralValue"]["ParameterA2"].GetDouble());
   RooRealVar TParameterB2("TParameterB2", "s2 = a2 + \"b2\" R^2", TDHFile["CentralValue"]["ParameterB2"].GetDouble());

   vector<RooFormulaVar *> S1;
   vector<RooFormulaVar *> S2;
   vector<RooRealVar *> SingleBinYields;
   vector<RooFormulaVar *> Yields;
   vector<RooFormulaVar *> NegativeYields;
   vector<RooFormulaVar *> NormalizedYields;
   vector<RooFormulaVar *> NormalizedNegativeYields;

   vector<RooFormulaVar *> WS1;
   vector<RooFormulaVar *> WS2;
   vector<RooRealVar *> WSingleBinYields;
   vector<RooFormulaVar *> WYields;
   vector<RooFormulaVar *> WNegativeYields;
   vector<RooFormulaVar *> WNormalizedYields;
   vector<RooFormulaVar *> WNormalizedNegativeYields;

   vector<RooFormulaVar *> TS1;
   vector<RooFormulaVar *> TS2;
   vector<RooRealVar *> TSingleBinYields;
   vector<RooFormulaVar *> TYields;
   vector<RooFormulaVar *> TNegativeYields;
   vector<RooFormulaVar *> TNormalizedYields;
   vector<RooFormulaVar *> TNormalizedNegativeYields;

   vector<RooAbsPdf *> Component1Models;
   vector<RooAbsPdf *> Component2Models;
   vector<RooAbsPdf *> Models;

   vector<RooAbsPdf *> WComponent1Models;
   vector<RooAbsPdf *> WComponent2Models;
   vector<RooAbsPdf *> WModels;

   vector<RooAbsPdf *> TComponent1Models;
   vector<RooAbsPdf *> TComponent2Models;
   vector<RooAbsPdf *> TModels;

   vector<RooRealVar *> Fraction;
   vector<RooRealVar *> WFraction;
   vector<RooRealVar *> TFraction;

   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      SingleBinYields.push_back(new RooRealVar(Form("SingleBinYield_%d", i),
         Form("Yield with R in bin %d", i), QCDDHFile["CentralValue"][Form("SingleBinYield_%d", i + 2)].GetDouble()));
      TSingleBinYields.push_back(new RooRealVar(Form("TSingleBinYield_%d", i),
         Form("Yield with R in bin %d", i), TDHFile["CentralValue"][Form("SingleBinYield_%d", i + 2)].GetDouble()));
      WSingleBinYields.push_back(new RooRealVar(Form("WSingleBinYield_%d", i),
         Form("Yield with R in bin %d", i), WDHFile["CentralValue"][Form("SingleBinYield_%d", i + 4)].GetDouble()));
   }

   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      S1.push_back(new RooFormulaVar(Form("S1_%d", i),
         Form("exponent 1 of the tail, R > %f", RCuts[i]),
         Form("-1 * @0 - %f * @1", RCuts[i] * RCuts[i]), RooArgList(ParameterA1, ParameterB1)));
      S2.push_back(new RooFormulaVar(Form("S2_%d", i),
         Form("exponent 2 of the tail, R > %f", RCuts[i]),
         Form("-1 * @0 - %f * @1", RCuts[i] * RCuts[i]), RooArgList(ParameterA2, ParameterB2)));
      WS1.push_back(new RooFormulaVar(Form("WS1_%d", i),
         Form("exponent 1 of the tail, R > %f", RCuts[i]),
         Form("-1 * @0 - %f * @1", RCuts[i] * RCuts[i]), RooArgList(WParameterA1, WParameterB1)));
      WS2.push_back(new RooFormulaVar(Form("WS2_%d", i),
         Form("exponent 2 of the tail, R > %f", RCuts[i]),
         Form("-1 * @0 - %f * @1", RCuts[i] * RCuts[i]), RooArgList(WParameterA2, WParameterB2)));
      TS1.push_back(new RooFormulaVar(Form("TS1_%d", i),
         Form("exponent 1 of the tail, R > %f", RCuts[i]),
         Form("-1 * @0 - %f * @1", RCuts[i] * RCuts[i]), RooArgList(TParameterA1, TParameterB1)));
      TS2.push_back(new RooFormulaVar(Form("TS2_%d", i),
         Form("exponent 2 of the tail, R > %f", RCuts[i]),
         Form("-1 * @0 - %f * @1", RCuts[i] * RCuts[i]), RooArgList(TParameterA2, TParameterB2)));

      Component1Models.push_back(new RooExponential(Form("Model1_%d", i), Form("First model for R > %f", RCuts[i]),
         MR, *S1[i]));
      Component2Models.push_back(new RooExponential(Form("Model2_%d", i), Form("Second model for R > %f", RCuts[i]),
         MR, *S2[i]));
      WComponent1Models.push_back(new RooExponential(Form("WModel1_%d", i), Form("First model for R > %f", RCuts[i]),
         MR, *WS1[i]));
      WComponent2Models.push_back(new RooExponential(Form("WModel2_%d", i), Form("Second model for R > %f", RCuts[i]),
         MR, *WS2[i]));
      TComponent1Models.push_back(new RooExponential(Form("TModel1_%d", i), Form("First model for R > %f", RCuts[i]),
         MR, *TS1[i]));
      TComponent2Models.push_back(new RooExponential(Form("TModel2_%d", i), Form("Second model for R > %f", RCuts[i]),
         MR, *TS2[i]));

      Fraction.push_back(new RooRealVar(Form("Fraction_%d", i), Form("Fraction_%d", i),
         QCDDHFile["CentralValue"][Form("Fraction_%d", i + 2)].GetDouble()));
      WFraction.push_back(new RooRealVar(Form("WFraction_%d", i), Form("WFraction_%d", i),
         WDHFile["CentralValue"][Form("Fraction_%d", i + 4)].GetDouble()));
      TFraction.push_back(new RooRealVar(Form("TFraction_%d", i), Form("TFraction_%d", i),
         TDHFile["CentralValue"][Form("Fraction_%d", i + 2)].GetDouble()));

      Models.push_back(new RooAddPdf(Form("Model_%d", i), Form("Total model for R > %f", RCuts[i]),
         *Component1Models[i], *Component2Models[i], *Fraction[i]));
      WModels.push_back(new RooAddPdf(Form("WModel_%d", i), Form("Total model for R > %f", RCuts[i]),
         *WComponent1Models[i], *WComponent2Models[i], *WFraction[i]));
      TModels.push_back(new RooAddPdf(Form("TModel_%d", i), Form("Total model for R > %f", RCuts[i]),
         *TComponent1Models[i], *TComponent2Models[i], *TFraction[i]));
   }

   Yields.push_back(new RooFormulaVar(Form("Yield_%d", RCuts.size() - 1), "Last bin yield",
      "@0", RooArgList(*SingleBinYields[SingleBinYields.size()-1])));
   for(int i = (int)RCuts.size() - 1 - 1; i >= 0; i--)
      Yields.push_back(new RooFormulaVar(Form("Yield_%d", i), Form("Yield with R above %f", RCuts[i]),
         "@0 + @1", RooArgList(*Yields[Yields.size()-1], *SingleBinYields[i])));
   reverse(Yields.begin(), Yields.end());

   WYields.push_back(new RooFormulaVar(Form("WYield_%d", RCuts.size() - 1), "Last bin yield",
      "@0", RooArgList(*WSingleBinYields[WSingleBinYields.size()-1])));
   for(int i = (int)RCuts.size() - 1 - 1; i >= 0; i--)
      WYields.push_back(new RooFormulaVar(Form("WYield_%d", i), Form("Yield with R above %f", RCuts[i]),
         "@0 + @1", RooArgList(*WYields[WYields.size()-1], *WSingleBinYields[i])));
   reverse(WYields.begin(), WYields.end());

   TYields.push_back(new RooFormulaVar(Form("TYield_%d", RCuts.size() - 1), "Last bin yield",
      "@0", RooArgList(*TSingleBinYields[TSingleBinYields.size()-1])));
   for(int i = (int)RCuts.size() - 1 - 1; i >= 0; i--)
      TYields.push_back(new RooFormulaVar(Form("TYield_%d", i), Form("Yield with R above %f", RCuts[i]),
         "@0 + @1", RooArgList(*TYields[TYields.size()-1], *TSingleBinYields[i])));
   reverse(TYields.begin(), TYields.end());

   for(int i = 0; i < (int)RCuts.size(); i++)
      cout << "Yields[" << i << "] has initial value " << Yields[i]->getVal() << endl;

   for(int i = 0; i < (int)RCuts.size(); i++)
      NegativeYields.push_back(new RooFormulaVar(Form("NYield_%d", i),
         "Negative yield", "-1 * @0", RooArgList(*Yields[i])));
   for(int i = 0; i < (int)RCuts.size(); i++)
      WNegativeYields.push_back(new RooFormulaVar(Form("WNYield_%d", i),
         "Negative yield", "-1 * @0", RooArgList(*WYields[i])));
   for(int i = 0; i < (int)RCuts.size(); i++)
      TNegativeYields.push_back(new RooFormulaVar(Form("TNYield_%d", i),
         "Negative yield", "-1 * @0", RooArgList(*TYields[i])));

   for(int i = 0; i < (int)RCuts.size() - 1; i++)
   {
      NormalizedYields.push_back(new RooFormulaVar(Form("NormalizedYield_%d", i),
         "Yield", "@0 / (@0 - @1)", RooArgList(*Yields[i], *Yields[i+1])));
      NormalizedNegativeYields.push_back(new RooFormulaVar(Form("NormalizedNegativeYield_%d", i),
         "Yield", "-@1 / (@0 - @1)", RooArgList(*Yields[i], *Yields[i+1])));
      WNormalizedYields.push_back(new RooFormulaVar(Form("WNormalizedYield_%d", i),
         "Yield", "@0 / (@0 - @1)", RooArgList(*WYields[i], *WYields[i+1])));
      WNormalizedNegativeYields.push_back(new RooFormulaVar(Form("WNormalizedNegativeYield_%d", i),
         "Yield", "-@1 / (@0 - @1)", RooArgList(*WYields[i], *WYields[i+1])));
      TNormalizedYields.push_back(new RooFormulaVar(Form("TNormalizedYield_%d", i),
         "Yield", "@0 / (@0 - @1)", RooArgList(*TYields[i], *TYields[i+1])));
      TNormalizedNegativeYields.push_back(new RooFormulaVar(Form("TNormalizedNegativeYield_%d", i),
         "Yield", "-@1 / (@0 - @1)", RooArgList(*TYields[i], *TYields[i+1])));
   }

   vector<RooAbsPdf *> ModelBeforeConstraint;
   vector<RooAbsPdf *> Constraint;
   vector<RooAbsPdf *> TopLevelModels;
   vector<RooAbsReal *> TopLevelYields;

   vector<RooAbsPdf *> WModelBeforeConstraint;
   vector<RooAbsPdf *> WConstraint;
   vector<RooAbsPdf *> WTopLevelModels;
   vector<RooAbsReal *> WTopLevelYields;

   vector<RooAbsPdf *> TModelBeforeConstraint;
   vector<RooAbsPdf *> TConstraint;
   vector<RooAbsPdf *> TTopLevelModels;
   vector<RooAbsReal *> TTopLevelYields;

   RooArgList ModelList;
   RooArgList YieldList;
   RooArgList WModelList;
   RooArgList WYieldList;
   RooArgList TModelList;
   RooArgList TYieldList;

   for(int i = 0; i < (int)RCuts.size(); i++)   // ps. last bin is special
   {
      if(i == RCuts.size() - 1)
         ModelBeforeConstraint.push_back(Models[i]);
      else
         ModelBeforeConstraint.push_back(new RooAddPdf(Form("ModelBeforeConstraint_%d", i),
            Form("Model before constraint (bin %d)", i), RooArgList(*Models[i], *Models[i+1]),
            RooArgList(*NormalizedYields[i], *NormalizedNegativeYields[i])));

      if(i == RCuts.size() - 1)
         WModelBeforeConstraint.push_back(WModels[i]);
      else
         WModelBeforeConstraint.push_back(new RooAddPdf(Form("WModelBeforeConstraint_%d", i),
            Form("Model before constraint (bin %d)", i), RooArgList(*WModels[i], *WModels[i+1]),
            RooArgList(*WNormalizedYields[i], *WNormalizedNegativeYields[i])));

      if(i == RCuts.size() - 1)
         TModelBeforeConstraint.push_back(TModels[i]);
      else
         TModelBeforeConstraint.push_back(new RooAddPdf(Form("TModelBeforeConstraint_%d", i),
            Form("Model before constraint (bin %d)", i), RooArgList(*TModels[i], *TModels[i+1]),
            RooArgList(*TNormalizedYields[i], *TNormalizedNegativeYields[i])));

      if(i == RCuts.size() - 1)
         Constraint.push_back(new RooAtLeast(Form("Constraint_%d", i), "Last bin constraint", R, RCuts[i]));
      else
         Constraint.push_back(new RooSameAs(Form("Constraint_%d", i),
            Form("Constraint R = %f - %f", RCuts[i], RCuts[i+1]), R,
            (RCuts[i+1] + RCuts[i]) / 2, (RCuts[i+1] - RCuts[i]) / 2));

      if(i == RCuts.size() - 1)
         WConstraint.push_back(new RooAtLeast(Form("WConstraint_%d", i), "Last bin constraint", R, RCuts[i]));
      else
         WConstraint.push_back(new RooSameAs(Form("WConstraint_%d", i),
            Form("Constraint R = %f - %f", RCuts[i], RCuts[i+1]), R,
            (RCuts[i+1] + RCuts[i]) / 2, (RCuts[i+1] - RCuts[i]) / 2));

      if(i == RCuts.size() - 1)
         TConstraint.push_back(new RooAtLeast(Form("TConstraint_%d", i), "Last bin constraint", R, RCuts[i]));
      else
         TConstraint.push_back(new RooSameAs(Form("TConstraint_%d", i),
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
         WTopLevelModels.push_back(new RooProdPdf(Form("WTopLevelModel_%d", i),
            Form("Top level model for bin with R > %f", RCuts[i]),
            RooArgList(*WModelBeforeConstraint[i], *WConstraint[i])));
      else
         WTopLevelModels.push_back(new RooProdPdf(Form("WTopLevelModel_%d", i),
            Form("Top level model for bin with R = %f - %f", RCuts[i], RCuts[i+1]),
            RooArgList(*WModelBeforeConstraint[i], *WConstraint[i])));

      if(i == RCuts.size() - 1)
         TTopLevelModels.push_back(new RooProdPdf(Form("TTopLevelModel_%d", i),
            Form("Top level model for bin with R > %f", RCuts[i]),
            RooArgList(*TModelBeforeConstraint[i], *TConstraint[i])));
      else
         TTopLevelModels.push_back(new RooProdPdf(Form("TTopLevelModel_%d", i),
            Form("Top level model for bin with R = %f - %f", RCuts[i], RCuts[i+1]),
            RooArgList(*TModelBeforeConstraint[i], *TConstraint[i])));

      if(i == RCuts.size() - 1)
         TopLevelYields.push_back(Yields[i]);
      else
         TopLevelYields.push_back(new RooFormulaVar(Form("BinYield_%d", i), "Bin yield variable",
            "@0 - @1", RooArgList(*Yields[i], *Yields[i+1])));

      if(i == RCuts.size() - 1)
         WTopLevelYields.push_back(WYields[i]);
      else
         WTopLevelYields.push_back(new RooFormulaVar(Form("WBinYield_%d", i), "Bin yield variable",
            "@0 - @1", RooArgList(*WYields[i], *WYields[i+1])));

      if(i == RCuts.size() - 1)
         TTopLevelYields.push_back(TYields[i]);
      else
         TTopLevelYields.push_back(new RooFormulaVar(Form("TBinYield_%d", i), "Bin yield variable",
            "@0 - @1", RooArgList(*TYields[i], *TYields[i+1])));

      ModelList.add(*TopLevelModels[i]);
      YieldList.add(*TopLevelYields[i]);
      WModelList.add(*WTopLevelModels[i]);
      WYieldList.add(*WTopLevelYields[i]);
      TModelList.add(*TTopLevelModels[i]);
      TYieldList.add(*TTopLevelYields[i]);
   }

   RooAddPdf AlmostFinalModel("AlmostFinalModel", "Final model!", ModelList, YieldList);
   RooAddPdf WFinalModel("WFinalModel", "Final model!", WModelList, WYieldList);
   RooAddPdf TFinalModel("TFinalModel", "Final model!", TModelList, TYieldList);

   RooRealVar WFinalRawYield("WFinalRawYield", "WFinalYield", 95.73);
   RooRealVar TTbarFinalRawYield("TTbarFinalRawYield", "TTbarFinalYield", 120.5);
   RooRealVar QCDFinalRawYield("QCDFinalRawYield", "QCD final yield", 115.51);

   RooRealVar TotalScalingFactor("ScalingFactor", "ScalingFactor", 1.0, 0, 20);
   RooRealVar QCDScalingFactor("QCDScalingFactor", "QCDScalingFactor",
      QCDDHFile["CentralValue"]["QCDOverallFactor"].GetDouble());

   RooFormulaVar WFinalYield("WFinalYield", "WFinalYield", "@0*@1", RooArgList(WFinalRawYield, TotalScalingFactor));
   RooFormulaVar TTbarFinalYield("TTbarFinalYield", "TFinalYield", "@0*@1",
      RooArgList(TTbarFinalRawYield, TotalScalingFactor));
   RooFormulaVar QCDFinalYield("QCDFinalYield", "QCDFinalYield", "@0*@1*@2",
      RooArgList(QCDFinalRawYield, TotalScalingFactor, QCDScalingFactor));

   RooAddPdf FinalModel("FinalModel", "Final model!!",
      RooArgList(AlmostFinalModel, WFinalModel, TFinalModel),
      RooArgList(QCDFinalYield, WFinalYield, TTbarFinalYield));

   // Total model vs. total data
   RooPlot *FinalResultPlot = MR.frame(Bins(25), Name("FitResults"),
      Title("MR overlay, total model"), Range(400, 1400));
   Dataset.plotOn(FinalResultPlot, MarkerSize(1.8));
   TH1D HTotal("HTotal", "", 25, 400 ,1400);
   TH1D HW("HW", "", 25, 400 ,1400);
   TH1D HT("HT", "", 25, 400 ,1400);
   TH1D HQ("HQ", "", 25, 400 ,1400);
   FinalModel.fillHistogram(&HTotal, RooArgSet(MR), 326.976377);
   WFinalModel.fillHistogram(&HW, RooArgSet(MR), 104.689395 * 0.9);
   TFinalModel.fillHistogram(&HT, RooArgSet(MR), 131.77765 * 0.9);
   AlmostFinalModel.fillHistogram(&HQ, RooArgSet(MR), 126.32061 * 0.9);
   HTotal.SetLineColor(kBlue);
   HW.SetLineColor(kRed);
   HT.SetLineColor(kYellow - 3);
   HQ.SetLineColor(kGreen);
   HW.SetFillColor(kRed);
   HT.SetFillColor(kYellow - 3);
   HQ.SetFillColor(kGreen);
   HTotal.SetLineWidth(2);
   HW.SetLineWidth(2);
   HT.SetLineWidth(2);
   HQ.SetLineWidth(2);
   // FinalModel.plotOn(FinalResultPlot, LineColor(kBlue), Normalization(326.976 / 295));
   // FinalModel.plotOn(FinalResultPlot, LineColor(kGreen), Components("AlmostFinalModel"), Normalization(326.976 / 295));
   // FinalModel.plotOn(FinalResultPlot, LineColor(kRed), Components("WFinalModel"), Normalization(326.976 / 295));
   // FinalModel.plotOn(FinalResultPlot, LineColor(kYellow), Components("TFinalModel"), Normalization(326.976 / 295));
   FinalResultPlot->SetMinimum(0.5);
   PsFile.AddPlot(FinalResultPlot, "", false);
   PsFile.AddPlot(FinalResultPlot, "", true);

   RooPlot *FinalResultPlotRestricted = MR.frame(Bins(25), Name("FitResultsRestricted"),
      Title("MR overlay, total model, no R2 > 0.5"), Range(400, 1400));
   Dataset.reduce("R*R < 0.5")->plotOn(FinalResultPlotRestricted, MarkerSize(1.8));

   RooArgList ComponentsAlmostAll;
   RooArgList QCDComponentsAlmostAll;
   RooArgList WComponentsAlmostAll;
   RooArgList TTbarComponentsAlmostAll;

   for(int i = 0; i < (int)RCuts.size() - 1; i++)
   {
      ComponentsAlmostAll.add(*TopLevelModels[i]);
      ComponentsAlmostAll.add(*WTopLevelModels[i]);
      ComponentsAlmostAll.add(*TTopLevelModels[i]);
      QCDComponentsAlmostAll.add(*TopLevelModels[i]);
      WComponentsAlmostAll.add(*WTopLevelModels[i]);
      TTbarComponentsAlmostAll.add(*TTopLevelModels[i]);
   }

   // double Factor = Dataset.sumEntries("R*R > 0.2 && MR > 400")
   //    / Dataset.sumEntries("R*R < 0.5 && MR > 400 && R*R > 0.2");
   double Factor = 1;
   FinalModel.plotOn(FinalResultPlotRestricted, LineColor(kBlue), Components(ComponentsAlmostAll),
      Normalization(Factor));
   FinalModel.plotOn(FinalResultPlotRestricted, LineColor(kGreen), Components(QCDComponentsAlmostAll),
      Normalization(Factor));
   FinalModel.plotOn(FinalResultPlotRestricted, LineColor(kRed), Components(WComponentsAlmostAll),
      Normalization(Factor));
   FinalModel.plotOn(FinalResultPlotRestricted, LineColor(kYellow - 3), Components(TTbarComponentsAlmostAll),
      Normalization(Factor));
   FinalResultPlotRestricted->SetMinimum(0.5);
   PsFile.AddPlot(FinalResultPlotRestricted, "", false);
   PsFile.AddPlot(FinalResultPlotRestricted, "", true);

   // TLegend Legend1(0.28, 0.85, 0.85, 0.55);
   TLegend Legend1(0.50, 0.85, 0.85, 0.55);
   Legend1.SetFillStyle(0);
   Legend1.SetBorderSize(0);
   Legend1.SetTextFont(42);

   TH1D Dummy1("Dummy1", "dummy!", 2, 0, 1);
   Dummy1.SetMarkerSize(1.8);
   Dummy1.SetLineWidth(1.8);
   Legend1.AddEntry(&Dummy1, "Data", "lp");

   TH1D Dummy2("Dummy2", "dummy!", 2, 0, 1);
   Dummy2.SetLineColor(kBlue);
   Dummy2.SetLineWidth(2);
   // Legend1.AddEntry(&Dummy2, "Total SM prediction", "l");
   
   TH1D Dummy3("Dummy3", "dummy!", 2, 0, 1);
   Dummy3.SetLineColor(kGreen);
   Dummy3.SetFillColor(kGreen);
   Dummy3.SetLineWidth(2);
   Legend1.AddEntry(&Dummy3, "Multijets", "f");

   TH1D Dummy4("Dummy4", "dummy!", 2, 0, 1);
   Dummy4.SetLineColor(kRed);
   Dummy4.SetFillColor(kRed);
   Dummy4.SetLineWidth(2);
   Legend1.AddEntry(&Dummy4, "W/Z + jets", "f");

   TH1D Dummy5("Dummy5", "dummy!", 2, 0, 1);
   Dummy5.SetLineColor(kYellow - 3);
   Dummy5.SetFillColor(kYellow - 3);
   Dummy5.SetLineWidth(2);
   Legend1.AddEntry(&Dummy5, "t#bar{t} + jets", "f");

   TH1D Dummy6("Dummy6", "dummy!", 2, 0, 1);
   Dummy6.SetLineColor(kMagenta);
   Dummy6.SetLineWidth(2);
   Legend1.AddEntry(&Dummy6, "Total SM Predction +", "l");
   Legend1.AddEntry(&Dummy6, "T2bb (M_{#tilde{b}} = 300 GeV, M_{#tilde{#chi}^{0}} = 100 GeV)", "");

   TFile F2("T2bbSignal.root");
   TH1D *HSignalOnly = (TH1D *)F2.Get("htemp");
   TH1D *HSignal = (TH1D *)HSignalOnly->Clone("HSignal");
   FinalModel.fillHistogram(HSignal, RooArgList(MR), 326.976);
   HSignal->Add(HSignalOnly);
   HSignal->SetLineWidth(2);
   HSignal->SetLineColor(kMagenta);

   TCanvas FinalResutlPlotCanvasSquare("C1", "C1", 1024, 1024);
   HSignal->SetStats(0);
   HSignal->SetTitle("");
   HSignal->GetXaxis()->SetTitle("M_{R} (GeV)");
   HSignal->GetYaxis()->SetTitle("Events / ( 40 GeV )");
   HSignal->SetMinimum(0.01);
   HSignal->Draw("hist");
   // HTotal.Draw("same hist");
   HQ.Add(&HW);
   HQ.Add(&HT);
   HW.Add(&HT);
   HQ.Draw("same hist bar");
   HW.Draw("same hist bar");
   HT.Draw("same hist bar");
   FinalResultPlot->Draw("same");
   Legend1.Draw();
   AddCMSPreliminary(0.1, 0.92, 1750);
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1.png");
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1.C");
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1.eps");
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1.pdf");
   FinalResutlPlotCanvasSquare.SetLogy();
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1_Log.png");
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1_Log.C");
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1_Log.eps");
   FinalResutlPlotCanvasSquare.SaveAs("FinalResultPlot1_Log.pdf");

   F2.Close();

   TCanvas FinalResutlPlotRestrictedCanvasSquare("C1", "C1", 1024, 1024);
   FinalResultPlotRestricted->SetTitle("");
   FinalResultPlotRestricted->SetMinimum(0.1);
   FinalResultPlotRestricted->Draw();
   Legend1.Draw();
   AddCMSPreliminary(0.1, 0.92, 1750);
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2.png");
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2.C");
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2.eps");
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2.pdf");
   // FinalResutlPlotRestrictedCanvasSquare.SetLogy();
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2_Log.png");
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2_Log.C");
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2_Log.eps");
   // FinalResutlPlotRestrictedCanvasSquare.SaveAs("FinalResultPlot1_NoHighR2_Log.pdf");

   // Each bin, model vs. data
   RooPlot *FinalResultPlot2 = MR.frame(Bins(25), Name("FitResults2"), Title("MR, different R cuts"),
      Range(400, 1500));
   Dataset.plotOn(FinalResultPlot2, MarkerSize(1.8), MarkerColor(1));
   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      RooArgList ComponentsToAdd;
      for(int j = i; j < (int)RCuts.size(); j++)
      {
         ComponentsToAdd.add(*TopLevelModels[j]);
         ComponentsToAdd.add(*WTopLevelModels[j]);
         ComponentsToAdd.add(*TTopLevelModels[j]);
      }
      FinalModel.plotOn(FinalResultPlot2, LineColor(kBlue), Components(ComponentsToAdd), LineColor(i + 1),
         Normalization(326.976 / 295));
   }
   for(int i = 1; i < (int)RCuts.size(); i++)
      Dataset.reduce(Cut(Form("%s > %f", RString.c_str(), RCuts[i])))->plotOn(FinalResultPlot2,
         MarkerSize(1.8), MarkerColor(i + 1));

   TCanvas FinalResultCanvas2;

   FinalResultPlot2->SetMinimum(0.1);
   FinalResultPlot2->Draw();

   vector<TH1D *> DummyHistograms;
   TLegend legend(0.6, 0.85, 0.9, 0.55);
   legend.SetFillStyle(0);
   legend.SetBorderSize(0);
   legend.SetTextFont(42);
   for(int i = 0; i < (int)RCuts.size(); i++)
   {
      DummyHistograms.push_back(new TH1D(Form("DummyHistogram_%d", i), "Dummy!", 2, 0, 1));
      DummyHistograms[i]->SetLineColor(i + 1);
      DummyHistograms[i]->SetFillColor(i + 1);
      DummyHistograms[i]->SetLineWidth(2);
      legend.AddEntry(DummyHistograms[i], Form("R^{2} > %.2f", RCuts[i] * RCuts[i]), "l");
   }
   legend.Draw();

   PsFile.AddCanvas(FinalResultCanvas2);
   FinalResultCanvas2.SetLogy();
   PsFile.AddCanvas(FinalResultCanvas2);

   TCanvas FinalResultCanvas2Square("C2", "C2", 1024, 1024);
   FinalResultPlot2->SetTitle("");
   FinalResultPlot2->SetMinimum(0.1);
   FinalResultPlot2->Draw();
   legend.Draw();
   AddCMSPreliminary(0.1, 0.92, 1750);
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2.png");
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2.C");
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2.eps");
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2.pdf");
   FinalResultCanvas2Square.SetLogy();
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2Log.png");
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2Log.C");
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2Log.eps");
   FinalResultCanvas2Square.SaveAs("FinalResultPlot2Log.pdf");

   RooPlot *FinalResultPlot2Restricted = MR.frame(Bins(25), Name("FitResults2Restricted"),
      Title("MR, different R cuts, R2 < 0.5"), Range(400, 1400));
   Dataset.reduce("R*R < 0.5")->plotOn(FinalResultPlot2Restricted, MarkerSize(1.8), MarkerColor(1));
   for(int i = 0; i < (int)RCuts.size() - 1; i++)
   {
      RooArgList ComponentsToAdd;
      for(int j = i; j < (int)RCuts.size() - 1; j++)
      {
         ComponentsToAdd.add(*TopLevelModels[j]);
         ComponentsToAdd.add(*WTopLevelModels[j]);
         ComponentsToAdd.add(*TTopLevelModels[j]);
      }
      FinalModel.plotOn(FinalResultPlot2Restricted, Components(ComponentsToAdd), LineColor(i + 1), Normalization(Factor));
   }
   for(int i = 1; i < (int)RCuts.size(); i++)
      Dataset.reduce(Cut(Form("%s > %f && R*R < 0.5", RString.c_str(), RCuts[i])))->plotOn(FinalResultPlot2Restricted,
         MarkerSize(1.8 - i * 0.05), MarkerColor(i + 1));

   TCanvas FinalResultCanvas2Restricted;

   FinalResultPlot2Restricted->SetMinimum(0.1);
   FinalResultPlot2Restricted->Draw();
   legend.Draw();
   PsFile.AddCanvas(FinalResultCanvas2Restricted);
   FinalResultCanvas2Restricted.SetLogy();
   PsFile.AddCanvas(FinalResultCanvas2Restricted);

   TCanvas FinalResultCanvas2RestrictedSquare("C2", "C2", 1024, 1024);
   FinalResultPlot2Restricted->SetTitle("");
   FinalResultPlot2Restricted->SetMinimum(0.1);
   FinalResultPlot2Restricted->Draw();
   legend.Draw();
   AddCMSPreliminary(0.1, 0.92, 1750);
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2.png");
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2.C");
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2.eps");
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2.pdf");
   // FinalResultCanvas2RestrictedSquare.SetLogy();
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2_Log.png");
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2_Log.C");
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2_Log.eps");
   // FinalResultCanvas2RestrictedSquare.SaveAs("FinalResultPlot2_NoHighR2_Log.pdf");

   double HistogramBins[7] = {sqrt(0.25), sqrt(0.30), sqrt(0.35), sqrt(0.38), sqrt(0.42), sqrt(0.50), 1.5};
   TH1D HDataRView("HDataRView", "Data R view!", 6, HistogramBins);
   Dataset.fillHistogram(&HDataRView, RooArgList(R));
   
   double HistogramBins2[7] = {0.25, 0.30, 0.35, 0.38, 0.42, 0.50, 0.60};
   TH1D HDataR2View("HDataR2View", "Data R2 view!", 6, HistogramBins2);
   for(int i = 1; i <= 6; i++)
      HDataR2View.SetBinContent(i, HDataRView.GetBinContent(i));
   HDataR2View.SetMarkerSize(1.8);

   TH1D HModelR2View("HModelR2View", "Model R2 view!!!", 6, HistogramBins2);
   HModelR2View.SetBinContent(1, 326.976 - 195.492);
   HModelR2View.SetBinContent(2, 195.492 - 121.880);
   HModelR2View.SetBinContent(3, 121.880 - 92.550);
   HModelR2View.SetBinContent(4, 92.550 - 63.098);
   HModelR2View.SetBinContent(5, 63.098 - 30.530);
   HModelR2View.SetBinContent(6, 30.530);
   HModelR2View.SetBinError(1, sqrt(30.98 * 30.98 - 25.58 * 25.58));
   HModelR2View.SetBinError(2, sqrt(25.58 * 25.58 - 21.51 * 21.51));
   HModelR2View.SetBinError(3, sqrt(21.51 * 21.51 - 18.29 * 18.29));
   HModelR2View.SetBinError(4, sqrt(18.29 * 18.29 - 12.95 * 12.95));
   HModelR2View.SetBinError(5, sqrt(12.95 * 12.95 - 8.97 * 8.97));
   HModelR2View.SetBinError(6, 8.97);
   HModelR2View.SetLineWidth(2);
   HModelR2View.SetLineColor(kBlue);
   // HModelR2View.SetFillColor(kBlue);
   // HModelR2View.SetFillStyle(3002);
   PsFile.AddPlot(HModelR2View);
   
   TH1D HModelR2ViewUp("HModelR2ViewUp", "Model R2 view!!!", 6, HistogramBins2);
   TH1D HModelR2ViewDown("HModelR2ViewDown", "Model R2 view!!!", 6, HistogramBins2);
   for(int i = 1; i <= 6; i++)
   {
      HModelR2ViewUp.SetBinContent(i, HModelR2View.GetBinContent(i) + HModelR2View.GetBinError(i));
      HModelR2ViewDown.SetBinContent(i, HModelR2View.GetBinContent(i) - HModelR2View.GetBinError(i));
   }
   HModelR2ViewUp.SetFillColor(kBlue);
   HModelR2ViewUp.SetFillStyle(3944);
   HModelR2ViewDown.SetFillColor(10);
   HModelR2ViewDown.SetFillStyle(1001);

   TH1D HLQR2View("HLQR2View", "LQ R2 view!!!!!", 6, HistogramBins2);
   HLQR2View.SetBinContent(1, 38.048);
   HLQR2View.SetBinContent(2, 27.902);
   HLQR2View.SetBinContent(3, 16.603);
   HLQR2View.SetBinContent(4, 15.680);
   HLQR2View.SetBinContent(5, 19.831);
   HLQR2View.SetBinContent(6, 24.212);
   HLQR2View.SetLineWidth(2);
   HLQR2View.SetLineColor(kMagenta);
   PsFile.AddPlot(HLQR2View);

   HLQR2View.Add(&HModelR2View);

   TLegend RViewLegend(0.24, 0.85, 0.88, 0.65);
   RViewLegend.SetFillStyle(0);
   RViewLegend.SetBorderSize(0);
   RViewLegend.SetTextFont(42);
   RViewLegend.AddEntry(&HDataR2View, "Data", "lp");
   RViewLegend.AddEntry(&HModelR2View, "Total SM Prediction", "l");
   RViewLegend.AddEntry(&HLQR2View, "Total SM Prediction +", "l");
   RViewLegend.AddEntry(&HLQR2View, "T2bb (M_{#tilde{b}} = 300 GeV, M_{#tilde{#chi}^{0}} = 100 GeV)", "");

   TCanvas FinalResult3Canvas;
   HDataR2View.SetMinimum(0);
   HDataR2View.Draw("error");
   HModelR2View.Draw("same hist");
   RViewLegend.Draw();
   PsFile.AddCanvas(FinalResult3Canvas);
   FinalResult3Canvas.SetLogy();
   PsFile.AddCanvas(FinalResult3Canvas);

   TCanvas FinalResult3CanvasSquare("C3", "", 1024, 1024);
   HModelR2ViewUp.SetTitle("");
   HModelR2ViewUp.SetStats(0);
   HModelR2ViewUp.GetXaxis()->SetTitle("R^{2}");
   HModelR2ViewUp.GetYaxis()->SetTitle("Events");
   HModelR2ViewUp.SetMaximum(180);
   HModelR2ViewUp.SetMinimum(0);
   HModelR2ViewUp.Draw();
   HModelR2ViewDown.Draw("same");
   HModelR2ViewUp.Draw("same axis");
   HDataR2View.SetStats(0);
   HDataR2View.Draw("same error");
   HModelR2View.Draw("same hist");
   HLQR2View.Draw("same hist");
   RViewLegend.Draw();
   AddCMSPreliminary(0.1, 0.92, 1750);
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3.png");
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3.C");
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3.eps");
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3.pdf");
   HModelR2ViewUp.SetMinimum(10);
   FinalResult3CanvasSquare.SetLogy();
   HDataR2View.Draw("same error");
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3_Log.png");
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3_Log.C");
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3_Log.eps");
   FinalResult3CanvasSquare.SaveAs("FinalResultPlot3_Log.pdf");

   for(int i = 0; i < (int)RCuts.size(); i++)
      delete DummyHistograms[i];
   DummyHistograms.clear();

   // Cleaning....
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

      if(Component1Models[i] != NULL) delete Component1Models[i];
      if(Component2Models[i] != NULL) delete Component2Models[i];
      if(Models[i] != NULL)   delete Models[i];
      if(Yields[i] != NULL)   delete Yields[i];
      if(S1[i] != NULL)       delete S1[i];
      if(S2[i] != NULL)       delete S2[i];
      if(Fraction[i] != NULL) delete Fraction[i];
   }

   TopLevelModels.clear();
   ModelBeforeConstraint.clear();
   Constraint.clear();
   TopLevelYields.clear();

   NormalizedYields.clear();
   NormalizedNegativeYields.clear();

   Component1Models.clear();
   Component2Models.clear();
   Models.clear();
   Yields.clear();
   S1.clear();
   S2.clear();
   Fraction.clear();

   F.Close();
}

