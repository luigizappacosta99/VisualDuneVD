#include "../include/workflowParams.h"

workflowParams* workflowParams::_workflowParamsPointer = nullptr;

workflowParams::workflowParams(){};

workflowParams::~workflowParams(){};

workflowParams* workflowParams::Instance(){
    if(_workflowParamsPointer == nullptr){
        _workflowParamsPointer = new workflowParams();
    }
    return _workflowParamsPointer;
};

bool workflowParams::getVerboseTruth(){return _verboseTruth;}
bool workflowParams::getVerboseG4(){return _verboseG4;}

void workflowParams::setVerboseTruth(bool input){_verboseTruth=input;}
void workflowParams::setVerboseG4(bool input){_verboseG4=input;}

void readConfig(){};