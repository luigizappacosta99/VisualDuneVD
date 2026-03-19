#ifndef SINGLETON_H
#define SINGLETON_H

class workflowParams
{
public:
    static workflowParams* Instance();

    //get functions
    bool getVerboseTruth();
    bool getVerboseG4();

    //set functions
    void setVerboseTruth(bool);
    void setVerboseG4(bool);

    void readConfig();

protected:
    static workflowParams* _workflowParamsPointer;

private:
    // Private constructor to prevent creation
    workflowParams();
    ~workflowParams();

    //params
    //workflowParams* _workflowParamsPointer;
    bool _verboseTruth;
    bool _verboseG4;

    workflowParams(const workflowParams&) = delete;
    workflowParams& operator=(const workflowParams&) = delete;
};

#endif // SINGLETON_H