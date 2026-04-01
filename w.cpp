#include "w.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

static string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool evaluateCondition(const string& expr, unordered_map<string, int>& vars) {
    size_t sp1 = expr.find(' ');
    size_t sp2 = expr.find(' ', sp1 + 1);
    if (sp1 == string::npos || sp2 == string::npos) return false;
    string varName = expr.substr(0, sp1);
    string op = expr.substr(sp1 + 1, sp2 - sp1 - 1);
    int rhs = stoi(expr.substr(sp2 + 1));
    if (vars.find(varName) == vars.end()) return false;
    int lhs = vars[varName];
    if (op == "==") return lhs == rhs;
    if (op == "!=") return lhs != rhs;
    if (op == "<")  return lhs <  rhs;
    if (op == ">")  return lhs >  rhs;
    if (op == "<=") return lhs <= rhs;
    if (op == ">=") return lhs >= rhs;
    return false;
}

void executeCommand(
    const vector<string>& lines,
    int& i,
    unordered_map<string, int>& vars,
    unordered_map<string, string>& stringVars,
    unordered_map<string, Function>& functions)
{
    string line = trim(lines[i]);
    if (line.empty() || line == "endif" || line == "else" || line == "endwhile" || line == "endfunc") return;

    if (line.rfind("input ", 0) == 0) {
        string args = line.substr(6);
        size_t sp = args.find(' ');
        string name;
        if (sp != string::npos) {
            name = args.substr(0, sp);
            string prompt = args.substr(sp + 1);
            if (prompt.front() == '"' && prompt.back() == '"')
                prompt = prompt.substr(1, prompt.length() - 2);
            cout << prompt;
        } else {
            name = args;
            cout << "Input " << name << ": ";
        }
        string val;
        getline(cin, val);
        try { vars[name] = stoi(val); }
        catch (...) { stringVars[name] = val; }
    }
    else if (line.rfind("var.createStr ", 0) == 0) {
        stringVars[line.substr(14)] = "";
    }
    else if (line.rfind("var.setStr ", 0) == 0) {
        size_t sp = line.find(' ', 11);
        if (sp != string::npos)
            stringVars[line.substr(11, sp - 11)] = line.substr(sp + 1);
    }
    else if (line.rfind("var.create ", 0) == 0) {
        vars[line.substr(11)] = 0;
    }
    else if (line.rfind("var.set ", 0) == 0 || line.rfind("mov ", 0) == 0) {
        size_t offset = (line[0] == 'v') ? 8 : 4;
        size_t sp = line.find(' ', offset);
        if (sp != string::npos)
            vars[line.substr(offset, sp - offset)] = stoi(line.substr(sp + 1));
    }
    else if (line.rfind("add ", 0) == 0 || line.rfind("sub ", 0) == 0 ||
             line.rfind("mul ", 0) == 0 || line.rfind("div ", 0) == 0) {
        string op = line.substr(0, 3);
        size_t sp = line.find(' ', 4);
        if (sp != string::npos) {
            string name = line.substr(4, sp - 4);
            int val = stoi(line.substr(sp + 1));
            if (op == "add") vars[name] += val;
            else if (op == "sub") vars[name] -= val;
            else if (op == "mul") vars[name] *= val;
            else if (op == "div" && val != 0) vars[name] /= val;
            else if (op == "div") cout << "ERROR: Division by zero\n";
        }
    }
    else if (line.rfind("!cr ", 0) == 0) {
        string name = line.substr(4);
        if (vars.count(name)) cout << vars[name] << endl;
        else if (stringVars.count(name)) cout << stringVars[name] << endl;
        else cout << "ERROR: Variable " << name << " not found\n";
    }
    else if (line.rfind("if ", 0) == 0) {
        bool condition = evaluateCondition(line.substr(3), vars);
        i++;
        if (condition) {
            while (i < (int)lines.size() && trim(lines[i]) != "else" && trim(lines[i]) != "endif") {
                executeCommand(lines, i, vars, stringVars, functions);
                i++;
            }
            while (i < (int)lines.size() && trim(lines[i]) != "endif") i++;
        } else {
            while (i < (int)lines.size() && trim(lines[i]) != "else" && trim(lines[i]) != "endif") i++;
            if (i < (int)lines.size() && trim(lines[i]) == "else") {
                i++;
                while (i < (int)lines.size() && trim(lines[i]) != "endif") {
                    executeCommand(lines, i, vars, stringVars, functions);
                    i++;
                }
            }
        }
    }
    else if (line.rfind("while ", 0) == 0) {
        int startWhile = i;
        string conditionExpr = line.substr(6);
        while (evaluateCondition(conditionExpr, vars)) {
            i = startWhile + 1;
            while (i < (int)lines.size() && trim(lines[i]) != "endwhile") {
                executeCommand(lines, i, vars, stringVars, functions);
                i++;
            }
        }
        while (i < (int)lines.size() && trim(lines[i]) != "endwhile") i++;
    }
    else if (line.rfind("call ", 0) == 0) {
        string funcName = line.substr(5);
        if (functions.count(funcName)) {
            int start = functions[funcName].startLine;
            for (int f = start; f < functions[funcName].endLine; ++f)
                executeCommand(lines, f, vars, stringVars, functions);
        } else {
            cout << "ERROR: Function " << funcName << " not found\n";
        }
    }
    else if (line == "end") { i = (int)lines.size(); }
    else {
        cout << "ERROR: Unknown command: " << line << "\n";
    }
}

void preprocessFunctions(const vector<string>& lines, unordered_map<string, Function>& functions) {
    for (int i = 0; i < (int)lines.size(); ++i) {
        string line = trim(lines[i]);
        if (line.rfind("func ", 0) == 0) {
            string name = line.substr(5);
            int start = i + 1;
            while (i < (int)lines.size() && trim(lines[i]) != "endfunc") i++;
            functions[name] = {start, i};
        }
    }
}
