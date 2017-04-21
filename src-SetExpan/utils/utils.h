#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <cctype>
#include <cmath>
#include <ctime>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

inline void printTest() {
    printf("%s\n", "Hello world!");
}

inline vector<string> splitBy(const string &s, char delim) 
{
    vector<string> result;
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

template<class T>
inline bool compBySecondElementDesc(const pair<T, double> &a, const pair<T, double> &b ) 
{
    return a.second > b.second;
}

template<class T>
inline vector<pair<T, double> > sortMapByKey(const unordered_map<T, double> &a )
{
    vector<pair<T, double> > result(a.begin(), a.end());
    sort(result.begin(), result.end(), compBySecondElementDesc<T>);
    return result;
}

template<class T>
inline vector<pair<T, double> > selectTopKByKey(const unordered_map<T, double> &a, const int topK)
{
    vector<pair<T, double> > tmp(a.begin(), a.end());
    partial_sort(tmp.begin(), tmp.begin()+topK, tmp.end(), compBySecondElementDesc<T>);
    vector<pair<T, double> > result(tmp.begin(), tmp.begin()+topK);
    return result;
}

inline int randomSelectOneInt(const unordered_set<int> &s)
{
    return 0;
    // int r = rand() % s.size();
    // unordered_set<int>::iterator it(s.begin());
    // advance(it, r);
    // return *it;
}

inline pair<double, double> calculateMeanAndStddev(const vector<double> &v) {
    double sum = 0;
    for(auto ele:v) {sum += ele; }
    double m = sum / v.size();

    double accum = 0;
    for_each (v.begin(), v.end(), [&](const double d) {
        accum += ( d - m ) * ( d - m );
    });

    double stddev = sqrt(accum / v.size() );

    pair<double, double> result(m, stddev);
    return result;
};

inline vector<vector<int> > readInQueries(const string &inputFileName)
{
    vector<vector<int> > queries;
    ifstream inFile;

    inFile.open(inputFileName);

    if(inFile.is_open()) {
        string line;
        char delim = ' ';
        while(inFile)
        {
            getline(inFile, line);
            if(line.size() > 0 and line != "EXIT") 
            {
                vector<string> segs = splitBy(line, delim);
                vector<int> query;
                for(auto ele:segs) {
                    // printf("%s\n", ele.c_str());
                    query.push_back(stoi(ele));
                }
                queries.push_back(query);
            }
            else {
                break;
            }
        }
        inFile.close();
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }
    return queries;
}

inline vector<int> readSeedPool(const string &inputFileName)
{
    vector<int> seedPool;

    ifstream inFile;

    inFile.open(inputFileName);

    if(inFile.is_open()) {
        string line;
        int eid;
        string ename;
        char delim = '\t';
        while(inFile) 
        {
            getline(inFile, line);
            if(line.size() > 0) 
            {
                vector<string> segs = splitBy(line, delim);
                eid = stoi(segs[0]);
                seedPool.push_back(eid);
            }
        }
        inFile.close();
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }
    return seedPool;
}

inline vector<int> readSeedPool2(const string &inputFileName, unordered_map<string, int> &ename2eid)
{
    vector<int> seedPool;

    ifstream inFile;

    inFile.open(inputFileName);

    if(inFile.is_open()) {
        string line;
        string prev_line = "";
        int eid;
        string ename;
        // char delim = '\t';
        while(inFile) 
        {
            getline(inFile, line);
            if (line == prev_line) {break;}
            if(line.size() > 0) 
            {
                // cout << line << endl;
                ename = line;
                if(ename2eid.count(ename) > 0 ) {
                    eid = ename2eid[ename];
                } else {
                    cout << "Unable to find entity " << ename << endl;
                }
                seedPool.push_back(eid);
                prev_line = line;
            }
        }
        inFile.close();
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }
    return seedPool;
}


inline unordered_map<int, string> readEntityMap(const string &inputFileName)
{
    unordered_map<int, string> eid2ename;
    
    ifstream inFile;

    inFile.open(inputFileName);

    if(inFile.is_open()) {
        string line;
        int eid;
        string ename;
        char delim = '\t';
        while(inFile) 
        {
            getline(inFile, line);
            if(line.size() > 0) 
            {
                vector<string> segs = splitBy(line, delim);
                ename = segs[0];
                eid = stoi(segs[1]);

                eid2ename[eid] = ename;
            }
        }
        inFile.close();
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }
    return eid2ename;
}

inline unordered_map<string, int> readEntityMap2(const string &inputFileName)
{
    unordered_map<string, int> ename2eid;

    ifstream inFile;

    inFile.open(inputFileName);

    if(inFile.is_open()) {
        string line;
        int eid;
        string ename;
        char delim = '\t';
        while(inFile) 
        {
            getline(inFile, line);
            if(line.size() > 0) 
            {
                vector<string> segs = splitBy(line, delim);
                ename = segs[0];
                eid = stoi(segs[1]);

                ename2eid[ename] = eid;
            }
        }
        inFile.close();
    } else {
        cout << "Cannot open file: " << inputFileName << endl;
    }
    return ename2eid;
}


inline void saveResult(const string &outputFileName, vector<unordered_set<int> > &seed_entities, 
    const vector<vector<int> > &meb_entities, unordered_map<int, string> &eid2ename)
{
    ofstream outFile;
    outFile.open(outputFileName);

    if(outFile.is_open()) {
        // Writing seed first
        for(int i = 0; i < seed_entities.size(); i++) 
        {
            outFile << "Class " << i << " seeds: ";
            for(auto entity_id:seed_entities[i]) 
            {
                string entity_name = eid2ename[entity_id];
                outFile << "(" << entity_id << "," << entity_name << ") "; 
            }
            outFile << "\n";
        }

        outFile << "========================================\n";
        
        // Writing expanded entities in order
        for(int i = 0; i < meb_entities.size(); i++) 
        {
            outFile << "Class " << i << " entities: " << "\n";
            for(int j = 0; j < meb_entities[i].size(); j++) 
            {
                int entity_id = meb_entities[i][j];
                string entity_name = eid2ename[entity_id];
                outFile << entity_id << "\t" << entity_name << "\n"; 
            }
            outFile << "----------------------------------------\n";
        }
        outFile.close();

    } else {
        cout << "Could not write into file: " << outputFileName << endl;
    }

    cout << "Finish writing results to files!!!" << endl;
}








// #define FOR(i,a) for (__typeof((a).begin()) i = (a).begin(); i != (a).end(); ++ i)

// #define PATTERN_CHUNK_SIZE 10
// #define SENTENCE_CHUNK_SIZE 5
// #define POINT_CHUNK_SIZE 5

// const int SUFFIX_MASK = (1 << 20) - 1; // should be 2^k - 1
// mutex separateMutex[SUFFIX_MASK + 1];

// const double EPS = 1e-8;

// /*! \brief return a real numer uniform in (0,1) */
// inline double next_double2(){
//     return (static_cast<double>( rand() ) + 1.0 ) / (static_cast<double>(RAND_MAX) + 2.0);
// }

// /*! \brief return  x~N(0,1) */
// inline double sample_normal(){
// 	double x,y,s;
// 	do{
// 		x = 2 * next_double2() - 1.0;
// 		y = 2 * next_double2() - 1.0;
// 		s = x*x + y*y;
// 	}while( s >= 1.0 || s == 0.0 );

// 	return x * sqrt( -2.0 * log(s) / s ) ;
// }

// inline bool myAssert(bool flg, string msg)
// {
// 	if (!flg) {
// 		cerr << msg << endl;
// 		exit(-1);
// 	}
// 	return flg;
// }

// inline int sign(double x)
// {
// 	return x < -EPS ? -1 : x > EPS;
// }

// inline string replaceAll(const string &s, const string &from, const string &to)
// {
//     string ret = "";
//     for (size_t i = 0; i < s.size(); ++ i) {
//         bool found = true;
//         for (size_t offset = 0; offset < from.size() && found; ++ offset) {
//             found &= i + offset < s.size() && s[i + offset] == from[offset];
//         }
//         if (found) {
//             ret += to;
//             i += from.size() - 1;
//         } else {
//             ret += s[i];
//         }
//     }
//     return ret;
// }

// inline double sqr(double x)
// {
//     return x * x;
// }

// template<class T>
// inline void fromString(const string &s, T &x)
// {
// 	stringstream in(s);
// 	in >> x;
// }

// inline string tolower(const string &a)
// {
// 	string ret = a;
// 	for (size_t i = 0; i < ret.size(); ++ i) {
// 		ret[i] = tolower(ret[i]);
// 	}
// 	return ret;
// }

// const int MAX_LENGTH = 100000000;

// char line[MAX_LENGTH + 1];

// inline bool getLine(FILE* in)
// {
// 	bool hasNext = fgets(line, MAX_LENGTH, in);
// 	int length = strlen(line);
// 	while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
// 		-- length;
// 	}
// 	line[length] = 0;
// 	return hasNext;
// }

// inline FILE* tryOpen(const string &filename, const string &param)
// {
// 	FILE* ret = fopen(filename.c_str(), param.c_str());
// 	if (ret == NULL) {
// 		cerr << "[Warning] failed to open " << filename  << " under parameters = " << param << endl;
// 	}
// 	return ret;
// }


// inline vector<string> splitBy(const string &line, char sep)
// {
// 	vector<string> tokens;
// 	string token = "";
// 	for (size_t i = 0; i < line.size(); ++ i) {
// 		if (line[i] == sep) {
// 		    if (token != "") {
//     			tokens.push_back(token);
// 			}
// 			token = "";
// 		} else {
// 			token += line[i];
// 		}
// 	}
// 	if (token != "") {
//     	tokens.push_back(token);
// 	}
// 	return tokens;
// }

// namespace Binary
// {
//     template<class T>
//     inline void write(FILE* out, const T& x) {
// 		fwrite(&x, sizeof(x), 1, out);
// 	}

//     template<class T>
// 	inline void read(FILE* in, T &size) {
// 		fread(&size, sizeof(size), 1, in);
// 	}

//     inline void write(FILE* out, const string &s) {
// 		write(out, s.size());
// 		if (s.size() > 0) {
// 			fwrite(&s[0], sizeof(char), s.size(), out);
// 		}
// 	}

// 	inline void read(FILE* in, string &s) {
// 		size_t size;
// 		read(in, size);
// 		s.resize(size);
// 		if (size > 0) {
// 			fread(&s[0], sizeof(char), size, in);
// 		}
// 	}
// }

#endif
