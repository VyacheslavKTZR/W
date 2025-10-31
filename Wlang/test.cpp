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
    if (!in) { err = "Не удалось открыть файл для чтения: " + filename; return false; }
    buffer.clear();
    string line;
    while (std::getline(in, line)) {
        // сохраняем строки без CR
        if (!line.empty() && line.back() == '\r') line.pop_back();
        buffer.push_back(line);
    }
    return true;
}

bool saveFile(const string& filename, const vector<string>& buffer, string& err) {
    setlocale(LC_ALL, "Russian");
    ofstream out(filename);
    if (!out) { err = "Не удалось открыть файл для записи: " + filename; return false; }
    for (size_t i = 0; i < buffer.size(); ++i) {
        out << buffer[i];
        if (i + 1 < buffer.size()) out << '\n';
    }
    return true;
}

void showHelp() {
    setlocale(LC_ALL, "Russian");
    cout <<
    "Команды редактора (начинаются с ':'):\n"
    " :help                 Показать эту справку\n"
    " :new <file>           Создать новый буфер и установить имя файла\n"
    " :open <file>          Открыть .w файл в буфере\n"
    " :save                 Сохранить в текущее имя файла\n"
    " :saveas <file>        Сохранить в указанное имя\n"
    " :list                 Показать буфер с номерами строк\n"
    " :insert <n> <text>    Вставить строку перед номером n (1-based)\n"
    " :edit <n> <text>      Заменить строку n\n"
    " :delete <n>           Удалить строку n\n"
    " :clear                Очистить буфер\n"
    " :run [fresh]          Запустить буфер (без аргумента — conserva состояние переменных между запусками; 'fresh' — сбросить переменные)\n"
    " :runfile <file>       Открыть файл и сразу выполнить (без загрузки в буфер)\n"
    " :exit                 Выйти\n"
    "Любая строка, не начинающаяся с ':' — добавляется в буфер (append).\n";
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
    cout << "W-language CLI editor. Введите :help для справки.\n";

    string input;
    while (true) {
        cout << (currentFile.empty() ? "[untitled]" : currentFile) << (dirty ? " *" : "") << " > ";
        if (!std::getline(cin, input)) break;
        string line = trim(input);
        if (line.empty()) continue;

        if (line[0] == ':') {
            // команда
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
                cout << "Новый буфер, файл: " << currentFile << '\n';
            }
            else if (cmd == ":open") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :open <filename>\n"; continue; }
                ensureWExtension(name);
                string err;
                if (loadFile(name, buffer, err)) {
                    currentFile = name;
                    dirty = false;
                    cout << "Файл загружен: " << currentFile << " (" << buffer.size() << " строк)\n";
                } else {
                    cout << err << '\n';
                }
            }
            else if (cmd == ":save") {
                if (currentFile.empty()) { cout << "Нет текущего имени файла. Используйте :saveas <file>\n"; continue; }
                string err;
                if (saveFile(currentFile, buffer, err)) {
                    dirty = false;
                    cout << "Сохранено в " << currentFile << '\n';
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
                    cout << "Сохранено в " << currentFile << '\n';
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
                if (n < 1 || n > (int)buffer.size() + 1) { cout << "Неверный индекс\n"; continue; }
                buffer.insert(buffer.begin() + (n - 1), text);
                dirty = true;
            }
            else if (cmd == ":edit") {
                int n;
                if (!(iss >> n)) { cout << "Usage: :edit <n> <text>\n"; continue; }
                string text;
                getline(iss, text);
                text = trim(text);
                if (n < 1 || n > (int)buffer.size()) { cout << "Неверный индекс\n"; continue; }
                buffer[n - 1] = text;
                dirty = true;
            }
            else if (cmd == ":delete") {
                int n;
                if (!(iss >> n)) { cout << "Usage: :delete <n>\n"; continue; }
                if (n < 1 || n > (int)buffer.size()) { cout << "Неверный индекс\n"; continue; }
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
                // выполняем строки по порядку
                for (const auto& ln : buffer) {
                    executeCommand(ln, vars, stringVars);
                    if (ln == "end") break;
                }
                cout << "Выполнение завершено\n";
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
                cout << "Выполнение файла " << name << " завершено\n";
            }
            else if (cmd == ":exit") {
                if (dirty) {
                    cout << "Есть несохранённые изменения. Выйти без сохранения? (y/N): ";
                    string a; getline(cin, a);
                    if (a != "y" && a != "Y") continue;
                }
                break;
            }
            else {
                cout << "Неизвестная команда. :help для справки\n";
            }
        }
        else {
            // строка не команда — добавляем в буфер (append)
            buffer.push_back(line);
            dirty = true;
        }
    }

    cout << "Выход.\n";
    return 0;
}
