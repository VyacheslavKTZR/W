#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include "w.h"

/*                      
 .----------------.
| .--------------. |
| | _____  _____ | |
| ||_   _||_   _|| |
| |  | | /\ | |  | |
| |  | |/  \| |  | |
| |  |   /\   |  | |
| |  |__/  \__|  | |
| |              | |
| '--------------' |
 '----------------'



 This file is part of W-language interpreter test CLI editor.


            __  __ ____  _____  ______ ____  _   _  ___    __    __    __ _     _   _     _
     /\   |  \/  |  _ \|  __ \|  ____/ __ \| \ | |/ _ \  / /   / /   / _| |   (_) | |   | |
    /  \  | \  / | |_) | |__) | |__ | |  | |  \| | (_) |/ /_  / /_  | |_| |__  _  | |__ | | __
   / /\ \ | |\/| |  _ <|  _  /|  __|| |  | | . ` |> _ <| '_ \| '_ \ |  _| '_ \| | | '_ \| |/ /
  / ____ \| |  | | |_) | | \ \| |___| |__| | |\  | (_) | (_) | (_) || | | | | | |_| | | |   <
 /_/    \_\_|  |_|____/|_|  \_\______\____/|_| \_|\___/ \___/ \___(_)_| |_| |_|_(_)_| |_|_|\_\






*/
using namespace std;





static inline string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool ensureWExtension(string& name) {
    if (name.size() >= 2 && name.substr(name.size() - 2) == ".w") return true;
    name += ".w";
    return true;
}

bool loadFile(const string& filename, vector<string>& buffer, string& err) {
    setlocale(LC_ALL, "Russian");
    ifstream in(filename);
    if (!in) { err = "Íå óäàëîñü îòêðûòü ôàéë äëÿ ÷òåíèÿ: " + filename; return false; }
    buffer.clear();
    string line;
    while (std::getline(in, line)) {
        // ñîõðàíÿåì ñòðîêè áåç CR
        if (!line.empty() && line.back() == '\r') line.pop_back();
        buffer.push_back(line);
    }
    return true;
}

bool saveFile(const string& filename, const vector<string>& buffer, string& err) {
    setlocale(LC_ALL, "Russian");
    ofstream out(filename);
    if (!out) { err = "Íå óäàëîñü îòêðûòü ôàéë äëÿ çàïèñè: " + filename; return false; }
    for (size_t i = 0; i < buffer.size(); ++i) {
        out << buffer[i];
        if (i + 1 < buffer.size()) out << '\n';
    }
    return true;
}

void showHelp() {
    setlocale(LC_ALL, "Russian");
    cout <<
    "Êîìàíäû ðåäàêòîðà (íà÷èíàþòñÿ ñ ':'):\n"
    " :help                 Ïîêàçàòü ýòó ñïðàâêó\n"
    " :new <file>           Ñîçäàòü íîâûé áóôåð è óñòàíîâèòü èìÿ ôàéëà\n"
    " :open <file>          Îòêðûòü .w ôàéë â áóôåðå\n"
    " :save                 Ñîõðàíèòü â òåêóùåå èìÿ ôàéëà\n"
    " :saveas <file>        Ñîõðàíèòü â óêàçàííîå èìÿ\n"
    " :list                 Ïîêàçàòü áóôåð ñ íîìåðàìè ñòðîê\n"
    " :insert <n> <text>    Âñòàâèòü ñòðîêó ïåðåä íîìåðîì n (1-based)\n"
    " :edit <n> <text>      Çàìåíèòü ñòðîêó n\n"
    " :delete <n>           Óäàëèòü ñòðîêó n\n"
    " :clear                Î÷èñòèòü áóôåð\n"
    " :run [fresh]          Çàïóñòèòü áóôåð (áåç àðãóìåíòà — conserva ñîñòîÿíèå ïåðåìåííûõ ìåæäó çàïóñêàìè; 'fresh' — ñáðîñèòü ïåðåìåííûå)\n"
    " :runfile <file>       Îòêðûòü ôàéë è ñðàçó âûïîëíèòü (áåç çàãðóçêè â áóôåð)\n"
    " :exit                 Âûéòè\n"
    "Ëþáàÿ ñòðîêà, íå íà÷èíàþùàÿñÿ ñ ':' — äîáàâëÿåòñÿ â áóôåð (append).\n";
}

void listBuffer(const vector<string>& buffer) {
    for (size_t i = 0; i < buffer.size(); ++i) {
        cout << (i + 1) << ": " << buffer[i] << '\n';
    }
}

int main() {
    vector<string> buffer;
    string currentFile;
    bool dirty = false;

    unordered_map<string, int> vars;
    unordered_map<string, string> stringVars;
    setlocale(LC_ALL, "Russian");
    cout << "W-language CLI editor. Ââåäèòå :help äëÿ ñïðàâêè.\n";

    string input;
    while (true) {
        cout << (currentFile.empty() ? "[untitled]" : currentFile) << (dirty ? " *" : "") << " > ";
        if (!std::getline(cin, input)) break;
        string line = trim(input);
        if (line.empty()) continue;

        if (line[0] == ':') {
            // êîìàíäà
            istringstream iss(line);
            string cmd;
            iss >> cmd; // fafpjfasjfsafjfjsafjsafjsafjsai
            if (cmd == ":help") {
                showHelp();
            }
            else if (cmd == ":new") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :new <filename>\n"; continue; }
                ensureWExtension(name);
                buffer.clear();
                currentFile = name;
                dirty = true;
                cout << "Íîâûé áóôåð, ôàéë: " << currentFile << '\n';
            }
            else if (cmd == ":open") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :open <filename>\n"; continue; }
                ensureWExtension(name);
                string err;
                if (loadFile(name, buffer, err)) {
                    currentFile = name;
                    dirty = false;
                    cout << "Ôàéë çàãðóæåí: " << currentFile << " (" << buffer.size() << " ñòðîê)\n";
                } else {
                    cout << err << '\n';
                }
            }
            else if (cmd == ":save") {
                if (currentFile.empty()) { cout << "Íåò òåêóùåãî èìåíè ôàéëà. Èñïîëüçóéòå :saveas <file>\n"; continue; }
                string err;
                if (saveFile(currentFile, buffer, err)) {
                    dirty = false;
                    cout << "Ñîõðàíåíî â " << currentFile << '\n';
                } else cout << err << '\n';
            }
            else if (cmd == ":saveas") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :saveas <filename>\n"; continue; }
                ensureWExtension(name);
                string err;
                if (saveFile(name, buffer, err)) {
                    currentFile = name;
                    dirty = false;
                    cout << "Ñîõðàíåíî â " << currentFile << '\n';
                } else cout << err << '\n';
            }
            else if (cmd == ":list") {
                listBuffer(buffer);
            }
            else if (cmd == ":insert") {
                int n;
                if (!(iss >> n)) { cout << "Usage: :insert <n> <text>\n"; continue; }
                string text;
                getline(iss, text);
                text = trim(text);
                if (n < 1 || n > (int)buffer.size() + 1) { cout << "Íåâåðíûé èíäåêñ\n"; continue; }
                buffer.insert(buffer.begin() + (n - 1), text);
                dirty = true;
            }
            else if (cmd == ":edit") {
                int n;
                if (!(iss >> n)) { cout << "Usage: :edit <n> <text>\n"; continue; }
                string text;
                getline(iss, text);
                text = trim(text);
                if (n < 1 || n > (int)buffer.size()) { cout << "Íåâåðíûé èíäåêñ\n"; continue; }
                buffer[n - 1] = text;
                dirty = true;
            }
            else if (cmd == ":delete") {
                int n;
                if (!(iss >> n)) { cout << "Usage: :delete <n>\n"; continue; }
                if (n < 1 || n > (int)buffer.size()) { cout << "Íåâåðíûé èíäåêñ\n"; continue; }
                buffer.erase(buffer.begin() + (n - 1));
                dirty = true;
            }
            else if (cmd == ":clear") {
                buffer.clear();
                dirty = true;
            }
            else if (cmd == ":run" || cmd == ":run!") {
                // :run [fresh]
                string arg;
                iss >> arg;
                bool fresh = (arg == "fresh");
                if (fresh) {
                    vars.clear();
                    stringVars.clear();
                }
                // âûïîëíÿåì ñòðîêè ïî ïîðÿäêó
                for (const auto& ln : buffer) {
                    executeCommand(ln, vars, stringVars);
                    if (ln == "end") break;
                }
                cout << "Âûïîëíåíèå çàâåðøåíî\n";
            }
            else if (cmd == ":runfile") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :runfile <filename>\n"; continue; }
                ensureWExtension(name);
                vector<string> tmp;
                string err;
                if (!loadFile(name, tmp, err)) { cout << err << '\n'; continue; }
                unordered_map<string, int> tmpVars;
                unordered_map<string, string> tmpStringVars;
                for (const auto& ln : tmp) {
                    executeCommand(ln, tmpVars, tmpStringVars);
                    if (ln == "end") break;
                }
                cout << "Âûïîëíåíèå ôàéëà " << name << " çàâåðøåíî\n";
            }
            else if (cmd == ":exit") {
                if (dirty) {
                    cout << "Åñòü íåñîõðàí¸ííûå èçìåíåíèÿ. Âûéòè áåç ñîõðàíåíèÿ? (y/N): ";
                    string a; getline(cin, a);
                    if (a != "y" && a != "Y") continue;
                }
                break;
            }
            else {
                cout << "Íåèçâåñòíàÿ êîìàíäà. :help äëÿ ñïðàâêè\n";
            }
        }
        else {
            // ñòðîêà íå êîìàíäà — äîáàâëÿåì â áóôåð (append)
            buffer.push_back(line);
            dirty = true;
        }
    }

    cout << "Âûõîä.\n";
    return 0;
}
