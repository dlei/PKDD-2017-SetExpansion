#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "../utils/utils.h"


string corpusName = "apr";
string outputSuffix = "3seeds"; 
string conceptSuffix = "country";

// Input: 1) entity-context graph, 2) entity-type graph, 3) entity name map
string inFileName; 
string inTypeFeatureFileName;
string entityMapFileName;

// Query: each line is a query
string inputSeedFileName;

// Output: 1) expansion results, 2) model parameters
string outputFileName;
string outputParameterFileName;


/*
Running settings
*/
bool FLAG_MULTI_SET = false; // multiset expansion or nor
bool DEBUG_FIRST_RUN = false; 
bool FLAG_USE_TYPE = true; // Use Type information or not
bool FLAG_DEBUG = true; // Print out debugging information or not
bool FLAG_SEED_ENSEMBLE = false; // Seed ensemble or not

/*
Parameters used in our model
*/
int PARAM_MAX_ITER = 10;
int PARAM_PATTERN_DIVIERSITY_LOW = 2;
int PARAM_PATTERN_DIVIERSITY_HIGH = 30; 
double PARAM_PATTERN_QUALITY_THRES = 0;
int PARAM_PATTERN_TOPK = 150;
int PARAM_ENTITY_TOPK = 5;
double PARAM_ENTITY_ZSCORE_THRES = 1.5;
double PARAM_TYPE_WEIGHT = 0.1;
double PARAM_PATTERN_NEG_DISCOUNT = 0.2;
double PARAM_ENTITY_NEG_DISCOUNT = 1.0;


double PARAM_PATTERN_SET_SIZE_PERC = 0.7;
int PARAM_PATTERN_ENSEMBLE_TIMES = 60;
double PARAM_TOPK_EACH_BATCH = 20;
double PARAM_RANK_THRESHOLD = 15;

double EPS = 1e-9;
double NEG_INF = -1e10;


/*
Parameters used in MEB/WMEB
*/
int SET_NUMBER = 2; // number of mutually exclusive sets
int PARAM_NC = 300;  // number of newly added patterns in each iteration
int PARAM_NT = 10; // number of newly added terms in each iteration

int PARAM_ENITYT_MIN_SUP = 5; // an entity should at least apper 5 times
int PARAM_PATTERN_MIN_SUP = 3; // a pattern should at least apper 5 times
int PARAM_PATTERN_MIN_DIVERSITY = 2; // a pattern should extract at least 2 entities


inline void updateParameters()
{
    /*Server file format*/
    inFileName = "../data_new/"+corpusName+"/intermediate/eidSkipgram2TFIDFStrength.txt";
    // inFileName = "../data_new/"+corpusName+"/intermediate/eidSkipgramID2TFIDFStrength.txt";
    inTypeFeatureFileName = "../data_new/"+corpusName+"/intermediate/eidCTypeStrengths.txt";
    entityMapFileName = "../data_new/"+corpusName+"/intermediate/entity2id.txt";

    inputSeedFileName = "../data_new/"+corpusName+"/source/query-"+conceptSuffix+".txt";
    
    outputFileName = "../result/"+corpusName+"/results-"+outputSuffix+".txt";
    outputParameterFileName = "../result/"+corpusName+"/params-"+outputSuffix+".txt";   

    /*Local file format*/
    // inFileName = "../data/"+corpusName+"/eidSkipgram2TFIDFStrength.txt";
    // inTypeFeatureFileName = "../data/"+corpusName+"/eidCTypeStrengths.txt";
    // entityMapFileName = "../data/"+corpusName+"/entity2id.txt";

    // inputSeedFileName = "../data/"+corpusName+"/query-"+corpusName+".txt";
    
    // outputFileName = "../result/"+corpusName+"/results-"+outputSuffix+".txt";
    // outputParameterFileName = "../result/"+corpusName+"/params-"+outputSuffix+".txt";   
}


inline void saveParameters(const string &outputParameterName)
{
    ofstream outFile;
    outFile.open(outputParameterName);

    if(outFile.is_open()) {
        outFile << "max_iter = " << PARAM_MAX_ITER << endl;
        outFile << "Q = " << PARAM_PATTERN_TOPK << endl;
        outFile << "A = " << PARAM_ENTITY_TOPK << endl;
        outFile << "T = " << PARAM_PATTERN_ENSEMBLE_TIMES << endl;
        outFile << "alpha = " << PARAM_PATTERN_SET_SIZE_PERC << endl;
        outFile << "PATTERN_DIVIERSITY_LOW = " << PARAM_PATTERN_DIVIERSITY_LOW << endl;
        outFile << "PATTERN_DIVIERSITY_HIGH = " << PARAM_PATTERN_DIVIERSITY_HIGH << endl;
        // outFile << "PARAM_PATTERN_QUALITY_THRES = " << PARAM_PATTERN_QUALITY_THRES << endl;
        // outFile << "PARAM_ENTITY_ZSCORE_THRES = " << PARAM_ENTITY_ZSCORE_THRES << endl;
        // outFile << "PARAM_TYPE_WEIGHT = " << PARAM_TYPE_WEIGHT << endl;
        // outFile << "PARAM_PATTERN_NEG_DISCOUNT = " << PARAM_PATTERN_NEG_DISCOUNT << endl;
        outFile.close();
    } else {
        cout << "Could not write into file: " << outputParameterName << endl;
    }
}


#endif
