#ifndef WORKFLOWPARAMS_H
#define WORKFLOWPARAMS_H

class workflowParams
{
public:

    static workflowParams* Instance();

    //get functions
    bool getVerboseTruth();
    bool getVerboseG4();
    bool getCustomAnalysis();
    bool getGraph();
    std::string getOutName();

    //set functions
    void setFromFile();

    void setVerboseTruth(bool);
    void setVerboseG4(bool);
    void setCustomAnalysis(bool);
    void setGraph(bool);
    void setOutName(std::string);

    //void readConfig();

protected:
    static workflowParams* _workflowParamsPointer;

private:
    // Private constructor to prevent creation
    workflowParams();
    ~workflowParams();

    //params
    bool _verboseTruth;
    bool _verboseG4;
    bool _customAnalysisOn;
    bool _graphOn;
    std::string _outName;

    workflowParams(const workflowParams&) = delete;
    workflowParams& operator=(const workflowParams&) = delete;
};

#endif // SINGLETON_H