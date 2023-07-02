//
// Created by galya on 30.06.23.
//

#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include "constants.h"
#include "fixew_priority_queue.h"
#include "IndexIO.h"
#include "IndexCommon.h"

using namespace std;


class BM25 {
public:

    BM25(double epsilon, double k1, IIndexIO *indexIO);

    void createIndex(const string &folderPath);

    void loadIndex();

    FixedPriorityQueue<string, TOP_COUNT> searchDocuments(const string &query);
private:
    double epsilon = 0.25;
    double K1 = 1.5;
    double avgdl = 0;

    IIndexIO *indexIO;

    idfMap idfs;
    vector<DocumentFreqData> termInDocFreqs;

    void indexDocuments(const string &folderPath);

    static vector<string> tokenize(const string &text);

    void calcIdf(const termFreqMap &wordFreqs, int corpusSize) ;
};

