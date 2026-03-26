#include <vector>

//is char a number?
bool isNumber(std::string& str){
    for(auto c : str){
        if(c<'0'||c>'9'){return false;}
    }
    return true;
}

//vector sums
int sum(std::vector<int> v){
    int sum_of_elems = 0;
    for (auto& e : v){sum_of_elems += e;}
    return sum_of_elems;
}

long sum(std::vector<long> v){
    long sum_of_elems = 0;
    for (auto& e : v){sum_of_elems += e;}
    return sum_of_elems;
}

float sum(std::vector<float> v){
    float sum_of_elems = 0;
    for (auto& e : v){sum_of_elems += e;}
    return sum_of_elems;
}

double sum(std::vector<double> v){
    double sum_of_elems = 0;
    for (auto& e : v){sum_of_elems += e;}
    return sum_of_elems;
}

int sum(std::vector<int> *v){
    int sum_of_elems = 0;
    if(!v){return sum_of_elems;}
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}

long sum(std::vector<long> *v){
    long sum_of_elems = 0;
    if(!v){return sum_of_elems;}
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}

float sum(std::vector<float> *v){
    float sum_of_elems = 0;
    if(!v){return sum_of_elems;}
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}

double sum(std::vector<double> *v){
    double sum_of_elems = 0;
    if(!v){return sum_of_elems;}
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}
     