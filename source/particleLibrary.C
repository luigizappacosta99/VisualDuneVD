#include <cmath>
#include <string>
#include <map>

const char* elementSymbols[] = {
    NULL,   // index 0 unused
    "H",                                                                                                                                                                                 "He",
    "Li", "Be",                                                                                                                             "B",  "C",  "N",  "O",  "F",  "Ne",
    "Na", "Mg",                                                                                                                             "Al", "Si", "P",  "S",  "Cl", "Ar",
    "K",  "Ca", "Sc", "Ti", "V",  "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr",
    "Rb", "Sr", "Y",  "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I",  "Xe",
    "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu",
                      "Hf", "Ta", "W",  "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn",
    "Fr", "Ra", "Ac", "Th", "Pa", "U"
};


const char* simpleParticlesLibrary(int particleID){
    switch (particleID)
    {
    case 11: return "e-";
    case -11: return "e+";
    case 12: return "nu_e";
    case -12: return "anu_e";
    case 13: return "mu-";
    case -13: return "mu+";
    case 14: return "nu_mu";
    case -14: return "anu_mu";
    case 15: return "tau-";
    case -15: return "tau+";
    case 16: return "nu_tau";
    case -16: return "anu_tau";
    case 22: return "gamma";
    case 111: return "pi0";
    case 211: return "pi+";
    case -211: return "pi-";
    case 2112: return "n";
    case 2212: return "p+";
    case -2212: return "p-";
    default: {
        static char buffer[64];
        std::string s = std::to_string(particleID);
        strncpy(buffer, s.c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        return buffer;
    }
    }
}

const char* nucleiLibrary(long particleID)
{

    static char buffer[64];

    //exitaton
    int excitation = particleID % 10;
    int iter1 = std::floor(particleID/10);

    //atomic mass
    int atomicMass = iter1 % 1000;
    int iter2 = std::floor(iter1/1000);

    //atomic number
    int atomicNumber = iter2 % 1000;

    std::string outString = std::to_string(atomicMass)
                          + "-" + std::string(elementSymbols[atomicNumber])
                          + ", I=" + std::to_string(excitation);

    strncpy(buffer, outString.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    return buffer;

}

const char* particleLibrary(long particleID)
{
    bool isAntiparticle = (particleID < 0);
    //if particle is an atomic nuclei, call function for reading what kind
    if(particleID>1e9 && particleID<2e9){return nucleiLibrary(particleID);}
    //else goes into single particle macros (TO DO)
    else{return simpleParticlesLibrary(particleID);}
}