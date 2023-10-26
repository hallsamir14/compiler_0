#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include "lex.cpp"
// Define the LexItem structure and the getNextToken function here
// ...

int main(int argc, char* argv[]) {
    bool printTokens = false;
    bool printNConst = false;
    bool printSConst = false;
    bool printBConst = false;
    bool printIdent = false;
    std::string filename;

    bool validArgumentFound = false; // Flag to track if a valid argument was found
    bool printSummary = true;      // Flag to print the summary information

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.size() > 1 && arg[0] == '-') {
            // Argument starts with '-', so it's a flag
            if (arg == "-v") {
                printTokens = true;
                validArgumentFound = true;
                printSummary = true;
            } else if (arg == "-nconst") {
                printNConst = true;
                validArgumentFound = true;
                //printSummary = false;

            } else if (arg == "-sconst") {
                printSConst = true;
                validArgumentFound = true;
                printSummary = false;
            } else if (arg == "-bconst") {
                printBConst = true;
                validArgumentFound = true;
                //printSummary = false;
            } else if (arg == "-ident") {
                printIdent = true;
                validArgumentFound = true;
                //printSummary = false;
            } else {
                std::cout << "UNRECOGNIZED FLAG " << "{"<< arg << "}" << std::endl;
                return 1;
            }
        } else {
            // Argument doesn't start with '-', so it's a filename
            if (filename.empty()) {
                filename = arg;
                validArgumentFound = true;
            } else {
                std::cout << "ONLY ONE FILE NAME IS ALLOWED." << std::endl;
                return 1;
            }
        }
    }
        if (filename.empty()) {
        std::cout << "NO SPECIFIED INPUT FILE." << std::endl;
        return 1;
    }
    if (!validArgumentFound) {
        std::cout << "No valid argument found." << std::endl;
        return 1;
    }



    // Open the input file
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cout << "CANNOT OPEN THE FILE " << filename << std::endl;
        return 1;
    }

    // Initialize data structures to collect tokens
    std::vector<LexItem> tokens;
    std::set<std::string> uniqueStrings;
    std::set<double> uniqueNumbers;
    std::set<std::string> uniqueBooleans;
    std::set<std::string> uniqueIdentifiers;


    int lineCount = 0; // Initialize the line number
    LexItem token;

    // Process tokens and collect data
    while ((token = getNextToken(inputFile, lineCount)).GetToken() != Token::DONE) {
        if (token.GetToken() == Token::ERR) {
            break;
        }
        
        //print all tokens for all tokens flag
        if (printTokens) {
            if (token.GetToken() == Token::IDENT){
               
                std::cout << "IDENT: \"" << token.GetLexeme() << "\"" <<endl;
            }
            else if(token.GetToken() == Token::ICONST){
                std::cout << "ICONST: \"" << token.GetLexeme() << "\"" <<endl;
            }
            else if(token.GetToken() == Token::RCONST){
                std::cout << "RCONST: \"" << token.GetLexeme() << "\"" <<endl;
            }
            else if(token.GetToken() == Token::SCONST){
                std::cout << "SCONST: \"" << token.GetLexeme() << "\"" <<endl;
            }
            else if(token.GetToken() == Token::BOOLEAN && (token.GetLexeme() == "true" || token.GetLexeme() == "false")){
                std::cout << "BCONST: \"" << token.GetLexeme() << "\"" <<endl;
            }
            else{
                std::cout << token.GetLexeme() <<endl;
            }
        }
        
        
    
        if (token.GetToken() == Token::IDENT) {
            uniqueIdentifiers.insert(token.GetLexeme());
        } 
        else if (token.GetToken() == Token::ICONST) {
            uniqueNumbers.insert(std::stod(token.GetLexeme()));
        }
        else if (token.GetToken() == Token::RCONST){
            uniqueNumbers.insert(std::stod(token.GetLexeme()));  
        }
        else if (token.GetToken() == Token::SCONST) {
            uniqueStrings.insert(token.GetLexeme());
        } 
        //only count and print boolean tokens with "true" or "false" lexeme value
        else if (token.GetToken() == Token::BOOLEAN) {
            if (token.GetLexeme() == "true" || token.GetLexeme() == "false") {
                uniqueBooleans.insert(token.GetLexeme());}
        }

        tokens.push_back(token);
    }
        //if last token was an ERR
    if (token.GetToken() == Token::ERR) {
          
                std::cout << "Error in line " << token.GetLinenum() << ": Unrecognized Lexeme " << "{" << token.GetLexeme() << "}" << std::endl;
                return 0;
            }

        if (tokens.empty()) {
            cout << "Empty File." << endl;
            return 0;
    }

    if (printSummary) {
        std::cout << endl << "Lines: " << lineCount << std::endl;
        std::cout << "Total Tokens: " << tokens.size() << std::endl;
        std::cout << "Identifiers: " << uniqueIdentifiers.size() << std::endl;
        std::cout << "Numbers: " << uniqueNumbers.size() << std::endl;
    

        std::cout << "Booleans: ";
        if (uniqueBooleans.size() > 0) {

            cout << uniqueBooleans.size() << std::endl;
        } else {
            cout << 0 << endl;
        }

        std::cout << "Strings: " << uniqueStrings.size() << std::endl;
    }

    

    // Process and print additional data based on flags
    if (printIdent) {
    std::cout << "IDENTIFIERS:" << endl;
    auto it = uniqueIdentifiers.begin();
    auto last = uniqueIdentifiers.end();
    for (; it != last; ++it) {
        std::cout << *it;
        if (std::next(it) != last) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

    if (printNConst) {
        std::cout << "NUMBERS:" << std::endl;
        for (const auto& num : uniqueNumbers) {
            std::cout << num << std::endl;
        }


    }

    if (printBConst) {
        std::cout << "BOOLEANS:" << std::endl;
        for (const auto& boolean : uniqueBooleans) {
            std::cout << boolean << std::endl;
        }
    }

    if (printSConst) {
        std::cout << "STRINGS:" << std::endl;
        for (const auto& str : uniqueStrings) {
            std::cout << "\"" << str << "\"" << std::endl;
        }
    }
    

    inputFile.close();
    return 0;
}
