#include "source/graphMacro.C"
#include "source/dumpMacro.C"
#include "include/workflowParams.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

bool isNumber(std::string& str){
    for(auto c : str){
        if(c<'0'||c>'9'){return false;}
    }
    return true;
}

void visualDuneVD(std::string filename, std::string mode = "single", std::string outFolder = ".") 
{

    int stallCheck = 0;

    TFile* file = TFile::Open(filename.data());
    if (!file || file->IsZombie()) {
        std::cerr << "Error: cannot open file " << filename << std::endl;
        return;
    }
    const char* foldername = "ana";
    TDirectory* dir = (TDirectory*)file->Get(foldername);
    if (!dir) {
        std::cerr << "Error: folder '" << foldername << "' not found in the file." << std::endl;
        file->Close();
        return;
    }

    TTree* treeEvent = (TTree*)dir->Get("eventData");
    unsigned int eventID;
    treeEvent->SetBranchAddress("eventID",&eventID);

    std::string input;
    char* outname;
    char* rootname;

    TCanvas* c = new TCanvas();
    bool isCanvasOpen = false;
    bool isEventFull = true;
    
    //initiate workflowParams
    workflowParams* params = workflowParams::Instance();
    params->setVerboseTruth(true);
    params->setVerboseG4(false);

    std::cout<<"\n"
             <<"    |\\  |\\ \n"
             <<"    | \\ | \\ \n"
             <<"    \\  ︶  | \n"
             <<"     \\  O O/ \n"
             <<"      \\   / \n"
             <<"      /   \\  \n"
             <<"  ┏━━━━∪━∪━━━━━━━━━━━━━━━┓\n"
             <<"  ╏ VisualDuneVD v0.4.30 ╏\n"
             <<"  ╏ By Luigi Zappacosta  ╏\n"
             <<"  ╏ Now on Lyon cluster! ╏\n"
             <<"  ┗━━━━━━━━━━━━━━━━━━━━━━┛\n"
             <<"\n";

    
    modeCheck:

    if(mode=="loop"){
        int myTotEvts = 0;
        std::string outname;
        std::cout<<"Insert number of events: ";
        std::cin>>myTotEvts;
        std::cout<<"Insert output file name: ";
        std::cin>>outname;

        //energy calibration stuff
        TGraph* calibration = new TGraph();

        int myEventID = 0;
        int skippedEvts = 0;

        TFile* f = new TFile((outname+".root").data(),"RECREATE");
        f->cd();

        std::ofstream fTxt((outname+".txt").data());

        while(myEventID<myTotEvts){
            treeEvent->GetEntry(myEventID);
            if(isCanvasOpen){
                c->Close();
                c = new TCanvas(std::to_string(eventID).c_str(),std::to_string(eventID).c_str(),1920,360);
                isCanvasOpen = false;
            }
            isEventFull = graphMacro(c,myEventID,dir);
            isCanvasOpen = true;
            if(isEventFull){
                c->Write();
                fTxt << dumpMacro(myEventID, dir, (outname+".txt").data(), calibration).str();
                std::cout<<"Event #"<<eventID<<" printed and saved;"<<std::endl;
            }
            else{++skippedEvts;}
            ++myEventID;
        }

        f->Close();

        std::cout<<"-----------------------------------------------------\n";
        std::cout<<" Task finished with "<<myTotEvts<<" events analysed,\n";
        std::cout<<" skipped "<<skippedEvts<<" events,\n";
        std::cout<<"-----------------------------------------------------\n";

        calibration->Draw("rs ");
        calibration->SaveAs("prova.root");
        stallCheck = 0;
    }
    else if(mode=="single"){
        stallCheck = 0;
        while(true){
            std::cout<< "EventID (q to quit): ";
            std::cin>>input;
            if(isNumber(input)){
                if(isCanvasOpen){
                    c->Close();
                    c = new TCanvas();
                    isCanvasOpen = false;
                }
                int myEventID=std::stoi(input);
                isEventFull = graphMacro(c,myEventID,dir);
                isCanvasOpen = true;
                std::cout << dumpMacro(myEventID, dir).str();
                if(isEventFull){
                    outname = ("./single" + input + ".pdf").data();
                    rootname = ("./single" + input + ".root").data();
                    c->SaveAs(outname);
                    c->SaveAs(rootname);
                }
            }
            else if (input=="q"){goto exitCheck;}
            else{continue;}
        }
    }
    else{
        stallCheck = 0;
        std::cout<<"Wrong mode inserted, please use either \"single\" or \"loop\": "; 
        std::cin>>mode;
        goto modeCheck;
    }

    exitCheck:

    stallCheck++;
    if(stallCheck>3){
        std::cout<<"Error occurred, killing VisualDuneVD... \n";
        return;
    }
    char input2;
    std::cout<<"Press: 'q' to exit program,\n";
    std::cout<<"       'c' to change filenale,\n";
    std::cout<<"       'm' to change mode,\n";
    std::cout<<".      'h' for documentation\n";
    std::cin>>input2;

    switch (input2)
    {
    case 'q':{
        std::cout << "quitting\n";
        return;}
    case 'c':{
        std::cout<<"Insert new filename: ";
        std::cin>>filename;
        goto modeCheck;}
    case 'm':{
        std::cout<<"Insert new mode: ";
        std::cin>>mode;
        goto modeCheck;}
    case 'h':{
        goto documentation;}
    default: 
        goto exitCheck;
    }

    documentation:

    std::ifstream documentationFile("./txt_files/documentation.txt");
    std::string line;

    std::cout<<"\n";
    while (std::getline(documentationFile, line)) {
        std::cout << line << "\n";
    }
    std::cout<<"\n";

    documentationFile.close();
    goto exitCheck;
}
