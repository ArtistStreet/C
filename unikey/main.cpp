#include "lib.h"
#include "config/config.h"

vector<string> buffer; // Change buffer to a single string instead of a vector<string>

void buildReverseAccentMap(
    const unordered_map<string, unordered_map<char, string>>& accentMap,
    unordered_map<string, pair<string, char>>& reverseMap
) {
    for (const auto& [baseChar, toneMap] : accentMap) {
        for (const auto& [toneChar, composedChar] : toneMap) {
            reverseMap[composedChar] = {baseChar, toneChar};
        }
    }
}

bool isEnglish = false;
bool modeSwitchPressed = false;
bool isWrong = false;

bool getStartupStatus() {
    ifstream file("config/run_on_startup.ini");
    string line;
    while (getline(file, line)) {
        if (line.find("run_on_startup") != string::npos) {
            return line.find("true") != string::npos;
        }
    }
    return false;
}

void setStartupStatus(bool enable) {
    ofstream file("config/run_on_startup.ini");
    file << "run_on_startup = " << (enable ? "true" : "false") << endl;
}

namespace fs = std::filesystem;

bool getStartupStatus(const string& path) {
    ifstream infile(path);
    if (!infile.is_open()) return false;

    string line;
    while (getline(infile, line)) {
        if (line.find("X-GNOME-Autostart-enabled=") == 0) {
            return line.find("true") != string::npos;
        }
    }
    return false;
}

void setAutostartEnabled(bool enable) {
    string path = string(getenv("HOME")) + "/.config/autostart/unikeyathome.desktop";
    if (!fs::exists(path)) {
        system(("cp unikeyathome.desktop " + path).c_str());
    }

    ifstream infile(path);
    if (!infile.is_open()) {
        cerr << "Cannot open autostart file for reading.\n";
        return;
    }

    stringstream buffer;
    string line;
    bool found = false;
    while (getline(infile, line)) {
        if (line.find("X-GNOME-Autostart-enabled=") == 0) {
            line = "X-GNOME-Autostart-enabled=" + string(enable ? "true" : "false");
            found = true;
        }
        buffer << line << '\n';
    }
    infile.close();

    if (!found) {
        buffer << "X-GNOME-Autostart-enabled=" << (enable ? "true" : "false") << '\n';
    }

    ofstream outfile(path);
    if (!outfile.is_open()) {
        cerr << "Cannot open autostart file for writing.\n";
        return;
    }

    outfile << buffer.str();
    outfile.close();
}


int main() {
    // loadConfig("config.ini");
    // if (!getConfigBool("enabled")) return 0;
    // // daemonize();

    Display *display = XOpenDisplay(nullptr); // X11
    if (!display) {
        cerr << "Cannot open display. Ensure X server is running.\n";
        return 1;
    }
    

    Window root = DefaultRootWindow(display);
    int opcode, event, error;

    if (!XQueryExtension(display, "XInputExtension", &opcode, &event, &error)) {
        cerr << "XInputExtension not available\n";
        XCloseDisplay(display);
        return 1;
    }

    XIEventMask evmask; // Event type
    unsigned char mask[(XI_LASTEVENT + 7) / 8] = {0}; // Create byte array ceil(XI_Lastevent / 8)
    XISetMask(mask, XI_KeyPress); // Listen only for key press events
    XISetMask(mask, XI_FocusIn);
    XISetMask(mask, XI_ButtonPress);

    evmask.deviceid = XIAllDevices; // Listen from all devices
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;
    XISelectEvents(display, root, &evmask, 1);

    cout << "\033[0;36m<Welcome to UnikeyAtHome>\033[0m\n"
    "\033[1;34m      \\\n"
    "      /\\_/\\\n"
    "     ( o.o )\n"
    "      > ^ <\033[0m\n";
    
    int fixBackspace = 0;
    
    bool current = getStartupStatus(string(getenv("HOME")) + "/.config/autostart/unikeyathome.desktop");
    cout << "[1]: Run on startup: " << (current ? "\033[0;31mtrue\033[0m" : "\033[0;31mfalse\033[0m") << endl;
    char ch;
    cin >> ch;

    if (ch == '1') {
        bool newStatus = !current;
        setAutostartEnabled(newStatus);
        cout << "Updated startup status: " << (newStatus ? "\033[0;31mtrue\033[0m" : "\033[0;31mfalse\033[0m") << endl;
    }
    while (true) {
        // bool current = getStartupStatus();

        // cout << "Press 1 to toggle startup status..." << endl;
        XEvent ev; // Holds data for an event
        XNextEvent(display, &ev);
        if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == opcode) { // Handle event
            XGetEventData(display, &ev.xcookie); // Get data
            if (ev.xcookie.evtype == XI_FocusIn) {
                buffer.clear();
                isWrong = false; // Reset isWrong
                // cout << "[FocusIn] Reset buffer\n";
            }
            if (ev.xcookie.evtype == XI_ButtonPress) {
                buffer.clear();
                isWrong = false; // Reset isWrong
                // cout << "[Button Press] Reset buffer\n";
            }
            if (ev.xcookie.evtype == XI_KeyPress) { // Check if key press
                XIDeviceEvent* xievent = (XIDeviceEvent*)ev.xcookie.data; // Get keycode

                if (handleModifiersAndControlKeys(xievent, display, buffer, isEnglish)) {
                    continue;
                }
                char c = keycode_to_char(display, xievent->detail);

                if (isascii(c) && isprint(c) && isEnglish == false && isWrong == false) {
                    // cout << "Lan " << ++cnt << endl; 
                    // cout << "Ki tu: " << c << endl; 

                    // cout << "Buffer before: ";
                    // for (auto &&i : buffer)
                    // {
                        // cout << i;
                    // }
                    // cout << endl;
                    bool foundBs = false;

                    if ((int)buffer.size() >= 1) {
                        if ((telexMap.find(string(1, c)) != telexMap.end()) || (c == 'w')) { // Xu li khi khong co w
                            buffer = handleTelexTransform(buffer, display, c, foundBs);
                            if (foundBs)
                                fixBackspace = (int)buffer.size();
                        } 
                        else if (isXJSRF(c)) {
                            buffer = handleXJSRF(buffer, display, c, foundBs);
                            if (foundBs){
                                fixBackspace = (int)buffer.size();
                            }
                        }
                    }  
                    if (foundBs == false) {
                        buffer.push_back(string(1, c)); // Append character to the buffer
                    }

                    // cout << "FIX " << fixBackspace << endl;
                    if (fixBackspace) {
                        for (int i = 0; i < fixBackspace + 1; i++) {
                            buffer.push_back("~");
                        }
                    }
                    fixBackspace = 0;

                }
                else {
                    continue;
                }
                XFreeEventData(display, &ev.xcookie);
            }
        }
    }
    
    XCloseDisplay(display);
    return 0;
}