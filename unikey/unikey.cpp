#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include <X11/XKBlib.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <unistd.h>

using namespace std;

string buffer;

unordered_map<string, string> telexMap = {
    {"aw", "ă"}, {"aa", "â"}, {"dd", "đ"},
    {"ee", "ê"}, {"oo", "ô"}, {"ow", "ơ"},
    {"uw", "ư"}
};

int main() {}