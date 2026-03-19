#include <vector>

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
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}

long sum(std::vector<long> *v){
    long sum_of_elems = 0;
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}

float sum(std::vector<float> *v){
    float sum_of_elems = 0;
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}

double sum(std::vector<double> *v){
    double sum_of_elems = 0;
    for (int i=0; i<v->size(); ++i){sum_of_elems += v->at(i);}
    return sum_of_elems;
}
     