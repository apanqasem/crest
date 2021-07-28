/*
    featureVector.C
    
    implementation for FeatureVector
    
    Summer 2013
    cs1629@txstate.edu
*/


#include "featureVector.h"


/* write out the feature vector to a file */
void FeatureVector::write (std::ofstream &outfile)
{
    std::map<std::string, FeatureValue>::iterator iter;
    
    /* run through all features that have been extracted */
    for (iter = this->begin (); iter != this->end (); iter++)
    {
        outfile << "<" << iter->first << ", " << iter->second << ">" << std::endl;
    }
}


/* end of featureVector.C */
