#include "Header/GarbageCollector.h"

namespace fs = std::experimental::filesystem;

GarbageCollector::GarbageCollector(vector<string> files, int threshold) {
    GarbageCollector::TempFiles = files;
    GarbageCollector::FileThreshold = threshold;
    
    string path = "temp";
    for (const auto & entry : fs::directory_iterator(path)) {
        TempFiles.push_back(entry.path());
    }
    
    while(TempFiles.size() > FileThreshold) {
		vector<string>::iterator it = TempFiles.begin(); 
		if(remove(TempFiles[0].c_str()) != 0) {
            cout << "GarbageCollector: Error deleting TEMP-file" << endl;
		}
        else {
            cout << "GarbageCollector: Deleted TEMP-file" << endl;
            TempFiles.erase(it);
		}
	}
}

void GarbageCollector::addFile(string path) {
    TempFiles.push_back(path);

    vector<string>::iterator it = TempFiles.begin(); 

    if(TempFiles.size() > FileThreshold) {
        if(remove(TempFiles[0].c_str()) != 0) {
            cout << "GarbageCollector: Error deleting TEMP-file" << endl;
		}
        else {
            cout << "GarbageCollector: Deleted TEMP-file" << endl;
            TempFiles.erase(it);
		}
    }
}


