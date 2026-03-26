#ifndef ANALYSISCLASS_H
#define ANALYSISCLASS_H

class analysisClass
{
    public:

    analysisClass();
    ~analysisClass();

    void openInputFile(std::string infilename);

    void setOutFiles();

    void graphAction(int);
    void dumpAction(int);
    void customAction(int);

    void endAnalysis();
    void endCustomAnalysis();

    private:

    workflowParams* params;

    TFile* infile;
    TDirectory* dir;

    TTree* edepTree;
    TTree* eventTree;
    TTree* hitTree;
    TTree* pointsTree;
    TTree* trackTree;
    TTree* particleTree;
    TTree* simTree;
    TTree* g4Tree; 

    std::string outfilename;

    TFile* graphFile;
    TFile* extraFile;
    std::ostream* oss;
    std::ofstream fileStream;

    //ALL DATA
    UInt_t eventID;

    //graphmacro
    std::vector<unsigned int>* wireID;
    std::vector<unsigned int>* TPCID;
    std::vector<unsigned int>* planeID;
    std::vector<float>* peak_time;
    std::vector<double>* integral;

    std::vector<unsigned int>* wireIDT;
    std::vector<unsigned int>* planeIDT;
    std::vector<unsigned int>* trackIDT;
    std::vector<unsigned int>* TPCIDT;
    std::vector<float>* peak_timeT;
    std::vector<double>* integralT;
    std::vector<int>* trackPIDS;

    bool clearHits = true;
    bool clearTracks = true;


    //dumpmacro
    Float_t nuEnergy;
    Float_t px;
    Float_t py;
    Float_t pz;
    Int_t nuPDG;
    Int_t leptonPDG;
    Int_t CCNC;
    Int_t nParticles;
    std::vector<float>* particleE;
    std::vector<int>* particlePDG;
    std::vector<int>* particleTrackID;

    //custommacro
    std::vector<float>* edepE;
    Int_t nPoints;

    TH1F* hTotal;
    TH1F* hReco2;

    int    nBins  = 50;
    double xMin   = 0.0;
    double xMax   = 1.0;

};

#endif