#include <iostream>
#include <vector>
using namespace std;

#include "Pythia.h"
using namespace Pythia8;

#include "TFile.h"
#include "TTree.h"

#include "TauHelperFunctions2.h"
#include "GenparticleTree.h"

int main()
{
   Pythia pythia;

   pythia.readString("ParticleDecays:limitTau0 = on");     // Particle lifetime cutoff
   pythia.readString("ParticleDecays:tauMax = 10");

   pythia.readString("WeakSingleBoson:ffbar2gmZ = on");    // Switch on Z
   pythia.readString("WeakSingleBoson:ffbar2W = off");     // Switch off W
   pythia.readString("23:oneChannel = 1 1 0 16 -16");      // Force Z to decay to neutrinos

   pythia.readString("Tune:pp 2");                         // Tune?  What??
   pythia.readString("Tune:ee 3");                         // I see this in CMS production settings.....

   pythia.readString("Random:setSeed = on");               // Random seed on
   pythia.readString("Random:seed = 0");                   // Set random seed based on time

   pythia.init(2212, 2212, 7000);                          // Proton-proton collision at 7 TeV

   TFile F("Output.root", "RECREATE");

   TTree OutputTree("Tree", "Tree");

   double PT, Theta, Y;
   OutputTree.Branch("PT", &PT, "PT/D");
   OutputTree.Branch("Theta", &Theta, "Theta/D");
   OutputTree.Branch("Y", &Y, "Y/D");

   int EntryCount = 100000;
   for(int iEntry = 0; iEntry < EntryCount; iEntry++)
   {
      // Generate event
      pythia.next();

      // Particles
      vector<GenParticle> Particles(pythia.event.size());

      // Loop over particles
      for(int j = 0; j < pythia.event.size(); j++)
      {
         Particles[j].P[0] = pythia.event[j].e();
         Particles[j].P[1] = pythia.event[j].px();
         Particles[j].P[2] = pythia.event[j].py();
         Particles[j].P[3] = pythia.event[j].pz();
         
         Particles[j].V[0] = pythia.event[j].tProd();
         Particles[j].V[1] = pythia.event[j].xProd();
         Particles[j].V[2] = pythia.event[j].yProd();
         Particles[j].V[3] = pythia.event[j].zProd();

         Particles[j].PDGID = pythia.event[j].id();
         Particles[j].StatusCode = pythia.event.statusHepMC(j);

         Particles[j].Mothers = pythia.event.motherList(j);
         Particles[j].Daughters = pythia.event.daughterList(j);
      }
      GenParticleTree Tree(Particles, 1, iEntry + 1);

      // Find the two neutrinos
      FourVector Nu1(0, 0, 0, 0), Nu2(0, 0, 0, 0);
      for(int i = 0; i < Tree.ParticleCount(); i++)
      {
         if(Tree[i].StatusCode != 1)
            continue;

         if(Tree[i].PDGID == 16 && FourVector(Tree[i].P).GetPT() > Nu1.GetPT())
            Nu1 = Tree[i].P;
         if(Tree[i].PDGID == -16 && FourVector(Tree[i].P).GetPT() > Nu2.GetPT())
            Nu2 = Tree[i].P;
      }

      // Gather information and write to tree
      FourVector NuNu = Nu1 + Nu2;
      FourVector BoostedNu1 = Nu1.Boost(NuNu, NuNu.GetBeta());
      // FourVector BoostedNu2 = Nu2.Boost(NuNu, NuNu.GetBeta());
      // cout << BoostedNu1 + BoostedNu2 << endl;

      Y = NuNu.GetY();
      PT = NuNu.GetPT();
      Theta = GetAngle(NuNu, BoostedNu1);
      OutputTree.Fill();
   }

   OutputTree.Write();
   F.Close();

   return 0;
}




