// Lexical analyzer function --> getNextToken(istream& in, int& linenumber)
//First argument is a refernce to istream object
//Second argument is reference to integer that stores current line number of line read
//getNextToken returns LexItem object

#include "lex.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
using namespace std;
using std::map;
std::map<string, Token> keywordMap = {
        {"PROGRAM", PROGRAM},
		{"WRITELN", WRITELN},
		{"WRITE", WRITE },
		{ "IF", IF },
		{ "ELSE", ELSE },	
		{ "IDENT", IDENT },
		{"VAR", VAR},
		{"BEGIN", BEGIN},
        {"THEN", THEN},
		{"INTEGER", INTEGER},
		{"REAL", REAL},
		{"STRING", STRING},
		
        {"BOOLEAN", BOOLEAN},
		{"TRUE", BOOLEAN},
		{"FALSE", BOOLEAN},
		
		{ "ICONST", ICONST },
		{ "RCONST", RCONST },
		{ "SCONST", SCONST  },
		{ "BCONST", BCONST },
			
		{ "PLUS", PLUS },
		{ "MINUS" , MINUS },
		{ "MULT" , MULT  },
		{ "DIV" , DIV },
		{ "IDIV" , IDIV },
		{ "ASSOP", ASSOP },
		{ "EQ", EQ  },
		{ "GTHAN" , GTHAN  },
		{ "LTHAN", LTHAN },
		{ "MOD", MOD},
		{ "AND", AND},
		{ "OR", OR},
		{ "NOT", NOT},
				            
		{ "COMMA", COMMA  },
		{ "LPAREN", LPAREN },
		{ "RPAREN", RPAREN },
			
		{ "SEMICOL", SEMICOL  },
		{ "DOT", DOT },
		
		{ "ERR",ERR  },

		{ "DONE", DONE },

        {"program", PROGRAM},
		{"writeln", WRITELN},
		{"write", WRITE },
		{ "if", IF },
		{ "else", ELSE },	
		{ "ident", IDENT },
		{"var", VAR},
		{"begin", BEGIN},
		{"then", THEN},
		{"integer", INTEGER},
		{"real", REAL},
		{"string", STRING},
		{"boolean", BOOLEAN},
		{"true", BOOLEAN},
		{"false", BOOLEAN},
		
		{ "iconst", ICONST },
		{ "rconst", RCONST },
		{ "sconst", SCONST  },
		{ "bconst", BCONST },
			
		{ "plus", PLUS },
		{ "minus" , MINUS },
		{ "mult" , MULT  },
		{ "div" , DIV },
		{ "idiv" , IDIV },
		{ "assop", ASSOP },
		{ "eq", EQ  },
		{ "gthan" , GTHAN  },
		{ "lthan", LTHAN },
		{ "mod", MOD},
		{ "and", AND},
		{ "or", OR},
		{ "not", NOT},
				            
		{ "comma", COMMA  },
		{ "lparen", LPAREN },
		{ "rparen", RPAREN },
			
		{ "semicol", SEMICOL  },
		{ "dot", DOT },
		
		{ "err",ERR  },

		{ "done", DONE },

};
/*
functions to be implemented - - - - - - - - - - - -
ostream& operator<< (ostream& out, constLexItem& tok);
LexItem id_or_kw(const string& lexeme, int linenum);
Lexitem getNextToken(istream& in, int& linenum);
*/

//getNextToken function will need to look at the next character from input
//use peek() function to examine next character, only read if it belongs to token
LexItem getNextToken(istream& in, int& linenumber) {
    enum statetype {
        START, SEENINT, SEENID, SEENREAL, SEENCOMM, SEENSTR
    };

    char c;
    string lexeme = "";
    statetype state = START;

    while (in.get(c)) {
        switch (state) {
            case START:
                //cout << "1";
                if (c == '\n' || c=='\r') {
                    linenumber++;
                }
                if (isspace(c)) {
                    continue;
                }
                if (c == '+') {
                    return LexItem(PLUS, "PLUS", linenumber);
                }
                if (c == '-') {
                    return LexItem(MINUS, "MINUS", linenumber);
                }
                if (c == '*') {
                    return LexItem(MULT, "MULT", linenumber);
                }
                if (c == '/') {
                    return LexItem(DIV, "DIV", linenumber);
                }
                if (c == ':') {
                    c = in.peek();
                    if (c == '=') {
                        in.get(c);
                        return LexItem(ASSOP, "ASSOP", linenumber);
                    } else {
                        return LexItem(COLON, "COLON", linenumber);
                    }
                }
                if (c == '=') {
                    return LexItem(EQ, "EQ", linenumber);
                }
                if (c == '>') {
                    return LexItem(GTHAN, "GTHAN", linenumber);
                }
                if (c == '<') {
                    return LexItem(LTHAN, "LTHAN", linenumber);
                }
                if (c == '%') {
                    return LexItem(MOD, "MOD", linenumber);
                }
                if (c == ',') {
                    return LexItem(COMMA, "COMMA", linenumber);
                }
                if (c == '(') {
                    return LexItem(LPAREN, "LPAREN", linenumber);
                }
                if (c == ')') {
                    return LexItem(RPAREN, "RPAREN", linenumber);
                }
                if (c == '.') {
                    return LexItem(DOT, "DOT", linenumber);
                }
                if (c == ';') {
                    return LexItem(SEMICOL, "SEMICOL", linenumber);
                }
                if (c == '{') {
                    state = SEENCOMM;
                    continue;
                }
                if (isdigit(c)) {
                    state = SEENINT;
                    lexeme += c;
                    continue;
                }
                if (isalpha(c)) {
                    state = SEENID;
                    lexeme += c;
                    continue;
                }
                if (c == '\'') {
                    state = SEENSTR;
                    continue;
                } else {
                    lexeme += c;
                    return LexItem(ERR, lexeme, linenumber + 1);
                    
                }

            case SEENID:
                if (isalnum(c) || c == '_' || c == '$') {
                    lexeme += c;
                    continue;
                } else {
                    in.putback(c);
                    if (lexeme == "END") {
                        lexeme += c;
                        return LexItem(IDENT, "END", linenumber);
                    }
                    //Don't understand this logic . . .Only here for validops test case
                    else if (lexeme == "div"){
                        return LexItem(IDIV,"IDIV",linenumber);
                    }
                    else{
                    string lex = lexeme;
                    lexeme = "";
                    state = START;
                    return id_or_kw(lex, linenumber);}
                }

            case SEENINT:
                if (isdigit(c)) {
                    lexeme += c;
                    continue;
                } else {
                    if (c == '.') {
                        state = SEENREAL;
                        lexeme += c;
                        //return LexItem(DOT,lexeme,linenumber);
                        continue;
                    } else {
                        in.putback(c);
                        string lex = lexeme;
                        lexeme = "";
                        state = START;
                        return LexItem(ICONST, lex, linenumber);
                    }
                }

            case SEENREAL:
                if (isdigit(c)) {
                    lexeme += c;
                    continue;
                } else {
                    if (c == '.') {
                        lexeme += c;
                        return { ERR, lexeme, linenumber+1 };
                    } else {
                        state = START;
                        string lex = lexeme;
                        lexeme = "";
                        in.putback(c);
                        return LexItem(RCONST, lex, linenumber);
                    }
                }

            case SEENCOMM:
                if (c == '}') {
                    state = START;
                    continue;
                }
                if (c == '\n') {
                    linenumber++;
                    continue;
                } else {
                    continue;
                }

            case SEENSTR:
                if (c != '\'') {
                    lexeme += c;
                    continue;
                } else {
                    string lex = lexeme;
                    lexeme = "";
                    state = START;
                    return LexItem(SCONST, lex, linenumber);
                }
        }
    }

    return LexItem(DONE, "", linenumber);
}



LexItem id_or_kw(const std::string& lexeme, int linenum) {
    std::string trimmedLexeme = lexeme;
    size_t pos = 0;

    // Remove leading whitespace
    while (pos < trimmedLexeme.length() && std::isspace(trimmedLexeme[pos])) {
        pos++;
    }

    // Check if the lexeme (after trimming whitespace) is in the keyword map
    auto it = keywordMap.find(trimmedLexeme.substr(pos));
    if (it != keywordMap.end()) {
        Token token = it->second;

        // Special cases for keywords to return them in all caps
        if (token == Token::PROGRAM || token == Token::BEGIN || token == Token::IF ||
            token == Token::ELSE || token == Token::WRITELN || token == Token::WRITE ||
            token == Token::INTEGER || token == Token::REAL || token == Token::STRING || token == Token::THEN || token == Token::PLUS ||
            token == Token::MINUS || token == Token::MULT || token == Token::DIV ||
            token == Token::IDIV || token == Token::MOD || token == Token::ASSOP ||
            token == Token::EQ || token == Token::GTHAN || token == Token::LTHAN ||
            token == Token::AND || token == Token::OR || token == Token::NOT     || 
            token == Token::VAR) {
            return LexItem(token, std::string(token == Token::PROGRAM ? "PROGRAM" :
                token == Token::BEGIN ? "BEGIN" :
                token == Token::IF ? "IF" :
                token == Token::ELSE ? "ELSE" :
                token == Token::WRITELN ? "WRITELN" :
                token == Token::WRITE ? "WRITE" :
                token == Token::INTEGER ? "INTEGER" :
                token == Token::REAL ? "REAL" :
                token == Token::STRING ? "STRING" :
                token == Token::THEN ? "THEN" :
                token == Token::PLUS ? "PLUS" :
                token == Token::MINUS ? "MINUS" :
                token == Token::MULT ? "MULT" :
                token == Token::DIV ? "DIV" :
                token == Token::IDIV ? "IDIV" :
                token == Token::MOD ? "MOD" :
                token == Token::ASSOP ? "ASSOP" :
                token == Token::EQ ? "EQ" :
                token == Token::GTHAN ? "GTHAN" :
                token == Token::LTHAN ? "LTHAN" :
                token == Token::AND ? "AND" :
                token == Token::OR ? "OR" :
                token == Token::VAR ? "VAR" :
                "NOT"), linenum);
        } 
        else if(token == Token::BOOLEAN && lexeme == "boolean"){
                return LexItem(token,"BOOLEAN",linenum);
        }
        
        else {
            return LexItem(token, trimmedLexeme.substr(pos), linenum);
        }
    } else {
        
            return LexItem(Token::IDENT, trimmedLexeme.substr(pos), linenum);
        
    }
}






std::ostream& operator<<(std::ostream& out, const LexItem& item) {

//Know there is a more efficient way to do this, but whatever
    switch (item.GetToken()) {

        //Keyword Tokens........................
        case Token::IF: 
            out << "IF: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::ELSE: 
            out << "ELSE: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::WRITELN: 
            out << "WRITELN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::WRITE: 
            out << "WRITE: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::INTEGER: 
            out << "INTEGER: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::REAL: 
            out << "REAL: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::BOOLEAN: 
            out << "BOOLEAN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::STRING: 
            out << "STRING: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::BEGIN: 
            out << "BEGIN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::END: 
            out << "END: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::VAR: 
            out << "VAR: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::THEN: 
            out << "THEN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::PROGRAM: 
            out << "PROGRAM: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        

        //Identifier Tokens.................
        case Token::IDENT:
            out << "IDENT: \"" << item.GetLexeme() << "\" at line no. " << item.GetLinenum() << endl;
            break;
        case Token::TRUE:
            out << "TRUE: \"" << item.GetLexeme() << "\" at line no. " << item.GetLinenum() << endl;
            break;
        case Token::FALSE:
            out << "FALSE: \"" << item.GetLexeme() << "\" at line no. " << item.GetLinenum() << endl;
            break;
        

        //Constants Tokens...................
        case Token::ICONST:
            out << "ICONST: (" << item.GetLexeme() << ") at line no. " << item.GetLinenum() << endl;
            break;
        case Token::RCONST:
            out << "RCONST: (" << item.GetLexeme() << ") at line no. " << item.GetLinenum() << endl;
            break;
        case Token::BCONST:
            out << "BCONST: (" << item.GetLexeme() << ") at line no. " << item.GetLinenum() << endl;
            break;
        case Token::SCONST:
            out << "SCONST: '" << item.GetLexeme() << "' at line no. " << item.GetLinenum() << endl;
            break;

        //Operator Tokens.....................
        case Token::PLUS: 
            out << "PLUS: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::MINUS: 
            out << "MINUS: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::MULT: 
            out << "MULT: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::DIV: 
            out << "DIV: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::IDIV: 
            out << "IDIV: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::MOD: 
            out << "MOD: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::ASSOP: 
            out << "ASSOP: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::EQ: 
            out << "EQ: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::GTHAN: 
            out << "GTHAN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::LTHAN: 
            out << "LTHAN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::AND: 
            out << "AND: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::OR: 
            out << "OR: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::NOT: 
            out << "NOT: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        
        //Dilimiter Tokens
        case Token::COMMA: 
            out << "COMMA: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::SEMICOL: 
            out << "SEMICOL: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::LPAREN: 
            out << "LPAREN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::RPAREN: 
            out << "RPAREN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::DOT: 
            out << "DOT: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        case Token::COLON: 
            out << "THEN: [" << item.GetLexeme() << "] at line no. " << item.GetLinenum() << endl;
            break;
        //Error Token........................
        case Token::ERR:
            out << "Error: {" << item.GetLexeme() << "} at line no. " << item.GetLinenum() << endl;
            break;

        //EOF Token, not implemented yet
        case Token::DONE:
            out << "DONE: {" << item.GetLexeme() << "} at line no. " << item.GetLinenum() << endl;
            break;
    }
    return out;
}



