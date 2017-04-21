#ifndef __SINGLESE_H__
#define __SINGLESE_H__

#include "utils/utils.h"
#include "utils/parameters.h"

inline FILE* tryOpen(const string &filename, const string &param)
{
    FILE* ret = fopen(filename.c_str(), param.c_str());
    if (ret == NULL) {
        cerr << "[Warning] failed to open " << filename  << " under parameters = " << param << endl;
    }
    return ret;
}

inline void readEntityFeatureFile(const string &inputFileName,
    unordered_map<int, unordered_set<string> > &eid2patterns,
    unordered_map<string, unordered_set<int> > &pattern2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength)
{

    ifstream inFile;
    inFile.open(inputFileName);
    if(inFile.is_open()) {
        string line;
        int eid;
        string pattern;
        double strength;
        char delim = '\t';
        int cnt = 0;
        while(inFile) {
            if (cnt % 1000000 == 0 and cnt != 0) {
                printf("Finish loading %d lines\n", cnt);
                fflush(stdout);
            }
            getline(inFile, line);
            if(line.size() > 0) {
                vector<string> segs = splitBy(line, delim);
                eid = stoi(segs[0]);
                pattern = segs[1];
                strength = stod(segs[2]); // last column is the EgoSet TF-IDF strength

                // update eid -> patterns
                if(eid2patterns.count(eid) > 0) {
                    eid2patterns[eid].insert(pattern);
                } else {
                    unordered_set<string> tmp;
                    tmp.insert(pattern);
                    eid2patterns[eid] = tmp;
                }

                // udpate pattern -> eids
                if(pattern2eids.count(pattern) > 0) {
                    pattern2eids[pattern].insert(eid);
                } else {
                    unordered_set<int> tmp;
                    tmp.insert(eid);
                    pattern2eids[pattern] = tmp;
                }

                // update (eid, pattern) -> strength
                if(eid2Pattern2Strength.count(eid) > 0) {
                    eid2Pattern2Strength[eid][pattern] = strength;
                } else {
                    unordered_map<string, double> tmp;
                    tmp[pattern] = strength;
                    eid2Pattern2Strength[eid] = tmp;
                }
            }
            cnt ++;
        }

        inFile.close();
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }

    printf("%s\n", "Finish loading files");
    printf("Number of entities = %lu\n", eid2patterns.size());
    printf("Number of patterns = %lu\n", pattern2eids.size());
}

inline void readEntityFeatureFileFast(const string &inputFileName,
    unordered_map<int, unordered_set<string> > &eid2patterns,
    unordered_map<string, unordered_set<int> > &pattern2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength)
{

    int MAX_STRING = 1000;
    int eid;
    char skipgram[MAX_STRING];
    double strength1, strength2;
    unordered_map<int, vector<string> > eid2patternsTmp;
    unordered_map<string, vector<int> > pattern2eidTmp;

    FILE* in = tryOpen(inputFileName, "r");
    int cnt = 0;
    while(fscanf(in, "%d\t%[^\t]\t%lf\t%lf", &eid, skipgram, &strength1, &strength2 ) == 4) {
        cnt ++ ; 
        if (cnt % 1000000 == 0 and cnt != 0) {
            printf("Finish loading %d lines\n", cnt);
            fflush(stdout);
        }

        string pattern(skipgram);
        
        // update eid -> patterns
        if(eid2patternsTmp.count(eid) > 0) {
            eid2patternsTmp[eid].push_back(pattern);
        } else {
            vector<string> tmp;
            tmp.push_back(pattern);
            eid2patternsTmp[eid] = tmp;
        }

        // udpate pattern -> eids
        if(pattern2eidTmp.count(pattern) > 0) {
            pattern2eidTmp[pattern].push_back(eid);
        } else {
            vector<int> tmp;
            tmp.push_back(eid);
            pattern2eidTmp[pattern] = tmp;
        }

        // update (eid, pattern) -> strength
        if(eid2Pattern2Strength.count(eid) > 0) {
            eid2Pattern2Strength[eid][pattern] = strength1;
        } else {
            unordered_map<string, double> tmp;
            tmp[pattern] = strength1;
            eid2Pattern2Strength[eid] = tmp;
        }
    }
    fclose(in);

    for(auto ele:eid2patternsTmp) {
        eid2patterns[ele.first] = unordered_set<string> (ele.second.begin(), ele.second.end());
    }

    for(auto ele:pattern2eidTmp) {
        pattern2eids[ele.first] = unordered_set<int> (ele.second.begin(), ele.second.end());
    }


    printf("%s\n", "Finish loading files");
    printf("Number of entities = %lu\n", eid2patterns.size());
    printf("Number of patterns = %lu\n", pattern2eids.size());
}

inline void readEntityTypeFeature(const string &inputFileName,
    unordered_map<int, unordered_set<string> > &eid2types,
    unordered_map<string, unordered_set<int> > &type2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Type2Strength)
{

    ifstream inFile;
    inFile.open(inputFileName);
    if(inFile.is_open()) {
        string line;
        int eid;
        string ctype;
        double strength;
        char delim = '\t';
        while(inFile) {
            getline(inFile, line);
            if(line.size() > 0) {
                vector<string> segs = splitBy(line, delim);
                eid = stoi(segs[0]);
                ctype = segs[1];
                strength = stod(segs[3]); // last column is the tf-idf score used

                // update eid -> types
                if(eid2types.count(eid) > 0) {
                    eid2types[eid].insert(ctype);
                } else {
                    unordered_set<string> tmp;
                    tmp.insert(ctype);
                    eid2types[eid] = tmp;
                }

                // udpate type -> eids
                if(type2eids.count(ctype) > 0) {
                    type2eids[ctype].insert(eid);
                } else {
                    unordered_set<int> tmp;
                    tmp.insert(eid);
                    type2eids[ctype] = tmp;
                }

                // update eid -> (type -> strength )
                if(eid2Type2Strength.count(eid) > 0) {
                    eid2Type2Strength[eid][ctype] = strength;
                } else {
                    unordered_map<string, double> tmp;
                    tmp[ctype] = strength;
                    eid2Type2Strength[eid] = tmp;
                }
            }
        }

        inFile.close();
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }

    printf("%s\n", "Finish loading files");
    printf("Number of entities = %lu\n", eid2types.size());
    printf("Number of types = %lu\n", type2eids.size());

    // cout << eid2Type2Strength[73205]["PERSON"] << endl;
    // cout << eid2Type2Strength[114368]["ORGANIZATION"] << endl;
    // cout << eid2Type2Strength[5603]["DIGIT"] << endl;
    // cout << eid2types[73205].size() << endl;
    // cout << eid2types[114368].size() << endl;
}

inline void scorePattern(unordered_map<string, double> &candidatePattern2Strength,    
    vector<int> &seedEids, 
    vector<int> &negativeEids,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength,
    unordered_map<string, unordered_set<int> > &pattern2eids,
    bool DEBUG_FLAG,
    bool NEGATIVE_FLAG = false) 
{

    /*
    NOTE: Change prelimnary pattern scoring method in this function
    Currently, the score of each pattern is sum of the strength of 
    its attached "positive" entities minus the sum of the strength of
    its attached "negative" entities (with a discount 0.2)
    */

    for(auto ele:seedEids) {
        for(auto patternStrengthPair:eid2Pattern2Strength[ele]) {
            string pattern = patternStrengthPair.first;
            double strength = patternStrengthPair.second;

            int patternDiversity = pattern2eids[pattern].size();
            if(patternDiversity < PARAM_PATTERN_DIVIERSITY_LOW or patternDiversity > PARAM_PATTERN_DIVIERSITY_HIGH) {
                continue; // filter patterns extracting < 3 or > 30 entities
            } else { // scoring the pattern left
                if(candidatePattern2Strength.count(pattern) > 0) {
                    candidatePattern2Strength[pattern] += strength;
                } else {
                    candidatePattern2Strength[pattern] = strength;
                }
            }
        }
    }

    // discount pattern matched with negative seeds
    if(NEGATIVE_FLAG) {
        for(auto ele:negativeEids) {
            for(auto patternStrengthPair:eid2Pattern2Strength[ele]) {
                string pattern = patternStrengthPair.first;
                double strength = patternStrengthPair.second;
                if(candidatePattern2Strength.count(pattern) > 0) {
                    candidatePattern2Strength[pattern] -= (PARAM_ENTITY_NEG_DISCOUNT * strength);
                }
            }
        }        
    }

    if(DEBUG_FLAG) {
        printf("Number of patterns after diversity filtering = %lu\n", candidatePattern2Strength.size());
    }
}

inline void selectCoreType(string &coreType, 
    vector<int> &seedEids, 
    vector<int> &negativeEids,
    unordered_map<int, unordered_map<string, double> > &eid2Type2Strength,
    bool DEBUG_FLAG,
    bool NEGATIVE_FLAG = false)
{
    /*
    NOTE: change type selection method in this function.
    Currently, we select only one "coreType" which should match with most positive examples, and
    if mulitple types can match the same number of positive example, we select the one with the 
    highest strength.
    NOTE: the negative examples are not used currently.
    */

    unordered_map<string, pair<int, double>> candidateType2CountStrength;
    for(auto ele:seedEids) {
        for(auto typeStrengthPair:eid2Type2Strength[ele]) {
            string type = typeStrengthPair.first;
            double strength = typeStrengthPair.second;

            if(candidateType2CountStrength.count(type) > 0) {
                candidateType2CountStrength[type].first += 1;
                candidateType2CountStrength[type].second += strength;
            } else {
                pair<int, double> tmp (1, strength);
                candidateType2CountStrength[type] = tmp;
            }
        }
    }

    if(NEGATIVE_FLAG) {
        // currently do nothing
    }

    coreType = ""; 
    int maxMatchCount = -1;
    double maxStrength = -1e10;
    for(auto ele:candidateType2CountStrength) {
        string type = ele.first;
        int matchedSeedCount = ele.second.first;
        double strength = ele.second.second;

        if(matchedSeedCount > maxMatchCount) {
            coreType = type;
            maxMatchCount = matchedSeedCount;
            maxStrength = strength;
        }
        else if (matchedSeedCount == maxMatchCount) {
            if(strength > maxStrength) {
                coreType = type;
                maxStrength = strength;
            }
        }
    }

    if(DEBUG_FLAG) {
        cout << "Strongest type = " <<  coreType << ", " <<  maxMatchCount << ", " <<  maxStrength << endl;
    }
}

inline void samplePatterns(vector<string> &corePatterns, 
    vector<pair<string, double> > &topPatternWStrength,
    double sample_percentage)
{
    /*
    randomly select sample_percentage corePatterns from topPatternWStrength,
    results are put in the corePatterns
    NOTE: we can later change the pattern sampling method
    */

    unordered_set<int> corePatternIdxs;
    int nOfPatterns = topPatternWStrength.size(); 
    corePatternIdxs.insert(rand() % nOfPatterns); 
    int PARAM_PATTERN_SET_SIZE = (int) (sample_percentage * nOfPatterns); 
    // PARAM_PATTERN_SET_SIZE = PARAM_PATTERN_SET_SIZE > nOfPatterns ? nOfPatterns: PARAM_PATTERN_SET_SIZE;

    for(int j = 0; j < PARAM_PATTERN_SET_SIZE; j++)
    {
        int toInsert = rand() % nOfPatterns;
        while(corePatternIdxs.find(toInsert) != corePatternIdxs.end())
        {
            toInsert = rand() % nOfPatterns;
        }
        corePatternIdxs.insert(toInsert);
    }
    
    for(auto idx:corePatternIdxs)
    {
        corePatterns.push_back(topPatternWStrength.at(idx).first);
    }
    corePatternIdxs.clear();
}

inline double scoreEntity(const int &candidate, const int &seed,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength,
    vector<string> &corePatterns)
{
    /*
    We currently use the Weighted Jaccard similarity to score entity
    */

    double score_min = 0;
    double score_max = 0;
    double score;

    for(auto corePattern:corePatterns)
    {
        double candidatePatternStrength = 0;
        double seedPatternStrength = 0;
        if(eid2Pattern2Strength[candidate].count(corePattern) > 0) {
            candidatePatternStrength = eid2Pattern2Strength[candidate][corePattern];
        }
        if(eid2Pattern2Strength[seed].count(corePattern) > 0) {
            seedPatternStrength = eid2Pattern2Strength[seed][corePattern];
        }

        score_min += min(candidatePatternStrength, seedPatternStrength);
        score_max += max(candidatePatternStrength, seedPatternStrength);
    }

    if(abs(score_max) < EPS ) {
        score = 0;
    } else {
        score = (score_min / score_max );
    }

    return score;
}

inline void scoreEntitySet(unordered_map<int, double> &candidateEntity2Score,
    vector<int> &seedEids,
    unordered_map<string, unordered_set<int> > &pattern2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength,
    unordered_map<int, unordered_map<string, double> > &eid2Type2Strength,
    vector<string> &corePatterns,
    string &coreType, bool DEBUG_FLAG, bool TYPE_FLAG )
{

    /*
    NOTE: we select and score the candidate entities using corePatterns and coreType
    */

    unordered_set<int> candidateEntities;
    for(auto corePattern:corePatterns)
    {
        unordered_set<int> tmp = pattern2eids[corePattern];
        for(auto candidate:tmp) {
            if(TYPE_FLAG) {
                if (eid2Type2Strength[candidate].count(coreType) > 0) {
                    // filter those entity that do not match the coreType
                    candidateEntities.insert(candidate);
                }
            } else {
                candidateEntities.insert(candidate);
            }

        }
        // candidateEntities.insert(tmp.begin(), tmp.end());
    }

    for(auto candidate:candidateEntities) {
        double score = 0;
        // if(TYPE_FLAG) {
        //     if (eid2Type2Strength[candidate].count(coreType) > 0) {
        //         score += 0;
        //     } else {
        //         score += NEG_INF; // filter those entity that do not match the coreType
        //     }      
        // }

        for(auto seed:seedEids) {
            score += scoreEntity(candidate, seed, eid2Pattern2Strength, corePatterns);
        }
        candidateEntity2Score[candidate] = score;
    }

}

inline void expandSet( const unordered_set<int> &userInputSeeds,
    vector<int> &seedEids,
    unordered_map<int, unordered_set<string> > &eid2patterns,
    unordered_map<string, unordered_set<int> > &pattern2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength,

    unordered_map<int, unordered_set<string> > &eid2types,
    unordered_map<string, unordered_set<int> > &type2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Type2Strength,

    bool DEBUG_FLAG,
    bool TYPE_FLAG,
    int MAX_ITER, 
    unordered_map<int, string> &eid2ename

    )
{

    // Initialize the entity seed set to expand
    for(auto ele:userInputSeeds) {
        seedEids.push_back(ele);
    }

    // Initialize the negative entity set
    vector<int> negativeEids = {};

    // One epoch bootstrap
    for(int iter = 0; iter < MAX_ITER; iter++)
    {
        if(DEBUG_FLAG) {
            printf("Iteration: %d\n", iter);
        }

        // Step0: Cache current seed set and check later whether there is no entites added in
        unordered_set<int> prev_seeds (seedEids.begin(), seedEids.end() );
        if(DEBUG_FLAG) { 
            printf("Number of entities currently in set = %lu\n", prev_seeds.size()); 
        }

        /*
        Step 1: Candidate pattern selection with diversity filtering
        prelimnary pattern ranking with optional negative seed provided.
        
        The last argument in function scorePattern() controls whether
        the negative examples are provided or not. 

        Result are directly put in the candidatePattern2Strength
        
        NOTE: change pattern scoring method in function scorePattern()
        */
        unordered_map<string, double> candidatePattern2Strength;
        scorePattern(candidatePattern2Strength, seedEids, negativeEids, 
            eid2Pattern2Strength, pattern2eids, DEBUG_FLAG, false);


        /* 
        Step2: Select top K quality patterns
        */
        int pattern_size = candidatePattern2Strength.size();
        PARAM_PATTERN_TOPK = PARAM_PATTERN_TOPK < pattern_size ? PARAM_PATTERN_TOPK : pattern_size;
        vector<pair<string, double> > topPatternWStrength = selectTopKByKey(candidatePattern2Strength, PARAM_PATTERN_TOPK);
        if(DEBUG_FLAG) {
            printf("Number of quality patterns selected = %lu\n", topPatternWStrength.size());
            for(int i = 0; i < 10; ++i) {
                printf("Quality pattern: %s [with score = %f]\n", 
                    topPatternWStrength[i].first.c_str(), topPatternWStrength[i].second);
            }
        }

        /*
        [Optional step]: Select one coretype. 
        */
        string coreType = "";
        if(TYPE_FLAG) {
            selectCoreType(coreType, seedEids, negativeEids, eid2Type2Strength, DEBUG_FLAG, false);
        }

        // NEW: Feature ensemble module
        vector<vector<int> > extractedEidSets;
        unordered_map<int, double> eid2mrr;

        /*
        Step 3: Entity Selection via ranking-based ensemble
        */
        printf("ensemble start !!!\n");
        for(int i = 0; i < PARAM_PATTERN_ENSEMBLE_TIMES; i++)
        {
            if (i % 10 == 0 ) { printf("batch %d ", i); fflush(stdout);}

            // Step 3.1: Sample a subset of patterns
            vector<string> corePatterns;
            samplePatterns(corePatterns, topPatternWStrength, PARAM_PATTERN_SET_SIZE_PERC);

            // Step 3.2: Score candidate entity using corePatterns and (optional) type
            unordered_map<int, double> candidateEntity2Score;
            scoreEntitySet(candidateEntity2Score, seedEids, pattern2eids, eid2Pattern2Strength,
                eid2Type2Strength, corePatterns, coreType, DEBUG_FLAG, TYPE_FLAG);

            // Step 3.3: Ranking-based ensemble, select top PARAM_TOPK_EACH_BATCH entities
            // to avoid all top k entites have already existing in list
            // IDEA_NOTE: This might serve as a stopping criterion
            int top_size = PARAM_TOPK_EACH_BATCH + seedEids.size(); 
            top_size = top_size < candidateEntity2Score.size() ? top_size : candidateEntity2Score.size();
            vector<pair<int, double> > sortedEntity2Score = selectTopKByKey(candidateEntity2Score, top_size);
            

            int count = 0;
            vector<int> extractedEids; // extracted eids in current batch
            for(auto ele:sortedEntity2Score)
            {
                if (count >= PARAM_TOPK_EACH_BATCH) {
                    break;
                }
                int entity = ele.first;
                bool alreadyExist = false;
                for(auto e:seedEids) {
                    if (e == entity) { // already exist
                        alreadyExist = true;
                        break;
                    }
                }
                if(alreadyExist) {
                    continue;
                } else {
                    count += 1; 
                    extractedEids.push_back(entity);
                    
                    if(eid2mrr.find(entity) != eid2mrr.end())
                    {
                        eid2mrr[entity] += 1.0/count;
                    }
                    else{
                        eid2mrr[entity] = 1.0/count;
                    }
                }
            }
            extractedEidSets.push_back(extractedEids);
        }
        extractedEidSets.clear();
        printf("\nensemble finished !!!\n");
        if(DEBUG_FLAG) {
            printf("Number of candidate entities after ensemble = %lu\n", eid2mrr.size());
        }

        vector<pair<int, double>> sortedEid2mrr = sortMapByKey(eid2mrr);
        int ct = 0;
        for(auto ele:sortedEid2mrr)
        {
            double strength = ele.second;
            // if(strength < PARAM_RANK_THRESHOLD) {
            if(ct >= PARAM_ENTITY_TOPK) { // each iteration will finally add in top A entites
                break;
            }
            ct += 1;
            if(DEBUG_FLAG) {
                string entity_name = eid2ename[ele.first];
                printf("Add in Entity: %s [with score = %f]\n", entity_name.c_str(), strength);
            }
            int eid = ele.first;
            seedEids.push_back(eid);
        }

        eid2mrr.clear();
        sortedEid2mrr.clear();
        if(DEBUG_FLAG) {
            printf("Number of entities in set after direct expansion = %lu\n", seedEids.size());
        }

        unordered_set<int> seedEidsSet (seedEids.begin(), seedEids.end());
        if(seedEidsSet == prev_seeds) {
            printf("!!!Termination: Due to no entity set change at Iteration %d \n", iter);
            break;
        }

        if(DEBUG_FLAG) {
            printf("----------------------------------------\n");
        }

    }
}

inline void saveSEResult(const string &outputFileName,
    const unordered_set<int> &userInputSeeds,
    const vector<int> &seedEids,
    unordered_map<int, string> &eid2ename)
{
    ofstream outFile;
    outFile.open(outputFileName);

    if(outFile.is_open()) {
        // Writing seed first
        outFile << "Seeds: ";
        for(auto seedEntityID:userInputSeeds) {
            string entity_name = eid2ename[seedEntityID];
            outFile << "(" << seedEntityID << "," << entity_name << ") ";
        }
        outFile << "\n";

        outFile << "----------------------------------------\n";

        // Writing expanded entities in order
        for(auto expandedEntityID:seedEids)
        {
            string entity_name = eid2ename[expandedEntityID];
            outFile << expandedEntityID << "\t" << entity_name << "\n";
        }
        outFile << "========================================\n";

        outFile.close();

    } else {
        cout << "Could not write into file: " << outputFileName << endl;
    }

    cout << "Finish writing results to files!!!" << endl;
}

inline void saveSEResultMulti(const string &outputFileName,
    const vector<unordered_set<int> > &userInputEntitiesMultiRound,
    const vector<vector<int> > &expandedEntitiesMultiRound,
    unordered_map<int, string> &eid2ename)
{

    if(userInputEntitiesMultiRound.size() != expandedEntitiesMultiRound.size()) {
        cout << "[ERROR] Unmatched multi-rounds number" << endl;
    }

    int ROUND_NUM = userInputEntitiesMultiRound.size();


    ofstream outFile;
    outFile.open(outputFileName);

    if(outFile.is_open()) {

        for(int i = 0; i < ROUND_NUM; i++) {
            outFile << "-----------------------------------------\n";
            outFile << "-----------------------------------------\n";
            outFile << "seeds:\n";
            for(auto seedEntityID:userInputEntitiesMultiRound[i]) {
                string entity_name = eid2ename[seedEntityID];
                outFile << seedEntityID << "\t" << entity_name << "\n";
                // outFile << "(" << seedEntityID << "," << entity_name << ") ";
            }
            // outFile << "\n";

            outFile << "-----------------------------------------\n";

            // Writing expanded entities in order
            for(auto expandedEntityID:expandedEntitiesMultiRound[i])
            {
                string entity_name = eid2ename[expandedEntityID];
                outFile << expandedEntityID << "\t" << entity_name << "\n";
            }
            // outFile << "========================================\n";
        }
        outFile.close();

    } else {
        cout << "Could not write into file: " << outputFileName << endl;
    }

    cout << "Finish writing results to files!!!" << endl;
}


// inline void saveEntityPairSimilarity(const string &outputFileName, 
//     const vector<vector<double> > &eidPairWSimilarity)
// {
//     ofstream outFile;
//     outFile.open(outputFileName);

//     if(outFile.is_open()) {
//         for(auto ele:eidPairWSimilarity) {
//             outFile << ele[0] << "\t" << ele[1] << "\t" << ele[2] << "\n";
//         }
//         outFile.close();
//     } else {
//         cout << "Could not write into file: " << outputFileName << endl;
//     }
// }

/*
inline void singleSetExpansion( const unordered_set<int> &userInputSeeds,
    vector<int> &seedEids,
    unordered_map<int, unordered_set<string> > &eid2patterns,
    unordered_map<string, unordered_set<int> > &pattern2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength,
    bool DEBUG_FLAG,
    int MAX_ITER )
{

    // Initialize the entity seed set to expand
    for(auto ele:userInputSeeds) {
        seedEids.push_back(ele);
    }

    for(int iter = 0; iter < MAX_ITER; iter++)
    {
        if(DEBUG_FLAG) {
            printf("Iteration: %d\n", iter);
        }


        // Step0: Cache current seed set and check later whether there is no entites added in
        unordered_set<int> prev_seeds (seedEids.begin(), seedEids.end() );
        if(DEBUG_FLAG) {
            printf("Number of entities currently in set = %lu\n", prev_seeds.size());
        }


        // Step1: Obtain Candidate Patterns && Pattern Diversity Filtering
        unordered_map<string, double> candidatePattern2Strength;
        for(auto ele:seedEids) {
            for(auto patternStrengthPiar:eid2Pattern2Strength[ele]) {
                string pattern = patternStrengthPiar.first;
                double strength = patternStrengthPiar.second;

                int patternDiversity = pattern2eids[pattern].size();
                if(patternDiversity < PARAM_PATTERN_DIVIERSITY_LOW or patternDiversity > PARAM_PATTERN_DIVIERSITY_HIGH) {
                    continue; // filter patterns extracting < 3 or > 30 entities
                } else { // scoring the pattern left
                    if(candidatePattern2Strength.count(pattern) > 0) {
                        candidatePattern2Strength[pattern] += strength;
                    } else {
                        candidatePattern2Strength[pattern] = strength;
                    }
                }
            }
        }
        if(DEBUG_FLAG) {
            printf("Number of patterns after diversity filtering = %lu\n", candidatePattern2Strength.size());
        }

        // Step2: Pattern Quality Ranking & Filtering
        vector<pair<string, double> > sortedPattern2Strength = sortMapByKey(candidatePattern2Strength);
        vector<string> corePatterns;
        int nOfSeedEids = seedEids.size();
        int count = 0;
        for(auto ele:sortedPattern2Strength)
        {
            if(count >= PARAM_PATTERN_TOPK) {
                break;
            }
            count += 1;

            string pattern = ele.first;
            double strength = ele.second;

            if( (strength/nOfSeedEids) > PARAM_PATTERN_QUALITY_THRES) {
                corePatterns.push_back(pattern);
            }
        }

        if(corePatterns.size() == 0)
        {
            printf("!!!Termination: Due to no new pattern introduced at iteration %d \n", iter);
            break;
        }
        if(DEBUG_FLAG) {
            printf("Number of quality patterns selected = %lu\n", corePatterns.size());
        }

        // Step3: Obtain candidate entities
        unordered_set<int> candidateEntities;
        for(auto corePattern:corePatterns)
        {
            unordered_set<int> tmp = pattern2eids[corePattern];
            candidateEntities.insert(tmp.begin(), tmp.end());
        }
        if(DEBUG_FLAG) {
            printf("Number of candidate entities = %lu\n", candidateEntities.size());
        }

        // Step4: Scoring each candidate entity []
        unordered_map<int, double> candidateEntity2Score;
        // int i = 0;
        for(auto candidate:candidateEntities) {
            double score = 0;
            for(auto seed:seedEids) {
                score += scoreEntity(candidate, seed, eid2Pattern2Strength, corePatterns);
            }
            // cout << "Fnish scoring candidate entity" << i++ << endl;
            candidateEntity2Score[candidate] = score;
        }
        if(DEBUG_FLAG) {
            printf("Finish scoring all candidate entities\n");
        }

        // Step5: Entity Quality Ranking & Expanding
        vector<pair<int, double> > sortedEntity2Score = sortMapByKey(candidateEntity2Score);
        count = 0;
        for(auto ele:sortedEntity2Score)
        {
            if (count >= PARAM_ENTITY_TOPK) {
                break;
            }
            int entity = ele.first;
            if(DEBUG_FLAG) {
                double strength = ele.second;
                printf("%f\n", strength);
            }

            bool alreadyExist = false;
            for(auto e:seedEids) {
                if (e == entity) { // already exist
                    alreadyExist = true;
                    break;
                }
            }
            if(alreadyExist) {
                continue;
            } else {
                seedEids.push_back(entity);
                count += 1;
            }
        }
        if(DEBUG_FLAG) {
            printf("Number of entities in set after direct expansion = %lu\n", seedEids.size());
        }

        // Step6: Coherence based pruning, delete several entities after first step
        int newSize = seedEids.size();
        unordered_map<int, double> entity2totalScore;
        for(auto entity1:seedEids) {
            double totalScore = 0;
            for(auto entity2:seedEids) {
                if(entity2 == entity1) {
                    continue;
                } else {
                    // TODO: Change the entity scoring later
                    totalScore += scoreEntity(entity1, entity2, eid2Pattern2Strength, corePatterns);
                }
            }
            totalScore = 0.5*totalScore/(newSize-1) + 0.5*candidateEntity2Score[entity1];
            entity2totalScore[entity1] = totalScore;
        }

        vector<double> tmp;
        for(auto ele:entity2totalScore) { tmp.push_back(ele.second);}
        pair<double, double> stats = calculateMeanAndStddev(tmp);
        double avesim = stats.first;
        double st = stats.second;

        vector<int> seedEidsTmp;
        for(auto entity:seedEids) { // Filter entities based on coherence
            if (entity2totalScore[entity] >= (avesim - st*PARAM_ENTITY_ZSCORE_THRES) ) {
                seedEidsTmp.push_back(entity);
            }
        }
        for(auto seedEntity:userInputSeeds) { // Add in seeds back
            bool alreadyExist = false;
            for(auto ele:seedEidsTmp) {
                if(ele == seedEntity) {
                    alreadyExist = true;
                    break;
                }
            }
            if(not alreadyExist) {
                seedEidsTmp.push_back(seedEntity);
            }
        }

        seedEids.clear();
        for(auto ele:seedEidsTmp) {
            seedEids.push_back(ele);
        }
        if(DEBUG_FLAG) {
            printf("Number of entities in set after coherence pruning = %lu\n", seedEids.size());
        }


        unordered_set<int> seedEidsSet (seedEids.begin(), seedEids.end());
        if(seedEidsSet == prev_seeds) {
            printf("!!!Termination: Due to no entity set change at Iteration %d \n", iter);
            break;
        }

        if(DEBUG_FLAG) {
            printf("----------------------------------------\n");
        }

    }
}

inline void singleSetExpansionWrapper( const unordered_set<int> &userInputSeeds,
    vector<int> &seedEids,
    unordered_map<int, unordered_set<string> > &eid2patterns,
    unordered_map<string, unordered_set<int> > &pattern2eids,
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength,
    bool DEBUG_FLAG,
    bool SEED_ENSEMBLE_FLAG
    )
{
    if(not SEED_ENSEMBLE_FLAG)  // no seed bagging
    {
        cout << "No Seed Ensemble!!!" << endl;
        singleSetExpansion(userInputSeeds, seedEids, eid2patterns, pattern2eids,
            eid2Pattern2Strength, DEBUG_FLAG, PARAM_MAX_ITER);
    }
    else  // with seed bagging
    {
        cout << "Seed Ensemble!!!" << endl;

        // Store each branches results
        vector<vector<int> > resultPools;

        // Step1: One iteration expansion from reall user input seeds
        vector<int> firstRoundExpandedEids;
        singleSetExpansion(userInputSeeds, firstRoundExpandedEids, eid2patterns, pattern2eids,
            eid2Pattern2Strength, DEBUG_FLAG, 1);

        unordered_set<int> newlyAddedEids; // eids expanded in first iteration
        for(auto ele:firstRoundExpandedEids) {
            if(userInputSeeds.count(ele) > 0) {
                continue;
            } else {
                newlyAddedEids.insert(ele);
            }
        }
        printf("Number of newly added eids = %lu \n", newlyAddedEids.size());

        // Step2.1: Start sampling 1 eid from original seed and 2 eids from newly expanded
        int PARAM_BRANCH = 12; // 12 branches;
        int PARAM_SELECTED_THRES = 3; // include one entitiy if it matches over 3 branch
        srand (123456789);
        unordered_set<int> userInputSeedTmp;
        vector<int> seedEidsTmp;
        for(int i = 0; i < PARAM_BRANCH; ++i)
        {
            userInputSeedTmp.insert(randomSelectOneInt(userInputSeeds));
            int a = randomSelectOneInt(newlyAddedEids);
            int b = randomSelectOneInt(newlyAddedEids);
            while (a == b) {
                b = randomSelectOneInt(newlyAddedEids);
            }
            userInputSeedTmp.insert(a);
            userInputSeedTmp.insert(b);

            cout << "Newly constructed \"seed\": ";
            for(auto ele:userInputSeedTmp) {
                cout << ele << " ";
            }
            cout << "\n";

            singleSetExpansion(userInputSeedTmp, seedEidsTmp, eid2patterns, pattern2eids,
                    eid2Pattern2Strength, DEBUG_FLAG, PARAM_MAX_ITER);
            resultPools.push_back(seedEidsTmp);

            userInputSeedTmp.clear();
            seedEidsTmp.clear();
        }

        // Step2.2: Add in results from original seed expansion
        singleSetExpansion(userInputSeeds, seedEidsTmp, eid2patterns, pattern2eids,
                    eid2Pattern2Strength, DEBUG_FLAG, PARAM_MAX_ITER);
        resultPools.push_back(seedEidsTmp);


        // Step3: Ensemble resultPools
        cout << "Number of branches = " << resultPools.size() << endl;
        unordered_map<int, double> eid2finalScore;
        // int selectedSize = resultPools.back().size();
        for(auto ele:resultPools) {
            for(int i = 0; i < ele.size(); i++) {
                int eid = ele[i];
                double score = 1;
                // if (i == 0) {
                //     score = 1;
                // } else {
                //     score = 1 / log( i+1 );
                // }

                if(eid2finalScore.count(eid) > 0) {
                    eid2finalScore[eid] += score;
                } else {
                    eid2finalScore[eid] = score;
                }
            }
        }



        vector<pair<int, double> > sortedEid2finalScore = sortMapByKey(eid2finalScore);
        for(auto ele:sortedEid2finalScore) { // majority vote
            int eid = ele.first;
            double score = ele.second;
            if(score >= PARAM_SELECTED_THRES) {
                seedEids.push_back(eid);
            }
        }
    }
}
*/

#endif
