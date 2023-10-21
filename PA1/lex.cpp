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
using namespace std;
using std::map;

/*
functions to be implemented - - - - - - - - - - - -
ostream& operator<< (ostream& out, constLexItem& tok);
LexItem id_or_kw(const string& lexeme, int linenum);
Lexitem getNextToken(istream& in, int& linenum);
*/

//getNextToken function will need to look at the next character from input
//use peek() function to examine next character, only read if it belongs to token
LexItem getNextToken(istream& in, int& linenumber){

enum tokenState{START,INID,ININT,INSTRING,INREAL,COMMENT,PLUS,DIV,ERR,DONE} lexstate = START;
    
string lexeme;
char ch;
LexItem token;
//each state has different rules
while(in.get(ch)){
    switch (lexstate){
        //Start state logic branches
        case START:
            if (isalpha(ch)){
                lexstate = INID;
                lexeme += ch;
            }
            else if (isdigit(ch)){
                lexstate = ININT;
                lexeme += ch;
            }
            else if (ch == '"'){
                lexstate = INSTRING;
                lexeme += ch;
            }
            else if (ch == '/'){
                char ch_next = in.peek();
                if(ch_next=='/'){
                    lexstate = COMMENT;
                }
                else{
                    lexstate = ERR;
                    lexeme = "/";
                    token = LexItem(Token::ERR, lexeme, linenumber);
            }
    }
            else if (ch='\n'){
                linenumber++;
}
            else if(in.eof()){
                lexstate = DONE;
                token = LexItem(Token::DONE,"",linenumber);

            }
            break;

            //Identifier logic ...
            case INID:
                if (isalnum(ch) || ch=='_'){
                    lexeme += ch;
                }
                else {
                    in.putback(ch);
                    lexstate = DONE;
                }
                break;

            //Int logic
            case ININT:
                if(isdigit(ch)){
                    lexeme += ch;
                }    
                else if(ch=='.'){
                    lexeme += ch;
                    lexstate = INREAL;
                }
                else {
                    in.putback(ch);
                    lexstate = DONE;
                    //determine token state on lexeme, set token variable
                }
            
            //String state logic
            case INSTRING:
                lexeme += ch;
                if(ch == '"'){
                    lexstate = DONE;
                    token = LexItem(Token::STRING,lexeme,linenumber);
                }
                else if (ch == '\n'){
                    lexstate = ERR;
                }
                break;



            


    


}

    
}
    return token;
}

std::map<string, Token> keywordMap = {
        {"PROGRAM", PROGRAM},
		{"WRITELN", WRITELN},
		{"WRITE", WRITE },
		{ "IF", IF },
		{ "ELSE", ELSE },	
		{ "IDENT", IDENT },
		{"VAR", VAR},
		{"BEGIN", BEGIN},
		{"END", END},
		{"INTEGER", INTEGER},
		{"REAL", REAL},
		{"STRING", STRING},
		{"BOOLEAN", BOOLEAN},
		{"TRUE", TRUE},
		{"FALSE", FALSE},
		
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

};

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
        return LexItem(it->second, trimmedLexeme.substr(pos), linenum);
    } else {
        // If not found in the map, check if it's "END"
        if (trimmedLexeme.substr(pos) == "end") {
            return LexItem(Token::END, trimmedLexeme.substr(pos), linenum);
        } else {
            // If it's not "END" and not found in the map, it's an IDENT
            return LexItem(Token::IDENT, trimmedLexeme.substr(pos), linenum);
        }
    }
}