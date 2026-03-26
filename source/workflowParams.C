#include "../include/workflowParams.h"
#include <string>

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
bool workflowParams::getGraph(){return _graphOn;}
bool workflowParams::getCustomAnalysis(){return _customAnalysisOn;}
std::string workflowParams::getOutName(){return _outName;}

void workflowParams::setVerboseTruth(bool input){_verboseTruth=input;}
void workflowParams::setVerboseG4(bool input){_verboseG4=input;}
void workflowParams::setGraph(bool input){_graphOn=input;}
void workflowParams::setCustomAnalysis(bool input){_customAnalysisOn=input;}
void workflowParams::setOutName(std::string input){_outName=input;}


static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    std::string result = s.substr(start, end - start + 1);
    // Strip surrounding double quotes if present
    if (result.size() >= 2 && result.front() == '"' && result.back() == '"')
        result = result.substr(1, result.size() - 2);
    return result;
}

void workflowParams::setFromFile() {
    std::ifstream file("../txt_files/settings.txt");
    if (!file.is_open())
        throw std::runtime_error("[INFO] Cannot open config file");

    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        ++lineNum;

        // Skip empty lines and comments
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#')
            continue;

        // Split on the first ':'
        size_t colonPos = trimmedLine.find(':');
        if (colonPos == std::string::npos) {
            std::cerr << "Warning: line " << lineNum << " has no ':' separator, skipping.\n";
            continue;
        }

        std::string key   = trim(trimmedLine.substr(0, colonPos));
        std::string value = trim(trimmedLine.substr(colonPos + 1));

        // --- Bool keys ---
        if (key == "verboseTruth" || key == "verboseG4" ||
            key == "graphOn"      || key == "customAnalysis")
        {
            bool bval;
            if      (value == "true")  bval = true;
            else if (value == "false") bval = false;
            else {
                std::cerr << "Warning: line " << lineNum
                          << ": expected true/false for key '" << key
                          << "', got '" << value << "', skipping.\n";
                continue;
            }

            if      (key == "verboseTruth")     workflowParams::setVerboseTruth(bval);
            else if (key == "verboseG4")        workflowParams::setVerboseG4(bval);
            else if (key == "graphOn")          workflowParams::setGraph(bval);
            else if (key == "customAnalysis")   workflowParams::setCustomAnalysis(bval);
        }

        else if (key == "outputName"){setOutName(value);}

        // --- Unknown key ---
        else {std::cerr << "Warning: line " << lineNum << ": unknown key '" << key << "', skipping.\n";}
    }
}
