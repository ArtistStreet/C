#include "../lib.h"

bool isAccentKey(char c) {
    return c == 'a' || c == 'e' || c == 'o' || c == 'w' || c == 'd';
}

bool found = false;

void handleReverseTelexTransform(vector<string> &buffer, char c) { // aw -> aa
    if (buffer.empty()) return ;

    for (int i = buffer.size() - 1; i >= 0; i--) {
        auto it = reverseTelexTransform.find(buffer[i]);
        if (it != reverseTelexTransform.end()) {
            if (c == it->second.second) {
                buffer[i] = it->second.first;
            }
        }
    }
}

vector<string> handleTelexTransform(vector<string>& buffer, Display* display, char c, bool &found) {
    vector<string> result;
    string last = string(1, c);
    int count = 0;
    unordered_map<string, string>::iterator it;

    auto checkIt = checkTelexExist.find(c); // Use a different variable for lookup
    if (checkIt != checkTelexExist.end()) {
        for (int i = 0; i < buffer.size(); i++) {
            if (buffer[i] == checkIt->second) {
                buffer.push_back(string(1, c));
                buffer[i] = removeAccents(buffer[i]);
                // cout << "Debug buffer: " << buffer[i];
                isWrong = true;
                for (int i = 0; i < (int)buffer.size(); i++) {
                    sendBackspace(display);
                }
                string temp = "";
                for (auto &&i : buffer)
                {
                    temp += i;
                }

                send_char(display, temp);
                return buffer;
            }
        }
    }

    handleReverseTelexTransform(buffer, c);

    for (int i = (int)buffer.size() - 1; i >= 0; --i) {
        if (c == 'w') {
            it = telexMapW.find(buffer[i]);
            if (it != telexMapW.end()) {
                found = true;
                count++;
                result.push_back(it->second);
                continue;
            }
        }
        else {
            it = telexMap.find(last);
            if ((it != telexMap.end()) && (buffer[i] == last)) {
                found = true;
                count++;
                result.push_back(it->second);
                fflush(stdout);
                continue;
            }
        }
        result.push_back(buffer[i]);
    }

    if (found) {
        reverse(result.begin(), result.end());
        for (int i = 0; i < (int)buffer.size() + 1; i++) {
            sendBackspace(display);
        }
        string temp = "";
        if (result[result.size() - 1] == "ă" and result[result.size() - 2] == "ư") {
            result[result.size() - 1] = "a";
        }
        for (auto &&i : result)
        {
            // cout << i << " "; 
            temp += i;
        }
        // cout << "DEBUG temp size: " << temp.size() << endl;
        send_char(display, temp);
        return result;
    }
    return buffer;
}