#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <regex>
#include <conio.h>
#include <Windows.h>
#include <list>

#define count_trash_var 10
#define count_trash_func 10
//два конфигурационных значения

#define SKIP 11
#define SPACE 6

#define CODE_FILE "code1.txt"

using namespace std;

map<string, int> name; //словарь для переменных
int cnt_name = 2, include_pl = 0;;
vector<string> variables; // переменные для замены
list<int> index; // места в которые можно вставить мусор


int symb(char c) {
    return isalpha(c) || isdigit(c) || c == '_';
}

void delete_com(string& code_text) { // удаление комментариев через регулярные выражения
    regex space_reg("(\/\\*([\\s\\S]*?)\\*\/)|\/\/[^\r\n\\\\]*|\/\/[^\r\n]*|\\\\\n[^\n\r\\\\]*");
    code_text = regex_replace(code_text, space_reg, "");
}


int include_place(const string& code_text) { // нахождение конца инклудов через регулярные выражения
    int mx = 0;
    regex include_reg("#include .*|#define .*");
    for (sregex_iterator i = sregex_iterator(code_text.begin(), code_text.end(), include_reg); i != sregex_iterator(); i++) {
        smatch m = *i;
        mx = max((int)m.position(), mx);
    }
    while (code_text[mx++] != '\n');
    return mx;
}


void delete_enter(string& code_text) { // удалеие переноса строк
    for (int i = include_place(code_text); i < code_text.length(); i++) {
        if (code_text[i] == '\n' ) code_text[i] = SKIP;
    }
}

void delete_space(string& code_text) { // удаление пробелов
    regex r("(const|int|float|char|FILE|unsigned|long|short|double|void)\\s|(const|int|float|char|FILE|unsigned|long|short|double|void)\\**\\s*?|else\\s|return\\s");
    for (sregex_iterator i = sregex_iterator(code_text.begin(), code_text.end(), r); i != sregex_iterator(); ++i) {
        smatch m = *i;
        for (int k = m.position() + 1; k <= m.str().length() + m.position(); k++) {
            if (code_text[k] == ' ') code_text[k] = SPACE;
        }
    }
}

void trash_var(string& code_text, vector<bool>& flags) { // внесение мусорных переменных
    int k;
    vector<string> tp = { "int ", "float ", "long long ", "unsigned char ", "double "}; // типы данных вносимых переменных
    string alf = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
    for (int i = 0; i < count_trash_var; i++) {
        index.clear();
        for (int i = 0; i < code_text.length(); ++i) {
            if (code_text[i] == '{') index.push_back(i + 1); // места для переменных
        }
        int tmp = 0;
        for (auto var : index) {
            if (rand() % 10 < 7) continue; // внесение псевдослучайной расстановки
            if ((i++) == count_trash_var) break;
            int cheaker = var+tmp;
            while (code_text[cheaker] == ' ' || code_text[cheaker] == '{') cheaker--;
            if (code_text[cheaker] == '=') {
                i--;
                continue;
            }
            string str = tp[rand()%5]+alf[rand()%52]+to_string(i*3)+alf[rand() % 52]+alf[rand() % 52]+to_string((i * 3) % 70)+alf[rand() % 52]+';'; //генерация имени
            code_text.insert(var + tmp, str);
            flags.insert(flags.begin() + var + tmp, str.length() , 0);
            tmp += str.length();
        }
    }
}

void trash_func(string& code_text, vector<bool>& flags) { // мусорные функции
    int k;
    string alf = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
    for (int i = 0; i < count_trash_func; i++) {
        index.clear();
        for (int i = 0; i < code_text.length(); ++i) {
            if (code_text[i] == '{') index.push_back(i + 1); // места для мусорных функций
        }
        int tmp = 0;
        for (auto var : index) {
            if (rand() % 10 < 7) continue;// внесение псевдослучайной расстановки
            if ((i++) == count_trash_func) break;
            int cheaker = var + tmp;
            while (code_text[cheaker] == ' ' || code_text[cheaker] == '{') cheaker--;
            if (code_text[cheaker] == '=') {
                i--;
                continue;
            }
            string s = alf[rand() % 52] + to_string(i * 3) + alf[rand() % 52] + alf[rand() % 52] + to_string((i * 3) % 70) + alf[rand() % 52]; // генерация имени переменной
            if(i%3==0) // разновидности мусорных функций и их генерация
                s = "for(int " + s + "=" + to_string(rand())+";" + s + "<=" + to_string(rand() % 300) + ';' + s + "++){if(" + s + "&"+to_string(rand())+"==1)"+ s +"*=0x7;}";
            if (i % 3 == 1)
                s = "while(1){int " + s + "=" + to_string(rand() % 9) + "*" + to_string(rand() % 5684) + "*2;" + "if(" + s + "&11==3)break;}";
            if (i % 3 == 2)
                s = "int " + s + "=" + to_string(rand()) + "; do{" + s + "-=" + to_string(rand()) + ";}while(" + s + ">=" + to_string(rand()) + ");";
            code_text.insert(var + tmp, s);
            flags.insert(flags.begin() + var + tmp, s.length(), 0);
            tmp += s.length();
        }
    }
}

void type_cheaker(string& code_text, string type, int& i) {
    string typez = type + "*";
    string types = type + " ";
    if (code_text.substr(i, type.length() + 1) == types || code_text.substr(i, type.length() + 1) == typez) { // нахождение и замена перменных

        i += type.length();
        do {
            string tmp = "";
            while (!symb(code_text[i]))i++;
            while (code_text[i] != ')' && code_text[i] != ',' && code_text[i] != '[' && code_text[i] != ';' && code_text[i] != '=' && code_text[i] != '(') {
                tmp += code_text[i++];
            }
            int n;
            if ((n = tmp.find(" ")) != string::npos) {
                if (tmp.find(" ") != tmp.length() - 1)
                    tmp = tmp.substr(tmp.find(" ") + 1, tmp.length() - tmp.find(" ") - 1);
            }
            if (tmp.back() == ' ') tmp.pop_back();
            if (tmp == "main" || tmp == "malloc" || tmp == "sizeof") {
                i+=tmp.length()-1; continue;
            }
            if (name[tmp] == 0) name[tmp] = cnt_name++;
            string str = variables[name[tmp] - 1];
            if (code_text[i - tmp.length() - 1] == ' ' || code_text[i - tmp.length() - 1] == '*' || code_text[i - tmp.length() - 1] == ')')
                code_text.replace(i - tmp.length(), tmp.length(), str);
            else code_text.replace(i - tmp.length() - 1, tmp.length(), str);
            i -= (tmp.length() - str.length());
            while (code_text[i] != ',' && code_text[i] != ';' && code_text[i] != ')') {
                if (code_text[i] == '(') return; i++;
            }
        }
        while (code_text[i] != ';' && code_text[i] != ')');
    }
}

void change_name(string& code_text, vector<bool>& flags) {
    for (int i = 0; i < code_text.length() - 4; i++) {
        type_cheaker(code_text, "unsigned char", i);
        type_cheaker(code_text, "char", i);
        type_cheaker(code_text, "unsigned int", i);
        type_cheaker(code_text, "int", i);
        type_cheaker(code_text, "unsigned long", i);
        type_cheaker(code_text, "long", i);
        type_cheaker(code_text, "float", i);
        type_cheaker(code_text, "double", i);
        type_cheaker(code_text, "void", i);
        type_cheaker(code_text, "FILE", i);
    }
    for (auto var : name) { // маска переменных
        regex r("[\\[\\]\\s\\(\+\/\%=,&\\)^]" + var.first + "[\\]\\[\\s\\)\+\/\%=,;\\(]");
        for (sregex_iterator i = sregex_iterator(code_text.begin(), code_text.end(), r); i != sregex_iterator(); ++i) {
            smatch m = *i;
            for (int k = m.position() + 1; k < m.str().length() - 1 + m.position(); k++) {
                flags[k] = 1;
            }
        }
    }
}



int main() {
    srand(time(0));
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    bool trash_v, trash_f, del_s, del_e, del_c, swap_n;
    cout << "Добавить мусорные переменные? ";  cin >> trash_v;
    cout << "Добавить мусорные функции? ";  cin >> trash_f;
    cout << "Удалть пробелы/табуляции? ";  cin >> del_s;
    cout << "Удалить переносы строк? ";  cin >> del_e;
    cout << "Удалть коментарии? ";  cin >> del_c;
    cout << "Поменять названия переменных? ";  cin >> swap_n;
    cout << endl << endl << endl;
    FILE* f = fopen(CODE_FILE, "r");
    FILE* var = fopen("variables.txt", "r");
    if (f == NULL || var == NULL) {
        cout << "Не удалось открыть один из конфигурационных файлов!";
        return EXIT_FAILURE;
    }
    string code_text;
    char c;
    string str;
    while (fscanf(f, "%c", &c) != EOF) code_text.push_back(c);
    vector<bool> flags(code_text.size() + 1000);
    while (!feof(var)) {
        c = fgetc(var);
        if (symb(c)) str += c;
        else {
            variables.push_back(str);
            str = "";
        }
    }
    include_pl = include_place(code_text);
    if(del_c) delete_com(code_text);
    if(swap_n) change_name(code_text, flags);
    if(trash_v) trash_var(code_text, flags);
    if(trash_f) trash_func(code_text, flags);
    if(del_e) delete_enter(code_text);
    if(del_s) delete_space(code_text);
    str = "";
    int check=0;
    for (int i = 0; i < include_pl; i++) cout << code_text[i]; // вывод до конца инклудов
    for (int i = include_pl; i < code_text.length(); i++) { //вывод
        if (code_text[i] == '"') check = (check + 1) % 2;
        if (check) {
            cout << code_text[i];
            continue;
        }
        if (code_text[i] == SKIP) continue;
        if (flags[i]) {
            str += code_text[i];
            continue;
        }
        if (str != "") {
            cout << variables[name[str] - 1];
            str = "";
        }
        if (code_text[i] == SPACE) cout << ' ';
        else if ((code_text[i] == ' ' || code_text[i] == '\t') && del_s) continue;
        else cout << code_text[i];
    }
    return EXIT_SUCCESS;
}

