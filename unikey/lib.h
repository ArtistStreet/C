#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include <X11/XKBlib.h>
#include <bits/stdc++.h>
#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

extern vector<string> buffer; // Change buffer to a single string instead of a vector<string>

extern unordered_map<string, string> telexMap;
extern unordered_map<string, string> telexMapW;
extern unordered_map<string, unordered_map<char, string>> accentMap; 
extern unordered_map<string, pair<string, char>> reverseAccentMap;
extern unordered_map<string, pair<string, char>> reverseTelexTransform;
extern const vector<string> priority;

char keycode_to_char(Display *display, unsigned int keycode);
void sendLeftArrow(Display *display);
void sendRightArrow(Display *display);
void sendBackspace(Display *display);
void sendCtrlA(Display* display);
void clearWithCtrlA(Display* display);
void send_char(Display *display, const string& utf8_char);
void handleCtrlBackspace(string &buffer);
void handleBackspace(vector<string>& buffer);
void daemonize();