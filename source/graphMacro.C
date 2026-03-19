#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>

bool graphMacro(TCanvas* c, int myEventID, TDirectory* dir) 
{
    TTree* treeHit = (TTree*)dir->Get("hitData");
    TTree* treeTrack = (TTree*)dir->Get("trackData");

    bool returnValue = true;
    bool clearHits = true;
    bool clearTracks = true;

    int x = 0;

    //hits data
    UInt_t eventID;
    std::vector<unsigned int>* wireID = nullptr;
    std::vector<unsigned int>* TPCID = nullptr;
    std::vector<unsigned int>* planeID = nullptr;
    std::vector<float>* peak_time = nullptr;
    std::vector<double>* integral = nullptr;

    treeHit->SetBranchAddress("peak_time", &peak_time);
    treeHit->SetBranchAddress("eventID", &eventID);
    treeHit->SetBranchAddress("wireID", &wireID);
    treeHit->SetBranchAddress("planeID", &planeID);
    treeHit->SetBranchAddress("TPCID", &TPCID);
    treeHit->SetBranchAddress("integral", &integral);

    treeHit->GetEntry(myEventID);

    //track data
    UInt_t eventIDT;
    std::vector<unsigned int>* wireIDT = nullptr;
    std::vector<unsigned int>* planeIDT = nullptr;
    std::vector<unsigned int>* trackIDT = nullptr;
    std::vector<unsigned int>* TPCIDT = nullptr;
    std::vector<float>* peak_timeT = nullptr;
    std::vector<double>* integralT = nullptr;
    std::vector<int>* trackPIDS = nullptr;

    treeTrack->SetBranchAddress("wireIDT", &wireIDT);
    treeTrack->SetBranchAddress("peak_timeT", &peak_timeT);
    treeTrack->SetBranchAddress("eventID", &eventIDT);
    treeTrack->SetBranchAddress("trackID", &trackIDT);
    treeTrack->SetBranchAddress("planeIDT", &planeIDT);
    treeTrack->SetBranchAddress("TPCID", &TPCIDT);
    treeTrack->SetBranchAddress("integralT", &integralT);
    treeTrack->SetBranchAddress("trackPIDS", &trackPIDS);

    treeTrack->GetEntry(myEventID);
    
    //check if vectors are empty
    if(wireID->size()==0){
        std::cout<<"Hit array empty in event "<<eventID<<", skipping \n";
        returnValue = false;
        clearHits = false;
        clearTracks = false;
        return returnValue;
    }
    if(peak_time->size()==0){
        std::cout<<"Hit array empty in event "<<eventID<<", skipping \n";
        returnValue = false;
        clearHits = false;
        clearTracks = false;
        return returnValue;
    }
    if(planeID->size()==0){
        std::cout<<"Hit array empty in event "<<eventID<<", skipping \n";
        returnValue = false;
        clearHits = false;
        clearTracks = false;
        return returnValue;
    }
    if(TPCID->size()==0){
        std::cout<<"Hit array empty in event "<<eventID<<", skipping \n";
        returnValue = false;
        clearHits = false;
        clearTracks = false;
        return returnValue;
    }
    if(integral->size()==0){
        std::cout<<"Hit array empty in event "<<eventID<<", skipping \n";
        returnValue = false;
        clearHits = false;
        clearTracks = false;
        return returnValue;
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
            std::cout<<"Encountered overflow in event "<<eventID<<" hit arrays (planeID), skipping \n";
            returnValue = false;
            clearHits = false;
            clearTracks = false;
            break;
        }
        if(!(j < integral->size())){
            std::cout<<"Encountered overflow in event "<<eventID<<" hit arrays (integral), skipping \n";
            returnValue = false;
            clearHits = false;
            clearTracks = false;
            break;
        }
        if(!(j < TPCID->size())){
            std::cout<<"Encountered overflow in event "<<eventID<<" hit arrays (TPCID), skipping \n";
            returnValue = false;
            clearHits = false;
            clearTracks = false;
            break;
        }
        if(!(j < peak_time->size())){
            std::cout<<"Encountered overflow in event "<<eventID<<" hit arrays (peak_time), skipping \n";
            returnValue = false;
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
    if(!returnValue){}
    else if(trackIDT->size()==0){
        std::cout<<"No tracks reconstructed in event "<<eventID<<", skipping track plotting \n";
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
                std::cout<<"Encountered overflow in event "<<eventID<<" track arrays (nTracks), skipping \n";
                returnValue = false;
                clearTracks = false;
                break;
            }

            m->SetMarkerStyle(24);
            m->SetMarkerColor(2+j);

            recordInLegend = true;

            for(int k = 0; k < wireIDT->size(); ++k){
                //check for overflows
                if(!(k < integralT->size())){
                    std::cout<<"Encountered overflow in event "<<eventID<<" track arrays (integralT), skipping \n";
                    returnValue = false;
                    clearTracks = false;
                    break;
                }
                if(!(k < TPCIDT->size())){
                    std::cout<<"Encountered overflow in event "<<eventID<<" track arrays (TPCIDT), skipping \n";
                    returnValue = false;
                    clearTracks = false;
                    break;
                }
                if(!(k < peak_timeT->size())){
                    std::cout<<"Encountered overflow in event "<<eventID<<" track arrays (peak_timeT), skipping \n";
                    returnValue = false;
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
    

    delete treeHit;
    delete treeTrack;

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
    //c->Show();

    gPad->Modified();
    gPad->Update();
    gSystem->ProcessEvents();
    //gSystem->ProcessEvents();

    return returnValue;

}