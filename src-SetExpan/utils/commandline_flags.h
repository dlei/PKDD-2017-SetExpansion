#ifndef __COMMANDLINE_FLAGS_H__
#define __COMMANDLINE_FLAGS_H__

#include "../utils/utils.h"
#include "../utils/parameters.h"

template<class T>
inline void fromString(const string &s, T &x)
{
 stringstream in(s);
 in >> x;
}

void parseCommandFlags(int argc, char* argv[])
{
    if (argc == 1) {
        cout << "Use default parameter settings" << endl;
    } else {
        for (int i = 1; i < argc; ++ i) {
            if (!strcmp(argv[i], "-max_iter")) {
                fromString(argv[++ i], PARAM_MAX_ITER);
            } else if (!strcmp(argv[i], "-in_seed")) {
                fromString(argv[++ i], inputSeedFileName);
            } else if (!strcmp(argv[i], "-out_result")) {
                fromString(argv[++ i], outputFileName);
            } else if (!strcmp(argv[i], "-out_para")) {
                fromString(argv[++ i], outputParameterFileName);
            } else if (!strcmp(argv[i], "-p_div_low")) {
                fromString(argv[++ i], PARAM_PATTERN_DIVIERSITY_LOW);
            } else if (!strcmp(argv[i], "-p_div_high")) {
                fromString(argv[++ i], PARAM_PATTERN_DIVIERSITY_HIGH);
            } else if (!strcmp(argv[i], "-p_qual_thres")) {
                fromString(argv[++ i], PARAM_PATTERN_QUALITY_THRES);
            } else if (!strcmp(argv[i], "-p_neg_disc")) {
                fromString(argv[++ i], PARAM_PATTERN_NEG_DISCOUNT);
            } else if (!strcmp(argv[i], "-e_zscore_thres")) {
                fromString(argv[++ i], PARAM_ENTITY_ZSCORE_THRES);
            } else if (!strcmp(argv[i], "-e_neg_disc")) {
                fromString(argv[++ i], PARAM_ENTITY_NEG_DISCOUNT);
            } else if (!strcmp(argv[i], "-type_weight")) {
                fromString(argv[++ i], PARAM_TYPE_WEIGHT);
            } else if (!strcmp(argv[i], "-corpus")) {
                fromString(argv[++ i], corpusName);
            } else if (!strcmp(argv[i], "-suffix")) {
                fromString(argv[++ i], outputSuffix);
            } else if (!strcmp(argv[i], "-concept")) {
                fromString(argv[++ i], conceptSuffix);
            } else if (!strcmp(argv[i], "-T")) {
                fromString(argv[++ i], PARAM_PATTERN_ENSEMBLE_TIMES);
            } else if (!strcmp(argv[i], "-alpha")) {
                fromString(argv[++ i], PARAM_PATTERN_SET_SIZE_PERC);
            } else if (!strcmp(argv[i], "-Q")) {
                fromString(argv[++ i], PARAM_PATTERN_TOPK);
            } else if (!strcmp(argv[i], "-A")) {
                fromString(argv[++ i], PARAM_ENTITY_TOPK);
            } else {
                fprintf(stderr, "[Warning] Unknown Parameter: %s\n", argv[i]);
            }
        }        
    }
}


#endif
