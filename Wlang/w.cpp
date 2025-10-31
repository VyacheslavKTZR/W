#include "w.h"
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std; // чтобы не ебаться с std:: каждый раз

void executeCommand(const string& line, // строка команды
	unordered_map<string, int>& vars, // числовые переменные
	unordered_map<string, string>& stringVars) // строковые переменные
{
    if (line.empty()) return; // пропускаем пустые строки
    if (line == "end") return; // выход из.. короче уходим на хуй 

    if (line.rfind("var.createStr ", 0) == 0) { // создание строковой переменной
        string name = line.substr(14);
        stringVars[name] = "";
    }
    else if (line.rfind("var.setStr ", 0) == 0) { // установка строки
        size_t spacePos = line.find(' ', 11);
        if (spacePos == string::npos) {
            cout << "ERORR: Missing value for var.setStr\n";
            return;
        }
        string name = line.substr(11, spacePos - 11);
        string value = line.substr(spacePos + 1);
        stringVars[name] = value;
    }
    else if (line.rfind("var.create ", 0) == 0) { // создание числовой переменной
		string name = line.substr(11); // геноцид переменной, выдираем название
        vars[name] = 0; // сначала сыздыым 0
    }
    else if (line.rfind("var.set ", 0) == 0) { // установка числа
		size_t spacePos = line.find(' ', 8); // ищем пробел после названия переменной
		if (spacePos == string::npos) { // если не нашли, то ошибка
            cout << "ERORR: Missing value for var.set\n";
            return; 
        }
		string name = line.substr(8, spacePos - 8); // выдираем название переменной
		string valueStr = line.substr(spacePos + 1); // выдираем значение
        try {
            int value = stoi(valueStr);
            vars[name] = value;
        }
        catch (...) {
            cout << "ERORR: Invalid number: " << valueStr << endl;
        }
    }
    else if (line.rfind("!cr ", 0) == 0) { // вывод переменной
        string name = line.substr(4);
        if (vars.find(name) != vars.end())
            cout << vars[name] << endl;
        else if (stringVars.find(name) != stringVars.end())
            cout << stringVars[name] << endl;
        else
            cout << "ERORR: Unknown variable! " << name << endl;
    }
    else {
        cout << "ERORR: Unknown command! " << line << endl;
    }
}

