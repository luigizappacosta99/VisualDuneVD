#include "source/workflowParams.C"
#include "source/analysisClass.C"
//#include "source/utils.C"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void duneVisual() 
{

    int stallCheck = 0;
    std::string input;

    std::string infilename = "";
    
    //initiate workflowParams
    workflowParams* params = workflowParams::Instance();
    params->setFromFile();

    analysisClass* analysis = new analysisClass();

    std::cout << "duneVisual " << params->getOutName() << std::endl;

    std::string mode = "single";

    std::cout<<"\n"
             <<"    |\\  |\\ \n"
             <<"    | \\ | \\ \n"
             <<"    \\  ︶  | \n"
             <<"     \\  O O/ \n"
             <<"      \\   / \n"
             <<"      /   \\  \n"
             <<"  ┏━━━━∪━∪━━━━━━━━━━━━━━━┓\n"
             <<"  ╏ duneVisual  v0.5.00  ╏\n"
             <<"  ╏ By Luigi Zappacosta  ╏\n"
             <<"  ╏ Now on Lyon cluster! ╏\n"
             <<"  ┗━━━━━━━━━━━━━━━━━━━━━━┛\n"
             <<"\n";

    std::cout<<"Insert name of input file: ";
    std::cin>>infilename;

    analysis->openInputFile(infilename);
    
    modeCheck:

    if(mode=="loop"){

        int myTotEvts = 0;
        std::cout<<"Insert number of events: ";
        std::cin>>myTotEvts;

        analysis->setOutFiles();

        int idx = 0;
        int skippedEvts = 0;

        while(idx<myTotEvts){
            analysis->dumpAction(idx);
            if(params->getCustomAnalysis()){analysis->customAction(idx);}
            if(params->getGraph()){analysis->graphAction(idx);}
            ++idx;
        }

        analysis->endAnalysis();
        if(params->getCustomAnalysis()){analysis->endCustomAnalysis();}

        std::cout<<"-----------------------------------------------------\n";
        std::cout<<" Task finished with "<<myTotEvts<<" events analysed,\n";
        std::cout<<" skipped "<<skippedEvts<<" events,\n";
        std::cout<<"-----------------------------------------------------\n";

        stallCheck = 0;
    }
    else if(mode=="single"){
        stallCheck = 0;
        while(true){
            std::cout<< "EventID (q to quit): ";
            std::cin>>input;
            if(isNumber(input)){
                
                int idx=std::stoi(input);

                params->setOutName("singleEvent_"+input);
                analysis->setOutFiles();

                analysis->dumpAction(idx);
                if(params->getCustomAnalysis()){analysis->customAction(idx);}
                if(params->getGraph()){analysis->graphAction(idx);}

            }
            else if (input=="q"){goto exitCheck;}
            else{
                std::cout << "[WARNING] Wrong input format, retry" << std::endl;
                continue;
            }
        }
    }
    else{
        stallCheck = 0;
        std::cout<<"[INFO] Wrong mode inserted, please use either \"single\" or \"loop\": "; 
        std::cin>>mode;
        goto modeCheck;
    }

    exitCheck:

    stallCheck++;
    if(stallCheck>3){
        std::cout<<"[INFO] Error occurred, killing VisualDuneVD... \n";
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
        std::cout<<"Insert new input file name: ";
        std::cin>>infilename;
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
