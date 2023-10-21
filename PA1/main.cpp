#include "lex.cpp"
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <cctype>
#include <algorithm>
#include <set>

using namespace std;
using std::map;

//main function takes command line args
/*
(flags)
-v (every token is printed out, followed by its lexeme)
-nconsts (prints all unique numeric constants)
-sconsts (prints all unique string constants in alphabetical order)
-bconst (prints all boolean constants in order)
-ident (prints all unique identifiers in aplhpabetical order)
..............
filename arg
*/
int main(int argc, char* argv[]) {
    bool printTokens = false;
    bool printIdentifiers = false;
    bool printNumericConstants = false;
    bool printStringConstants = false;
    bool printBooleanConstants = false;
    string filename;
    bool multipleFilenames = false;

    // Process command-line flags
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-v") {
            printTokens = true;
        } else if (arg == "-ident") {
            printIdentifiers = true;
        } else if (arg == "-nconst") {
            printNumericConstants = true;
        } else if (arg == "-sconst") {
            printStringConstants = true;
        } else if (arg == "-bconst") {
            printBooleanConstants = true;
        } else if (arg[0] == '-') {
            cout << "UNRECOGNIZED FLAG " << arg << endl;
            return 1;
        } else {
            if (multipleFilenames) {
                cout << "ONLY ONE FILE NAME IS ALLOWED." << endl;
                return 1;
            }
            filename = arg;
            multipleFilenames = true;
        }
    }

    if (!multipleFilenames) {
        cout << "NO SPECIFIED INPUT FILE." << endl;
        return 1;
    }

    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cout << "CANNOT OPEN THE FILE " << filename << endl;
        return 1;
    }


    string lexeme = "";
    int lineCount = 0;
    int totalTokens = 0;
    int identifierCount = 0;
    int numericConstantCount = 0;
    int booleanConstantCount = 0;
    int stringConstantCount = 0;
    map<string, bool> seenIdentifiers;
    set<string> uniqueNumericConstants;
    set<string> uniqueStringConstants;
    set<string> uniqueBooleanConstants;

    if (printTokens) {
        cout << "Tokens:" << endl;
    }

    while ((getNextToken(inputFile,lineCount)!= ERR) || (getNextToken(inputFile,lineCount)!= DONE )) {
        LexItem token = getNextToken(inputFile, lineCount);
        if (token.GetToken() == DONE) {
            //lexstate = DONE;
            return 1;
        } else if (token.GetToken() == ERR) {
            cout << "Error in line " << token.GetLinenum() << ": Unrecognized Lexeme " << token.GetLexeme() << endl;
            return 1;
        } else {
            totalTokens++;
            if (printTokens) {
                if (token.GetToken() == IDENT || token.GetToken() == ICONST || token.GetToken() == RCONST || token.GetToken() == SCONST || token.GetToken() == BCONST) {
                    cout << token.GetToken() << ": \"" << token.GetLexeme() << "\"" << endl;
                } else {
                    cout << token.GetToken() << endl;
                }
            }
            if (token.GetToken() == IDENT) {
                identifierCount++;
                seenIdentifiers[token.GetLexeme()] = true;
            } else if (token.GetToken() == ICONST || token.GetToken() == RCONST) {
                numericConstantCount++;
                uniqueNumericConstants.insert(token.GetLexeme());
            } else if (token.GetToken() == SCONST) {
                stringConstantCount++;
                uniqueStringConstants.insert(token.GetLexeme());
            } else if (token.GetToken() == BCONST) {
                booleanConstantCount++;
                uniqueBooleanConstants.insert(token.GetLexeme());
            }
        }
    }

    cout << "Lines: " << lineCount << endl;
    cout << "Total Tokens: " << totalTokens << endl;
    cout << "Identifiers: " << identifierCount << endl;
    cout << "Numbers: " << numericConstantCount << endl;
    cout << "Booleans: " << booleanConstantCount << endl;
    cout << "Strings: " << stringConstantCount << endl;

    if (identifierCount > 0 && printIdentifiers) {
        cout << "IDENTIFIERS: ";
        vector<string> identifiers;
        for (const auto& id : seenIdentifiers) {
            identifiers.push_back(id.first);
        }
        sort(identifiers.begin(), identifiers.end());
        for (size_t i = 0; i < identifiers.size(); i++) {
            if (i > 0) {
                cout << ", ";
            }
            cout << identifiers[i];
        }
        cout << endl;
    }

    if (numericConstantCount > 0 && printNumericConstants) {
        cout << "NUMBERS:" << endl;
        for (const string& num : uniqueNumericConstants) {
            cout << num << endl;
        }
    }

    if (booleanConstantCount > 0 && printBooleanConstants) {
        cout << "BOOLEANS:" << endl;
        for (const string& boolean : uniqueBooleanConstants) {
            cout << boolean << endl;
        }
    }

    if (stringConstantCount > 0 && printStringConstants) {
        cout << "STRINGS:" << endl;
        for (const string& str : uniqueStringConstants) {
            cout << "\"" << str << "\"" << endl;
        }
    }

    if (totalTokens == 0) {
        cout << "Empty File." << endl;
    }

    inputFile.close();
    return 0;
}