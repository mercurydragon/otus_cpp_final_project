#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include "constants.h"
#include "fixew_priority_queue.h"
#include "IndexIO.h"

using namespace std;

using idfMap = unordered_map<string, double>;
using termFreqMap = unordered_map<string, int>;

struct DocumentFreqData {
    DocumentFreqData() {
        url = "";
        len = 0;
        termFreqs = unordered_map<string, int>();
    }

    DocumentFreqData(string url, unsigned int len, termFreqMap termFreqs) : url(std::move(url)),
                                                                            len(len),
                                                                            termFreqs(std::move(
                                                                                    termFreqs)) {}

    string url;
    unsigned int len;
    termFreqMap termFreqs;
};