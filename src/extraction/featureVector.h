/*
    featureVector.h
    
    class for holding features extracted
    
    Summer 2013
    cs1629@txstate.edu
*/


#ifndef FEATUREVECTOR_H
#define FEATUREVECTOR_H


#include <map>
#include <string>
#include <fstream>


/* the possible values of a given feature */
/*union FeatureValue
{
    long ival;
    double fval;
};*/
typedef double FeatureValue;


/*  */
/* TODO: eventually will probably need custom key ordering... */
class FeatureVector : public std::map<std::string, FeatureValue>
{
public:
    FeatureVector ()
    {
    }
    
    void write (std::ofstream &outfile);
private:
    
};


#endif  /* end of featureVector.h */
