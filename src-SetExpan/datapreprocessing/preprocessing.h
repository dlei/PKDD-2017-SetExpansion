#ifndef __PREPROCESSING_H__
#define __PREPROCESSING_H__


#include "../utils/utils.h"
#include "../utils/parameters.h"


// inline void preprocessingTest() {
//     printf("%s\n", "Hello preprocessingTest");
// }

inline void readEntityPatternRawCount(const string &inputFileName, 
    unordered_map<int, vector<pair<string, double> > > &eid2patternWstrength,
    unordered_map<string, vector<pair<int, double> > > &pattern2eidWstrength )
{
    ifstream inFile;
    inFile.open(inputFileName);
    if(inFile.is_open()) {
        string line;
        int eid;
        string pattern;
        double strength;
        char delim = '\t';
        while(inFile) {
            getline(inFile, line);
            if(line.size() > 0) {
                vector<string> segs = splitBy(line, delim);
                eid = stoi(segs[0]);
                pattern = segs[1];
                strength = stod(segs[2]);

                // eid -> patterns
                if(eid2patternWstrength.count(eid) > 0) {
                    eid2patternWstrength[eid].push_back(pair<string, double>(pattern, strength) );
                } else {
                    vector<pair<string, double> > tmp;
                    tmp.push_back(pair<string, double>(pattern, strength) );
                    eid2patternWstrength[eid] = tmp;
                }

                // patterns -> eid
                if(pattern2eidWstrength.count(pattern) > 0) {
                    pattern2eidWstrength[pattern].push_back(pair<int, double>(eid, strength) );
                } else {
                    vector<pair<int, double> > tmp;
                    tmp.push_back(pair<int, double>(eid, strength) );
                    pattern2eidWstrength[pattern] = tmp;
                }
            }
        }
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }

    inFile.close();
    printf("%s\n", "Finish loading files");
    printf("Number of entities = %lu\n", eid2patternWstrength.size());
    printf("Number of patterns = %lu\n", pattern2eidWstrength.size());
}

inline void calculateEntityPatternStrength(
    unordered_map<int, vector<pair<string, double> > > &eid2patternWcount, 
    unordered_map<string, vector<pair<int, double> > > &pattern2eidWcount, 
    string &similarityName,
    unordered_map<int, vector<pair<string, double> > > &eid2patternWstrength, 
    unordered_map<string, vector<pair<int, double> > > &pattern2eidWstrength) 
{
    // Step1: select frequent entity & patterns
    unordered_set<int> freqEntitySet;
    unordered_set<string> freqPatternSet;

    for (auto it = eid2patternWcount.begin(); it != eid2patternWcount.end(); it++) {
        int entity_id = it->first;
        int freq = 0;
        for (auto ele: it->second ) {
            freq += ele.second;
        }
        if (freq >= PARAM_ENITYT_MIN_SUP) { // Filter low-frequency entities
            freqEntitySet.insert(entity_id);
        }
    }

    for (auto it = pattern2eidWcount.begin(); it != pattern2eidWcount.end(); it++) {
        string pattern = it->first;
        
        int diversity = it->second.size();
        if (diversity >= PARAM_PATTERN_MIN_DIVERSITY) {
            int freq = 0;
            for( auto ele: it->second) { // Filter those patterns extracting only one entities
                freq += ele.second;
            }
            if (freq >= PARAM_PATTERN_MIN_SUP) { // Filter low-frequency patterns
                freqPatternSet.insert(pattern);
            }  
        }
    }

    printf("%s\n", "Finish filtering low-frequency entities & patterns");
    printf("Number of left entities = %lu\n", freqEntitySet.size());
    printf("Number of left patterns = %lu\n", freqPatternSet.size());

    // Step2: obtain entity and pattern occurrence within
    unordered_map<int, int> entity2count;
    unordered_map<string, int> pattern2count;

    for (auto eid: freqEntitySet) {
        int freq = 0;
        for(auto patternPair: eid2patternWcount[eid] ) {
            string pattern = patternPair.first;
            int count = patternPair.second;
            if(freqPatternSet.count(pattern) > 0 ) {
                freq += count;
            }
        }
        if (freq > 0) {
            entity2count[eid] = freq;
        }
    }

    for (auto pattern: freqPatternSet) {
        int freq = 0;
        for(auto entityPair: pattern2eidWcount[pattern] ) {
            int entity = entityPair.first;
            int count = entityPair.second;
            if(freqEntitySet.count(entity) > 0 ) {
                freq += count;
            }
        }
        if (freq > 0) {
            pattern2count[pattern] = freq;
        }
    }

    printf("%s\n", "Finish filtering isolated entities & patterns");
    printf("Number of left entities = %lu\n", entity2count.size());
    printf("Number of left patterns = %lu\n", pattern2count.size());


    // Step3: Caluate entity-pattern similarity
    if(similarityName == "PMI_square") {
        printf("%s: %s\n", "Start calculate entity-pattern strength", similarityName.c_str());
        for(auto ele: entity2count) { // for each entity, get its important patterns
            int eid = ele.first;
            int eidcount = ele.second;

            vector<pair<string, double> > tmpPatternWstrength;
            for(auto patternPair: eid2patternWcount[eid]) {
                string pattern = patternPair.first;
                double co_occurrence = patternPair.second;
                if (pattern2count.count(pattern) > 0) { // not isolated pattern
                    int patterncount = pattern2count[pattern];
                    double strength = log2( co_occurrence * co_occurrence / (eidcount * patterncount)  );
                    tmpPatternWstrength.push_back(make_pair(pattern, strength));
                }
            }

            eid2patternWstrength[eid] = tmpPatternWstrength;
        }

        for(auto ele: pattern2count) {
            string pattern = ele.first;
            int patterncount = ele.second;

            vector<pair<int, double> > tmpEntityWstrength;
            for(auto entityPair: pattern2eidWcount[pattern]) {
                int entity = entityPair.first;
                double co_occurrence = entityPair.second;
                if (entity2count.count(entity) > 0) {
                    int eidcount = entity2count[entity];
                    double strength = log2( co_occurrence * co_occurrence / (eidcount * patterncount)  );
                    tmpEntityWstrength.push_back(make_pair(entity, strength));
                }
            }

            pattern2eidWstrength[pattern] = tmpEntityWstrength;
        }

    }

    printf("%s\n", "Finish calculate entity-pattern strength");
    // printf("Number of left entities = %lu\n", entity2count.size());
    // printf("Number of left patterns = %lu\n", pattern2count.size());
}

inline void saveIntermediateFile(const string &outputFileName, 
    unordered_map<int, vector<pair<string, double> > > &eid2patternWstrength )
{
    ofstream outFile;
    outFile.open(outputFileName);

    if(outFile.is_open()) {
        for(auto ele: eid2patternWstrength) {
            int entity_id = ele.first;
            for( auto patternStrengthPair: ele.second) {
                string pattern = patternStrengthPair.first;
                double strength = patternStrengthPair.second;
                outFile << entity_id << "\t" << pattern << "\t" << strength << "\n";
            }
        }
        outFile.close();
    } else {
        cout << "Could not write into file: " << outputFileName << endl;
    }

    cout << "Finish writing intermediate results to files!!!" << endl;
}


#endif
