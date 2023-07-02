#pragma once

#include "IndexCommon.h"

class IIndexIO {
public:
    virtual void saveIdfs(const idfMap &idfs) = 0;
    virtual void saveDocFreq(const vector<DocumentFreqData> &termInDocFreqs, double avgdl) = 0;
    virtual idfMap readIdf() = 0;
    virtual tuple<vector<DocumentFreqData>, double> readTermInDocFreq() = 0;
};


class IndexIO : public IIndexIO{
public:
    IndexIO(string docTermFreqFilePath, string ifdFilePath);

    ~IndexIO() = default;

    void saveIdfs(const idfMap &idfs) override;
    void saveDocFreq(const vector<DocumentFreqData> &termInDocFreqs, double avgdl) override;
    idfMap readIdf() override;
    tuple<vector<DocumentFreqData>, double> readTermInDocFreq() override;
private:
    string docTermFreqFilePath;
    string ifdFilePath;

    static tuple<string, double> splitDocString(const string &input);
};
