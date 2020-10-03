#include "LoginHandler.h"
#include <stdio.h>
#include <experimental/filesystem>

class GarbageCollector {
	public:
	
	vector<string> TempFiles;
	
	int FileThreshold;
	
	GarbageCollector(vector<string> files, int threshold);
	
	GarbageCollector() {};
	
	void addFile(string path);
};
