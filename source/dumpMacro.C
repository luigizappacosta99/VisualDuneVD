#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "workflowParams.C"
#include "particleLibrary.C"
#include "utils.C"

std::ostringstream dumpMacro(int myEventID, TDirectory* dir, const std::string& outfile = "", TGraph* gr = nullptr) 
{
    workflowParams* params = workflowParams::Instance();

    std::ostringstream oss;

    /*if (!outfile.empty()) {
        fileStream.open(outfile);
        if (!fileStream.is_open()) {
            std::cerr << "Error: could not open file " << outfile << std::endl;
            return;
        }
        out = &fileStream;
    } else {
        out = &std::cout;
    }*/

    if(params->getVerboseTruth()){

        TTree* treeTruth = (TTree*)dir->Get("simData");
        TTree* treeEdep = (TTree*)dir->Get("EDepData");

        //MCTruth data

        UInt_t eventID;
        Float_t nuEnergy;
        Float_t px;
        Float_t py;
        Float_t pz;
        Int_t nuPDG;
        Int_t leptonPDG;
        Int_t CCNC;
        Int_t nParticles;

        treeTruth->SetBranchAddress("nuEnergy", &nuEnergy);
        treeTruth->SetBranchAddress("nuPDG", &nuPDG);
        treeTruth->SetBranchAddress("px", &px);
        treeTruth->SetBranchAddress("py", &py);
        treeTruth->SetBranchAddress("pz", &pz);
        treeTruth->SetBranchAddress("CCNC", &CCNC);
        treeTruth->SetBranchAddress("leptonPDG", &leptonPDG);
        treeTruth->SetBranchAddress("nParticles", &nParticles);
        treeTruth->SetBranchAddress("eventID",&eventID);

        std::vector<float>* particleE = nullptr;
        std::vector<int>* particlePDG = nullptr;
        std::vector<int>* particleTrackID = nullptr;

        treeTruth->SetBranchAddress("particleE", &particleE);
        treeTruth->SetBranchAddress("particlePDG", &particlePDG);
        treeTruth->SetBranchAddress("particleTrackID", &particleTrackID);

        treeTruth->GetEntry(myEventID);

        //edep data

        std::vector<float>* edepE = nullptr;

        treeEdep->SetBranchAddress("EdepE", &edepE);

        treeEdep->GetEntry(myEventID);

        //simulation data dump

        oss << "EVENT #" << eventID << " \n";
        oss << std::string(48, '-') << "\n";
        oss << "Neutrino data:\n";
        oss << " > pdg " << nuPDG << "\n";
        oss << " > energy "<<nuEnergy<<"\n";
        oss << " > "<<(CCNC == 0? "CC event":"NC event")<<"\n";
        oss << " > momentum: ("<<px<<","<<py<<","<<pz<<")\n";
        oss << " > lepton PDG "<<leptonPDG<<"\n";

        oss << std::string(48, '-') << "\n\n";
        oss << "Energy deposition:\n";
        double edepTot = sum(edepE);
        oss << " > total raw count " << edepTot << "\n";

        if(CCNC == 0){gr->SetPoint(myEventID,edepTot,nuEnergy);}

        oss << std::string(48, '-') << "\n\n";
        oss <<"MCTruth particles table\n";

        oss << " > total particles produced "<<nParticles<<"\n";
        oss << std::string(48, '-') << "\n";
        oss << std::left << std::setw(6) << "#" 
                  << std::setw(16) << "PDG" 
                  << std::setw(16) << "Energy" 
                  << std::setw(8) << "TrackID" << "\n";
        oss << std::string(48, '-') << "\n";
        for(int i=0; i<nParticles; ++i){
            oss << std::left << std::setw(6) << i
                      << std::setw(16) << particleLibrary(particlePDG->at(i))
                      << std::setw(16) << particleE->at(i)
                      << std::setw(8) << particleTrackID->at(i)
                      << "\n";
        }
        oss << std::string(48, '-') << "\n\n";

        //delete vectors

        particleE->clear();
        particlePDG->clear();
        particleTrackID->clear();
    }

    if(params->getVerboseG4()){

        TTree* treeG4 = (TTree*)dir->Get("g4Data");

        //G4 data

        std::vector<float>* particleEg4 = nullptr;
        std::vector<int>* particlePDGg4 = nullptr;
        std::vector<int>* particleMotherg4 = nullptr;
        std::vector<std::string>* particleProcessg4 = nullptr;
        std::vector<int>* particleTrackIDg4 = nullptr;

        treeG4->SetBranchAddress("g4PDGs", &particlePDGg4);
        treeG4->SetBranchAddress("g4Eparticles", &particleEg4);
        treeG4->SetBranchAddress("g4Mother", &particleMotherg4);
        treeG4->SetBranchAddress("g4Process", &particleProcessg4);
        treeG4->SetBranchAddress("g4TrackID", &particleTrackIDg4);

        treeG4->GetEntry(myEventID);

        //g4 data dump

        oss<<"Geant4 particles table\n";

        oss<<" > total particles produced "<<particlePDGg4->size()<<"\n";
        oss << std::string(80, '-') << "\n";
        oss << std::left << std::setw(6) << "#" 
                  << std::setw(16) << "PDG" 
                  << std::setw(16) << "Energy" 
                  << std::setw(16) << "Mother ID" 
                  << std::setw(16) << "Process" 
                  << std::setw(8) << "TrackID" << "\n";
        oss << std::string(80, '-') << "\n";
        for(int i=0; i<particlePDGg4->size(); ++i){
        oss << std::left << std::setw(6) << i
                  << std::setw(16) << particleLibrary(particlePDGg4->at(i))
                  << std::setw(16) << particleEg4->at(i)
                  << std::setw(8) << particleMotherg4->at(i)
                  << std::setw(24) << particleProcessg4->at(i)
                  << std::setw(8) << particleTrackIDg4->at(i)
                  << "\n";
        }
        oss << std::string(80, '-') << "\n";

        particlePDGg4->clear();
        particleEg4->clear();
        particleMotherg4->clear();
        particleProcessg4->clear();
        particleTrackIDg4->clear();  

    }

    return oss;
  
}