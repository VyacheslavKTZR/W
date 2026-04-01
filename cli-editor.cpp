#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "w.h"

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
    ifstream in(filename);
    if (!in) { err = "Не удалось открыть файл: " + filename; return false; }
    buffer.clear();
    string line;
    while (getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        buffer.push_back(line);
    }
    return true;
}

bool saveFile(const string& filename, const vector<string>& buffer, string& err) {
    ofstream out(filename);
    if (!out) { err = "Не удалось открыть для записи: " + filename; return false; }
    for (size_t i = 0; i < buffer.size(); ++i)
        out << buffer[i] << '\n';
    return true;
}

void showHelp() {
    cout << "Команды редактора (начинаются с ':'):\n"
         << " :help               Справка\n"
         << " :new <file>         Новый файл\n"
         << " :open <file>        Открыть файл\n"
         << " :save               Сохранить\n"
         << " :saveas <file>      Сохранить как\n"
         << " :list               Показать буфер\n"
         << " :insert <n> <text>  Вставить строку перед n\n"
         << " :edit <n> <text>    Изменить строку n\n"
         << " :delete <n>         Удалить строку n\n"
         << " :clear              Очистить буфер\n"
         << " :run [fresh]        Запуск (fresh - сброс переменных)\n"
         << " :runfile <file>     Запустить файл\n"
         << " :exit               Выход\n";
}

void listBuffer(const vector<string>& buffer) {
    for (size_t i = 0; i < buffer.size(); ++i)
        cout << (i + 1) << ": " << buffer[i] << '\n';
}

void runProgram(const vector<string>& lines,
                unordered_map<string, int>& vars,
                unordered_map<string, string>& stringVars)
{
    unordered_map<string, Function> functions;
    preprocessFunctions(lines, functions);

    for (int i = 0; i < (int)lines.size(); ++i) {
        string l = trim(lines[i]);
        // пропускаем тела функций
        if (l.rfind("func ", 0) == 0) {
            while (i < (int)lines.size() && trim(lines[i]) != "endfunc") i++;
            continue;
        }
        if (l == "end") break;
        executeCommand(lines, i, vars, stringVars, functions);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    vector<string> buffer;
    string currentFile;
    bool dirty = false;
    unordered_map<string, int> vars;
    unordered_map<string, string> stringVars;

    cout << "W-language CLI editor. Введите :help для справки.\n";

    string input;
    while (true) {
        cout << (currentFile.empty() ? "[untitled]" : currentFile) << (dirty ? " *" : "") << " > ";
        if (!getline(cin, input)) break;
        string line = trim(input);
        if (line.empty()) continue;

        if (line[0] == ':') {
            istringstream iss(line);
            string cmd;
            iss >> cmd;

            if (cmd == ":help") {
                showHelp();
            }
            else if (cmd == ":new") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :new <file>\n"; continue; }
                ensureWExtension(name);
                buffer.clear();
                currentFile = name;
                dirty = true;
                cout << "Новый буфер, файл: " << currentFile << '\n';
            }
            else if (cmd == ":open") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :open <file>\n"; continue; }
                ensureWExtension(name);
                string err;
                if (loadFile(name, buffer, err)) {
                    currentFile = name;
                    dirty = false;
                    cout << "Файл загружен: " << currentFile << " (" << buffer.size() << " строк)\n";
                } else cout << err << '\n';
            }
            else if (cmd == ":save") {
                if (currentFile.empty()) { cout << "Нет имени файла. Используйте :saveas\n"; continue; }
                string err;
                if (saveFile(currentFile, buffer, err)) { dirty = false; cout << "Сохранено\n"; }
                else cout << err << '\n';
            }
            else if (cmd == ":saveas") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :saveas <file>\n"; continue; }
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
                string text; getline(iss, text); text = trim(text);
                if (n < 1 || n > (int)buffer.size() + 1) { cout << "Неверный индекс\n"; continue; }
                buffer.insert(buffer.begin() + (n - 1), text);
                dirty = true;
            }
            else if (cmd == ":edit") {
                int n;
                if (!(iss >> n)) { cout << "Usage: :edit <n> <text>\n"; continue; }
                string text; getline(iss, text); text = trim(text);
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
            else if (cmd == ":run") {
                string arg; iss >> arg;
                if (arg == "fresh") { vars.clear(); stringVars.clear(); }
                runProgram(buffer, vars, stringVars);
                cout << "Выполнение завершено\n";
            }
            else if (cmd == ":runfile") {
                string name;
                if (!(iss >> name)) { cout << "Usage: :runfile <file>\n"; continue; }
                ensureWExtension(name);
                vector<string> tmp;
                string err;
                if (loadFile(name, tmp, err)) {
                    unordered_map<string, int> tmpVars;
                    unordered_map<string, string> tmpStringVars;
                    runProgram(tmp, tmpVars, tmpStringVars);
                    cout << "Файл выполнен\n";
                } else cout << err << '\n';
            }
            else if (cmd == ":exit") {
                if (dirty) {
                    cout << "Есть несохранённые изменения. Выйти? (y/N): ";
                    string a; getline(cin, a);
                    if (a != "y" && a != "Y") continue;
                }
                break;
            }
            else {
                cout << "Неизвестная команда. :help для справки\n";
            }
        } else {
            buffer.push_back(line);
            dirty = true;
        }
    }

    cout << "Выход.\n";
    return 0;
}
