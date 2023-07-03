#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>


using namespace std;
using tokenizer = boost::tokenizer<boost::char_separator<char>>;

#include "BM25.h"
#include "FileReader.h"


void BM25::calcIdf(const termFreqMap &wordFreqs, int corpusSize) {
    // collect idf sum to calculate an average idf for epsilon value
    double idfSum = 0;

    // collect words with negative idf to set them a special epsilon value.
    // idf can be negative if word is contained in more than half of documents
    vector<string> negative_idfs;
    for (const auto &wordFreq: wordFreqs) {
        double idf = log(corpusSize - wordFreq.second + 0.5) - log(wordFreq.second + 0.5);
        idfs[wordFreq.first] = idf;
        idfSum += idf;
        if (idf < 0)
            negative_idfs.push_back(wordFreq.first);
    }
    auto averageIdf = idfSum / (double)idfs.size();

    auto eps = this->epsilon * averageIdf;
    for (const auto &word: negative_idfs) {
        idfs[word] = eps;
    }
}

void BM25::createIndex(const string &folderPath) {
    indexDocuments(folderPath);
    indexIO->saveIdfs(idfs);
    indexIO->saveDocFreq(termInDocFreqs, avgdl);
}


void BM25::indexDocuments(const string &folderPath) {
    unordered_map<string, double> bm25Scores;
    unordered_map<string, int> wordFreqs;

    auto documents = FileReader::readDir(folderPath);

    // Индексация документов
    int corpusSize = documents.size();
    unordered_map<string, double> avgDocLengths;
    int totalDocLength = 0;

    for (const auto &document: documents) {
        vector<string> words = tokenize(document.descr);
        unordered_map<string, int> termFreq;

        for (const string &word: words) {
            termFreq[word]++;
        }

        termInDocFreqs.emplace_back(document.url, words.size(), termFreq);

        for (const auto &pair: termFreq) {
            string term = pair.first;
            wordFreqs[term]++;
        }

        totalDocLength += words.size();
    }
    calcIdf(wordFreqs, corpusSize);

    avgdl = static_cast<double>(totalDocLength) / corpusSize;
}


vector<string> BM25::tokenize(const string &text) {

    vector<string> words;
    boost::char_separator<char> separator(" \t\n\r\f.,;:\"\'()[]{}<>!?-");
    tokenizer tokens(text, separator);

    for (const auto &token: tokens) {
        // сверх простая попытка в лемматизацию - если слово длиное отрезаем последние 2 буквы - предполагаем что это окончание
//        auto new_token = token;
//        if (token.size() > 4)
//            new_token = token.substr(0, token.size()-2);
        words.push_back(token);
    }

    return words;
}

FixedPriorityQueue<string, TOP_COUNT> BM25::searchDocuments(const string &query) {
    // считаем скоры, кладе в очередь с приоритетом вощвращаем топчик
    vector<string> queryTerms = tokenize(query);
    unordered_map<string, double> documentScores;
    FixedPriorityQueue<string, TOP_COUNT> topResult;
    for (const auto &docData: termInDocFreqs) {
        auto termDocFreq = docData.termFreqs;
        double score = 0;
        for (const string &term: queryTerms) {
            double qFreq = 0;
            if (auto search = termDocFreq.find(term); search != termDocFreq.end())
                qFreq = search->second;

            double idf = 0;
            if (auto search = idfs.find(term); search != idfs.end())
                idf = search->second;
            score += idf * (qFreq * (K1 + 1) / (qFreq + K1 * (1 - B + B * docData.len / avgdl)));
        }

        topResult.push(pair(docData.url, score));
    }


    return topResult;
}

BM25::BM25(double epsilon, double k1, IIndexIO *indexIO) : epsilon(epsilon), K1(k1), indexIO(indexIO) {}

void BM25::loadIndex() {
    idfs = indexIO->readIdf();
    std::tie(termInDocFreqs, avgdl) = indexIO->readTermInDocFreq();
}
