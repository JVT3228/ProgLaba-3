#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <windows.h>

bool isKeyword(const std::string& word) {
    return word == "for" || word == "while" || word == "do" || word == "continue";
}

bool checkContinueSyntax(const std::string& code) {
    std::stack<std::string> contextStack;
    bool insideString = false;
    bool insideChar = false;
    bool insideSingleComment = false;
    bool insideMultiComment = false;
    std::string currentWord;

    for (size_t i = 0; i < code.length(); ++i) {
        char ch = code[i];

        if (!insideSingleComment && !insideMultiComment) {
            if (ch == '"' && !insideChar) {
                insideString = !insideString;
                continue;
            }
            if (ch == '\'' && !insideString) {
                insideChar = !insideChar;
                continue;
            }
        }

        if (!insideString && !insideChar) {
            if (!insideSingleComment && !insideMultiComment && ch == '/' && i + 1 < code.length()) {
                if (code[i + 1] == '/') {
                    insideSingleComment = true;
                    ++i;
                    continue;
                }
                else if (code[i + 1] == '*') {
                    insideMultiComment = true;
                    ++i;
                    continue;
                }
            }
            else if (insideSingleComment && ch == '\n') {
                insideSingleComment = false;
            }
            else if (insideMultiComment && ch == '*' && i + 1 < code.length() && code[i + 1] == '/') {
                insideMultiComment = false;
                ++i;
                continue;
            }
        }

        if (insideString || insideChar || insideSingleComment || insideMultiComment) {
            continue;
        }

        if (isalnum(ch) || ch == '_') {
            currentWord += ch;
        }
        else {
            if (!currentWord.empty()) {
                if (isKeyword(currentWord)) {
                    if (currentWord == "continue") {
                        if (contextStack.empty() || !isKeyword(contextStack.top())) {
                            std::cout << "Ошибка: 'continue' используется вне цикла на позиции " << i << ".\n";
                            return false;
                        }
                    } else {
                    contextStack.push(currentWord);
                    }
                }
                else if (currentWord == "do") {
                    contextStack.push("do");
                }
                else if (currentWord == "while" && !contextStack.empty() && contextStack.top() == "do") {
                    contextStack.pop();
                }
                currentWord.clear();
            }
            if (ch == '}' && !contextStack.empty()) {
                contextStack.pop();
            }
        }
    }
    return true;
}

void runTest() {
    std::string testCode1 = R"(
        int main() {
            for (int i = 0; i < 10; i++) {
                if (i % 2 == 0) continue;
            }
            return 0;
        }
    )";

    std::string testCode2 = R"(
        int main() {
            if (true) {
                continue;
            }
            return 0;
        }
    )";

    std::cout << "Тест 1 (корректный 'continue'): ";
    if (checkContinueSyntax(testCode1)) {
        std::cout << "Пройден\n";
    }
    else {
        std::cout << "Провален\n";
    }

    std::cout << "Тест 2 (некорректный 'continue'): ";
    if (!checkContinueSyntax(testCode2)) {
        std::cout << "Пройден\n";
    }
    else {
        std::cout << "Провален\n";
    }
}

void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    std::cout << "Запуск тестов...\n";
    runTest();

    setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << "\n!!!Если не работает, то вводите полный путь!!!";
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "\nВведите имя файла для анализа: ";
    std::string filename;
    std::cin >> filename;

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Ошибка: невозможно открыть файл " << filename << "\n";
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (checkContinueSyntax(code)) {
        std::cout << "Код содержит корректное использование операторов 'continue'.\n";
    }
    else {
        std::cout << "Код содержит некорректное использование операторов 'continue'.\n";
    }

    return 0;
}