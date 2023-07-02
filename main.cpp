#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include "BM25.h"


using namespace std;


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: ./bm25_util --index \"/path/to/dir\" --search \"search query\"" << endl;
        return 1;
    }

    auto indexIO = IndexIO(DOC_FREQ_RESULT_PATH, IDFS_RESULT_PATH);
    auto bm25 = BM25(EPSILON, K1, &indexIO);
    string mode = argv[1];

    if (mode == "--index") {
        string folderPath = argv[2];
        bm25.createIndex(folderPath);

        cout << "Indexing complete. Result saved to " << IDFS_RESULT_PATH << ", " << DOC_FREQ_RESULT_PATH << endl;
    } else if (mode == "--search") {
        string query = argv[2];
        bm25.loadIndex();
        cout << "Index loaded" << endl;
        auto top = bm25.searchDocuments(query);
        top.print();
    } else {
        cout << "Invalid mode specified." << endl;
        cout << "Usage: ./bm25_util --index \"/path/to/dir\" --search \"search query\"" << endl;
        return 1;
    }

    return 0;
}