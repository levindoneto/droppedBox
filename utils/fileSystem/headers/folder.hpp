#pragma once

#include <string>
#include <string.h>
#include <vector>

using namespace std;

string getHome();
vector<string> getTimes(string filePath);
string timesToString(vector<string> times, int mode);
void listFolder(string folderPath);
string listFiles(int mode, string userId);
