#ifndef __MULTISE_H__
#define __MULTISE_H__

#include "utils/utils.h"
#include "utils/parameters.h"

inline void printTestMultiSE() {
    cout << "This is Multiset expansion" << endl;
}

inline void multiSetExpansionWithType( const vector<unordered_set<int> > &userInputSeedsMulti, 
    vector<vector<int> > &expandedEntitiesMulti, 
    unordered_map<int, unordered_set<string> > &eid2patterns, 
    unordered_map<string, unordered_set<int> > &pattern2eids, 
    unordered_map<int, unordered_map<string, double> > &eid2Pattern2Strength,

    unordered_map<int, unordered_set<string> > &eid2types, 
    unordered_map<string, unordered_set<int> > &type2eids, 
    unordered_map<int, unordered_map<string, double> > &eid2Type2Strength,

    bool FLAG_DEBUG,
    int MAX_ITER) 
{

    // Initialize the entity seed set for each semantic class
    int NUM_OF_CLASS = userInputSeedsMulti.size();
    printf("Number of semantic class = %d\n", NUM_OF_CLASS);

    // Whether a class is stoped for expanding or not
    vector<bool> stoped_flag (false, NUM_OF_CLASS);

    // Initializet the entity set to expand
    for(auto ele:userInputSeedsMulti) {
        expandedEntitiesMulti.push_back(vector<int> (ele.begin(), ele.end()) );
    }

    for(int iter = 0; iter < MAX_ITER; iter++)
    {
        if(FLAG_DEBUG) {printf("Iteration: %d\n", iter);}
        bool stop_all = true;
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            if(not stoped_flag[i]) {
                stop_all = false;
                break;
            }
        }
        if(stop_all) {
            printf("!!!Terminate at Iteration-%d\n", iter);
            break;
        }

        // Step 0: Cache current seed set and check later for stopping criterion
        vector<unordered_set<int> > pre_seeds_multi;
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            unordered_set<int> pre_seeds (expandedEntitiesMulti[i].begin(), expandedEntitiesMulti[i].end());
            pre_seeds_multi.push_back(pre_seeds);
        }
        if(FLAG_DEBUG) {
            for(int i = 0; i < NUM_OF_CLASS; i++) {
                printf("Number of entities currently in class-%d = %lu\n", i, pre_seeds_multi[i].size());    
            }
        }

        // Step 1: Pattern quality estimation
        // TODO: Some other way to deal with negative score
        vector<unordered_map<string, double> > candidatePattern2ScoreMult;
        for(int i = 0; i < NUM_OF_CLASS; i++) { // for each class, first find the positive score of each pattern in it
            unordered_map<string, double> candidatePattern2Score;

            for(auto eid:expandedEntitiesMulti[i]) { // for each entity
                for(auto patternStrengthPair:eid2Pattern2Strength[eid]) { // for each pattern
                    string pattern = patternStrengthPair.first;
                    double strength = patternStrengthPair.second;
                    int diversity = pattern2eids[pattern].size();

                    if(diversity < PARAM_PATTERN_DIVIERSITY_LOW or diversity > PARAM_PATTERN_DIVIERSITY_HIGH) {
                        continue; // filter patterns extracting < 3 or > 30 entities
                    } else { // scoring the pattern left
                        if(candidatePattern2Score.count(pattern) > 0) {
                            candidatePattern2Score[pattern] += strength;
                        } else {
                            candidatePattern2Score[pattern] = strength;
                        }
                    }
                }
            }
            candidatePattern2ScoreMult.push_back(candidatePattern2Score);
        }

        vector<unordered_map<string, double> > candidatePattern2FinalScoreMult;
        for(int i = 0; i < NUM_OF_CLASS; i++) { // for each class, then find the negative score of each pattern in it
            unordered_map<string, double> tmp;
            for(auto ele:candidatePattern2ScoreMult[i]) { 
                string pattern = ele.first;
                double strength = ele.second;

                for(int j = 0; j < NUM_OF_CLASS; j++) {
                    if(j == i) {
                        continue;
                    } else {
                        if (candidatePattern2ScoreMult[j].count(pattern) > 0) {
                            double neg_strength = candidatePattern2ScoreMult[j][pattern];
                            strength -= (PARAM_PATTERN_NEG_DISCOUNT * neg_strength);
                        }
                    }
                }

                tmp[pattern] = strength;
            }
            candidatePattern2FinalScoreMult.push_back(tmp);
        }

        if(FLAG_DEBUG) {
            for(int i = 0; i < NUM_OF_CLASS; i++) {
                printf("Number of patterns after diversity filtering in class-%d = %lu\n", i, candidatePattern2FinalScoreMult[i].size());    
            }            
        }

        // Step 2: Pattern quality ranking
        vector<vector<string> > corePatternsMulti;
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            vector<pair<string, double> > sortedPattern2Strength = sortMapByKey(candidatePattern2FinalScoreMult[i]);
            vector<string> corePatterns;
            int nOfSeedEids = expandedEntitiesMulti[i].size();
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

            if (corePatterns.size() == 0) {
                stoped_flag[i] = true;
            }

            corePatternsMulti.push_back(corePatterns);
        }
        
        if(FLAG_DEBUG) {
            for(int i = 0; i < NUM_OF_CLASS; i++) {
                printf("Number of quality patterns selected for class-%d = %lu\n", i, corePatternsMulti[i].size());    
            }
        }

        // Step 3: Type selection
        // TODO: other way to select the strongest type
        vector<unordered_map<string, pair<int, double> > >  candidateType2CountStrengthMulti;
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            unordered_map<string, pair<int, double>> candidateType2CountStrength;

            for(auto ele:expandedEntitiesMulti[i]) {
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

            candidateType2CountStrengthMulti.push_back(candidateType2CountStrength); 
        }


        vector<string> coreTypeMulti;
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            string coreType = "";
            int maxMatchCount = -1;
            double maxStrength = -1e10;
            for(auto ele:candidateType2CountStrengthMulti[i]) {
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
            coreTypeMulti.push_back(coreType);
        }

        if(FLAG_DEBUG) {
            for(int i = 0; i < NUM_OF_CLASS; i++) {
                cout << "Strongest type for class " << i << " : " << coreTypeMulti[i] << endl;    
            }
        }


        // Step 4: Obtain candidate entities
        vector<unordered_set<int> > candidateEntitiesMulti;
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            unordered_set<int> candidateEntities;
            for(auto corePattern:corePatternsMulti[i]) 
            {
                unordered_set<int> tmp = pattern2eids[corePattern];
                candidateEntities.insert(tmp.begin(), tmp.end());
            }
            candidateEntitiesMulti.push_back(candidateEntities);
        }

        if(FLAG_DEBUG) {
            for(int i = 0; i < NUM_OF_CLASS; i++) {
                printf("Number of candidate entities for class-%d = %lu\n", i, candidateEntitiesMulti[i].size());
            }
        }        


        // Step 5: Scoring each candidate entity
        // TODO: a better way for candidate entity scoring
        vector<unordered_map<int, double> > candidateEntity2ScoreMultiTmp;
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            unordered_map<int, double> candidateEntity2ScoreTmp;
            string coreType = coreTypeMulti[i];

            for(auto candidate:candidateEntitiesMulti[i]) {
                double score = 0;
                // TODO: other way to combine type
                if( eid2Type2Strength[candidate].count(coreType) > 0) {
                    score += ( PARAM_TYPE_WEIGHT * eid2Type2Strength[candidate][coreType] );
                } else {
                    score += NEG_INF; // filter those entity that do not match the coreType
                }

                for(auto seed:expandedEntitiesMulti[i]) {
                    // scoreEntity is defined in singleSE.h
                    score += scoreEntity(candidate, seed, eid2Pattern2Strength, corePatternsMulti[i]);
                }
                // cout << "Fnish scoring candidate entity" << i++ << endl;
                candidateEntity2ScoreTmp[candidate] = score;
            }
            candidateEntity2ScoreMultiTmp.push_back(candidateEntity2ScoreTmp);
        }

        vector<unordered_map<int, double> > candidateEntity2ScoreMulti; 
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            unordered_map<int, double> tmp;
            for(auto ele:candidateEntity2ScoreMultiTmp[i]) {
                int eid = ele.first;
                double strength = ele.second;

                for(int j = 0; j < NUM_OF_CLASS; j++) {
                    if(j == i) {
                        continue;
                    } else { // minus the negative score if that entity apper in other classes
                        if(candidateEntity2ScoreMultiTmp[j].count(eid) > 0) {
                            double neg_strength = candidateEntity2ScoreMultiTmp[j][eid];
                            strength -= (PARAM_ENTITY_NEG_DISCOUNT * neg_strength);
                        }
                    }
                }

                tmp[eid] = strength;
            }
            candidateEntity2ScoreMulti.push_back(tmp);
        }

        if(FLAG_DEBUG) {
            printf("Finish scoring all candidate entities\n");
        }

        // Step 6: Entity Quality Ranking & Expanding
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            vector<pair<int, double> > sortedEntity2Score = sortMapByKey(candidateEntity2ScoreMulti[i]);
            int count = 0;
            for(auto ele:sortedEntity2Score)
            {
                if (count >= PARAM_ENTITY_TOPK) {
                    break;
                }
                int entity = ele.first;
                // if(FLAG_DEBUG) {
                //     double strength = ele.second;
                //     printf("%f\n", strength);
                // }
                
                bool alreadyExist = false;
                for(auto e:expandedEntitiesMulti[i]) {
                    if (e == entity) { // already exist
                        alreadyExist = true;
                        break;
                    }
                }
                if(alreadyExist) {
                    continue;
                } else {
                    expandedEntitiesMulti[i].push_back(entity);
                    count += 1;
                }
            }            
        }

        if(FLAG_DEBUG) {
            for(int i = 0; i < NUM_OF_CLASS; i++) {
                printf("Number of entities in set after direct expansion for class-%d= %lu\n", i, expandedEntitiesMulti[i].size());
            }
        }

        // Step 7: Coherence based pruning
        for(int i = 0; i < NUM_OF_CLASS; i++) {
            int newSize = expandedEntitiesMulti[i].size();


            unordered_map<int, double> entity2totalScore;
            for(auto entity1:expandedEntitiesMulti[i]) {
                double totalScore = 0;
                for(auto entity2:expandedEntitiesMulti[i]) {
                    if(entity2 == entity1) {
                        continue;
                    } else {
                        // TODO: Change the entity scoring later
                        totalScore += scoreEntity(entity1, entity2, eid2Pattern2Strength, corePatternsMulti[i]);
                    }
                }
                totalScore = 0.5*totalScore/(newSize-1) + 0.5*candidateEntity2ScoreMulti[i][entity1];
                entity2totalScore[entity1] = totalScore;
            }

            vector<double> tmp;
            for(auto ele:entity2totalScore) { tmp.push_back(ele.second);}
            pair<double, double> stats = calculateMeanAndStddev(tmp);
            double avesim = stats.first;
            double st = stats.second;

            vector<int> seedEidsTmp;
            for(auto entity:expandedEntitiesMulti[i]) { // Filter entities based on coherence
                if (entity2totalScore[entity] >= (avesim - st*PARAM_ENTITY_ZSCORE_THRES) ) {
                    seedEidsTmp.push_back(entity);
                }
            }
            for(auto seedEntity:userInputSeedsMulti[i]) { // Add in seeds back
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

            expandedEntitiesMulti[i].clear();
            for(auto ele:seedEidsTmp) {
                expandedEntitiesMulti[i].push_back(ele);
            }

            if(FLAG_DEBUG) {
                printf("Number of entities in set after coherence pruning for class-%d = %lu\n", i, expandedEntitiesMulti[i].size());
            }


            unordered_set<int> seedEidsSet (expandedEntitiesMulti[i].begin(), expandedEntitiesMulti[i].end());
            if(seedEidsSet == pre_seeds_multi[i]) {
                stoped_flag[i] = true;
            }

        }

        if(FLAG_DEBUG) {
            for(int i = 0; i < NUM_OF_CLASS; i++) {
                cout << "Class-" << i << ": ";
                for(auto ele:expandedEntitiesMulti[i]) {
                    cout << ele << " ";
                }
                cout << endl;
            }
            printf("----------------------------------------\n");
        }
    }
}




#endif