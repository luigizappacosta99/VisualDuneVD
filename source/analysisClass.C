#include "../include/analysisClass.h"
#include "../include/workflowParams.h"
#include "utils.C"
#include "particleLibrary.C"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

analysisClass::analysisClass(){

    params = workflowParams::Instance(); //for workflow parameters

    graphFile = nullptr;
    extraFile = nullptr;
    oss = nullptr;

    //set all vectors
    particleE = nullptr;
    particlePDG = nullptr;
    particleTrackID = nullptr;
    edepE = nullptr;

    wireID = nullptr;
    TPCID = nullptr;
    planeID = nullptr;
    peak_time = nullptr;
    integral = nullptr;

    wireIDT = nullptr;
    planeIDT = nullptr;
    trackIDT = nullptr;
    TPCIDT = nullptr;
    peak_timeT = nullptr;
    integralT = nullptr;
    trackPIDS = nullptr;

    //histograms
    hTotal = new TH1F("hTotal", "Total events",         nBins, xMin, xMax);
    hReco2 = new TH1F("hReco2", "Reconstructed events", nBins, xMin, xMax);

    outfilename = "";

    //oss = nullptr;

}

analysisClass::~analysisClass(){

    graphFile->Close();
    extraFile->Close();

}

void analysisClass::setOutFiles(){

    outfilename = params->getOutName();

    //std::cout << "analysisClass " << outfilename << std::endl;

    if (graphFile && !graphFile->IsZombie()) {
        std::cout << "[INFO] Closing currently open graphFile: "
                  << graphFile->GetName() << std::endl;
        graphFile->Close();
        delete graphFile;
        graphFile = nullptr;
    }

    if (extraFile && !extraFile->IsZombie()) {
        std::cout << "[INFO] Closing currently open extraFile: "
                  << extraFile->GetName() << std::endl;
        extraFile->Close();
        delete extraFile;
        extraFile = nullptr;
    }

    //prepare output files
    graphFile = new TFile((outfilename+"_graphs.root").data(),"RECREATE");
    extraFile = new TFile((outfilename+"_extra.root").data(),"RECREATE");

    //set dump outstream
    if (!outfilename.empty()) {
        fileStream.open(outfilename+"_dump.txt");
        if (!fileStream.is_open()) {
            std::cerr << "[WARNING] Error: could not open file " << outfilename << std::endl;
            oss = &std::cout;
        }
        else {oss = &fileStream;}
    } else {
        oss = &std::cout;
    }

}

void analysisClass::openInputFile(std::string infilename){

    //get data
    TFile* file = TFile::Open(infilename.data());
    if (!file || file->IsZombie()) {
        std::cerr << "[INFO] Error: cannot open file " << infilename << std::endl;
        return;
    }
    const char* foldername = "ana";
    TDirectory* dir = (TDirectory*)file->Get(foldername);
    if (!dir) {
        std::cerr << "[INFO] Error: folder '" << foldername << "' not found in the file." << std::endl;
        file->Close();
        return;
    }

    //set all trees
    eventTree    = (TTree*)dir->Get("eventData");
    edepTree     = (TTree*)dir->Get("EDepData");
    hitTree      = (TTree*)dir->Get("hitData");
    pointsTree   = (TTree*)dir->Get("pointsData");
    trackTree    = (TTree*)dir->Get("trackData");
    particleTree = (TTree*)dir->Get("particleData");
    simTree      = (TTree*)dir->Get("simData");
    g4Tree       = (TTree*)dir->Get("g4Data");

}

void analysisClass::graphAction(int idx){

    //std::cout << "graphAction" <<std::endl;

    TCanvas* c = new TCanvas();

    graphFile->cd();

    clearHits = true;
    clearTracks = true;

    int x = 0;

    //hits data
    hitTree->SetBranchAddress("peak_time", &peak_time);
    hitTree->SetBranchAddress("eventID", &eventID);
    hitTree->SetBranchAddress("wireID", &wireID);
    hitTree->SetBranchAddress("planeID", &planeID);
    hitTree->SetBranchAddress("TPCID", &TPCID);
    hitTree->SetBranchAddress("integral", &integral);

    hitTree->GetEntry(idx);

    //track data
    trackTree->SetBranchAddress("wireIDT", &wireIDT);
    trackTree->SetBranchAddress("peak_timeT", &peak_timeT);
    trackTree->SetBranchAddress("trackID", &trackIDT);
    trackTree->SetBranchAddress("planeIDT", &planeIDT);
    trackTree->SetBranchAddress("TPCID", &TPCIDT);
    trackTree->SetBranchAddress("integralT", &integralT);
    trackTree->SetBranchAddress("trackPIDS", &trackPIDS);

    trackTree->GetEntry(idx);
    
    //check if vectors are empty
    if(wireID->size()==0){
        std::cout<<"[INFO] Hit array empty in event "<<eventID<<", skipping \n";
        clearHits = false;
        clearTracks = false;
        return;
    }
    if(peak_time->size()==0){
        std::cout<<"[WARNING] Hit array empty in event "<<eventID<<", skipping \n";
        clearHits = false;
        clearTracks = false;
        return;
    }
    if(planeID->size()==0){
        std::cout<<"[WARNING] Hit array empty in event "<<eventID<<", skipping \n";
        clearHits = false;
        clearTracks = false;
        return;
    }
    if(TPCID->size()==0){
        std::cout<<"[WARNING] Hit array empty in event "<<eventID<<", skipping \n";
        clearHits = false;
        clearTracks = false;
        return;
    }
    if(integral->size()==0){
        std::cout<<"[WARNING] Hit array empty in event "<<eventID<<", skipping \n";
        clearHits = false;
        clearTracks = false;
        return;
    }

    //initialize graph
    const double tmin = (*std::min_element(peak_time->begin(), peak_time->end()))*0.9;
    const double tmax = (*std::max_element(peak_time->begin(), peak_time->end()))*1.1;
    const int wmin = 0;
    int nTPCs = ((*std::max_element(TPCID->begin(), TPCID->end())))/8;
    const int wmax = 2*(nTPCs+1)*(292-102+292-51);

    //gStyle->SetOptTitle(0);
    c->Divide(4,1);
    for (int i=1;i<=3;++i){
        c->cd(i);
        c->DrawFrame(wmin, tmin, wmax, tmax);
    }
    //TLegend* legend = new TLegend(0.1,0.9,0.9,0.95);
    TLegend* legend = new TLegend(0,0,1,1);
    
    TMarker *m = new TMarker();
    m->SetMarkerStyle(20);

    std::vector<int> hitCounts = {0,0,0};

    //main graph
    int TPCposition;
    double intMax = *std::max_element(integral->begin(), integral->end());
    int TPCmin = *std::min_element(TPCID->begin(), TPCID->end());

    int wireSize = wireID->size();
    for(int j = 0; j < wireSize; ++j){
        //check for overflows
        if(!(j < planeID->size())){
            std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" hit arrays (planeID), skipping \n";
            clearHits = false;
            clearTracks = false;
            break;
        }
        if(!(j < integral->size())){
            std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" hit arrays (integral), skipping \n";
            clearHits = false;
            clearTracks = false;
            break;
        }
        if(!(j < TPCID->size())){
            std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" hit arrays (TPCID), skipping \n";
            clearHits = false;
            clearTracks = false;
            break;
        }
        if(!(j < peak_time->size())){
            std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" hit arrays (peak_time), skipping \n";
            clearHits = false;
            clearTracks = false;
            break;
        }

        //set up marker
        c->cd(planeID->at(j)+1);
        m->SetMarkerSize( 2. * integral->at(j) / intMax );
        m->SetMarkerColor(kBlack);

        // true wire
        switch (planeID->at(j)){
            case 0: x = wireID->at(j) + ((295-102) * (0 + (TPCID->at(j)%8))) + ((150-51) * std::floor((TPCID->at(j) - 0) * 1./ 8)); break;
            case 1: x = wireID->at(j) + ((295-102) * (7 - (TPCID->at(j)%8))) + ((150-51) * std::floor((TPCID->at(j) - 0) * 1./ 8)); break;
            case 2: x = wireID->at(j) + (292*std::floor((TPCID->at(j) - 0) * 1./ 8)); break;
        }

        hitCounts[planeID->at(j)]++;

        m->DrawMarker(x,peak_time->at(j));
    }

    std::vector<TLatex*> titles = {};

    for (int i=1;i<=3;++i){
        c->cd(i);
        //c->SetTitle(std::to_string(hitCounts[i-1]).c_str());
        titles.push_back(new TLatex(0.5, 0.95, std::to_string(hitCounts[i-1]).c_str()));
        titles[i-1]->SetNDC();
        titles[i-1]->SetTextAlign(22);
        titles[i-1]->SetTextSize(0.06);
        titles[i-1]->Draw();
    }

    //track visualisation
    if(!clearHits){}
    else if(trackIDT->size()==0){
        std::cout<<"[WARNING] No tracks reconstructed in event "<<eventID<<", skipping track plotting \n";
        clearTracks = false;
    }
    else{
        int nTracks = trackIDT->at(trackIDT->size()-1);
        //legend->SetNColumns(nTracks);
        std::string label;
        bool recordInLegend = true;
        TMarker* mLeg = nullptr;
        for(int j = 0; j < nTracks; ++j){

            //check for overflows
            if(!(j < nTracks)){
                std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" track arrays (nTracks), skipping \n";
                clearTracks = false;
                break;
            }

            m->SetMarkerStyle(24);
            m->SetMarkerColor(2+j);

            recordInLegend = true;

            for(int k = 0; k < wireIDT->size(); ++k){
                //check for overflows
                if(!(k < integralT->size())){
                    std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" track arrays (integralT), skipping \n";
                    clearTracks = false;
                    break;
                }
                if(!(k < TPCIDT->size())){
                    std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" track arrays (TPCIDT), skipping \n";
                    clearTracks = false;
                    break;
                }
                if(!(k < peak_timeT->size())){
                    std::cout<<"[WARNING] Encountered overflow in event "<<eventID<<" track arrays (peak_timeT), skipping \n";
                    clearTracks = false;
                    break;
                }

                //take track j
                if(trackIDT->at(k)!=j){continue;}

                m->SetMarkerSize( 2. * integralT->at(k) / intMax );
                TPCposition = std::floor((TPCIDT->at(k) - 0) * 1. / 8);
                c->cd(planeIDT->at(k)+1);

                //true wire
                switch (planeIDT->at(k)){
                    case 0: x = wireIDT->at(k) + ((295-102) * (0 + (TPCIDT->at(k)%8))) + ((150-51) * std::floor((TPCIDT->at(k) - 0) * 1./ 8)); break;
                    case 1: x = wireIDT->at(k) + ((295-102) * (7 - (TPCIDT->at(k)%8))) + ((150-51) * std::floor((TPCIDT->at(k) - 0) * 1./ 8)); break;
                    case 2: x = wireIDT->at(k) + (292*std::floor((TPCIDT->at(k) - 0) * 1./ 8)); break;
                }

                m->DrawMarker(x,peak_timeT->at(k));

                //legend for tracks
                if(recordInLegend){
                    mLeg = (TMarker*) m->Clone();
                    mLeg->SetMarkerStyle(21);
                    mLeg->SetMarkerSize(2);
                    //label = std::to_string(trackPIDS->at(j));
                    label = std::to_string(k);
                    legend->AddEntry(mLeg,label.c_str(),"P");
                    recordInLegend = false;
                }
            }
        }
    }

    if(clearHits){
        wireID->clear();
        TPCID->clear();
        planeID->clear();
        peak_time->clear();
        integral->clear();
    }
    
    if(clearTracks){
        wireIDT->clear();
        TPCIDT->clear();
        trackIDT->clear();
        planeIDT->clear();
        peak_timeT->clear();
        integralT->clear();
    }

    if(clearTracks){
        c->cd(4);
        legend->Draw();
    }

    c->Draw();
    c->Write();

    gPad->Modified();
    gPad->Update();
    gSystem->ProcessEvents();

}

void analysisClass::dumpAction(int idx){

    if(params->getVerboseTruth()){

        simTree->SetBranchAddress("nuEnergy", &nuEnergy);
        simTree->SetBranchAddress("nuPDG", &nuPDG);
        simTree->SetBranchAddress("px", &px);
        simTree->SetBranchAddress("py", &py);
        simTree->SetBranchAddress("pz", &pz);
        simTree->SetBranchAddress("CCNC", &CCNC);
        simTree->SetBranchAddress("leptonPDG", &leptonPDG);
        simTree->SetBranchAddress("nParticles", &nParticles);
        simTree->SetBranchAddress("eventID",&eventID);

        simTree->SetBranchAddress("particleE", &particleE);
        simTree->SetBranchAddress("particlePDG", &particlePDG);
        simTree->SetBranchAddress("particleTrackID", &particleTrackID);

        simTree->GetEntry(idx);

        edepTree->SetBranchAddress("EdepE", &edepE);

        edepTree->GetEntry(idx);

        //simulation data dump

        *oss << "EVENT #" << eventID << " \n";
        *oss << std::string(48, '-') << "\n";
        *oss << "Neutrino data:\n";
        *oss << " > pdg " << particleLibrary(nuPDG) << "\n";
        *oss << " > energy "<<nuEnergy<<" GeV \n";
        *oss << " > "<<(CCNC == 0? "CC event":"NC event")<<"\n";
        *oss << " > momentum: ("<<px<<","<<py<<","<<pz<<")\n";
        *oss << " > lepton "<<particleLibrary(leptonPDG)<<"\n";

        *oss << std::string(48, '-') << "\n\n";
        *oss << "Energy deposition:\n";
        double edepTot = sum(edepE);
        *oss << " > total raw count " << edepTot/1000 << " GeV \n";

        *oss << std::string(48, '-') << "\n\n";
        *oss <<"MCTruth particles table\n";

        *oss << " > total particles produced "<<nParticles<<"\n";
        *oss << std::string(48, '-') << "\n";
        *oss << std::left << std::setw(6) << "#" 
                  << std::setw(16) << "PDG" 
                  << std::setw(16) << "Energy" 
                  << std::setw(8) << "TrackID" << "\n";
        *oss << std::string(48, '-') << "\n";
        for(int i=0; i<nParticles; ++i){
            *oss << std::left << std::setw(6) << i
                      << std::setw(16) << particleLibrary(particlePDG->at(i))
                      << std::setw(16) << particleE->at(i)
                      << std::setw(8) << particleTrackID->at(i)
                      << "\n";
        }
        *oss << std::string(48, '-') << "\n\n";

        //delete vectors

        particleE->clear();
        particlePDG->clear();
        particleTrackID->clear();
    }

}

void analysisClass::customAction(int idx){

    pointsTree->SetBranchAddress("nPoints", &nPoints);
    pointsTree->GetEntry(idx);

    edepTree->SetBranchAddress("EdepE", &edepE);
    edepTree->GetEntry(idx);

    simTree->SetBranchAddress("nuEnergy",&nuEnergy);


    double edepTot = sum(edepE)/1000;

    float fraction = (nuEnergy - edepTot) / nuEnergy;

    if(fraction<=0 || fraction >=1){std::cout << nPoints << " " 
                << nuEnergy << " " 
                << edepTot << " " 
                << fraction << " " 
                << std::endl;}


    hTotal->Fill(fraction);
    if(nPoints>0){hReco2->Fill(fraction);}

    edepE->clear();

}

void analysisClass::endAnalysis(){

}

void analysisClass::endCustomAnalysis(){

    //customEnd
    extraFile->cd();
    /*TH1F* hEff = (TH1F*) hReco2->Clone("hEff");
    hEff->Divide(hReco2, hTotal, 1.0, 1.0, "B");  // "B" = binomial errors

    // --- Style ---*/
    hTotal->SetTitle("hTotal; Edep; Count");
    hTotal->SetLineColor(kBlue);
    hTotal->SetLineWidth(2);

    hReco2->SetTitle("hReco2; Edep; Count");
    hReco2->SetLineColor(kBlue);
    hReco2->SetLineWidth(2);

    // --- Draw ---
    //TCanvas* c1 = new TCanvas("c1", "Efficiency", 800, 600);
    //hEff->SaveAs("provaHist.root","E");        // "E" draws error bars
    hTotal->Write();
    hReco2->Write();
    extraFile->Close();

}
