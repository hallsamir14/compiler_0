/* Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
 * parser.cpp
 * Programming Assignment 2
 * Fall 2023
*/

//parser includes one function per syntactic rule/non terminal
/*Lexical analyzer’s error messages should be included as well.
You can still use the same function ParseError() given and
add the lexeme causing the problem.*/

#include <map>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "parser.h"
#include "lex.h"


map<string, bool> defVar;
map<string, Token> SymTable;

//Foward declaration for function declarations not in "parser.h"



//....................................................
namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
        
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}
//namespace Parser ^^^........................................




static int error_count = 0;
int ErrCount()
{
    return error_count;
}
void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}
//error count and handling^^^.........................................



//(EBNF BEGIN)Prog parsing function.........................
bool Prog(istream& in, int& line) {
    LexItem token;

    // Expect the PROGRAM keyword
    token = Parser::GetNextToken(in, line);
    if (token != Token::PROGRAM) {
        ParseError(line, "PROGRAM keyword expected");
        return false;
    }

    // Expect an identifier
    token = Parser::GetNextToken(in, line);
    if (token.GetToken() != Token::IDENT) {
        ParseError(line, "IDENT expected");
        return false;
    }

    // Expect a semicolon
    token = Parser::GetNextToken(in, line);
    if (token != Token::SEMICOL) {
        ParseError(line, "; expected");
        return false;
    }

    // Parse the declaration part
    bool declarationPart = DeclPart(in, line);
    if (!declarationPart) {
        ParseError(line, "Error in declaration part");
        return false;
    }

    // Parse the compound statement
    bool compoundStatement = CompoundStmt(in, line);
    if (!compoundStatement) {
        ParseError(line, "Error in compound statement");
        return false;
    }

    // Successfully parsed the program
    return true;
}


bool DeclPart(istream& in, int& line) {
    LexItem token;

    // Expect the VAR keyword
    token = Parser::GetNextToken(in, line);
    if (token != Token::VAR) {
        ParseError(line, "VAR keyword expected in declaration part.");
        return false;
    }

    // Parse one or more declaration statements separated by semicolons
    bool validDeclarations = DeclStmt(in, line);
    if (!validDeclarations) {
        ParseError(line, "Invalid declaration statement in declaration part");
        return false;
    }

    // Continue to parse additional declaration statements if semicolon is encountered
    while (true) {
        token = Parser::GetNextToken(in, line);
        if (token == Token::SEMICOL) {
            validDeclarations = DeclStmt(in, line);
            if (!validDeclarations) {
                ParseError(line, "Invalid declaration statement in declaration part.");
                return false;
            }
        } else {
            Parser::PushBackToken(token);
            break;
        }
    }

    return validDeclarations;
}//DeclPart



bool DeclStmt(istream& in, int& line) {
    LexItem token;

    // Expect at least one identifier
    token = Parser::GetNextToken(in, line);
    if (token.GetToken() != Token::IDENT) {
        ParseError(line, "IDENT expected in declaration statement");
        return false;
    }
    cout << token << endl;
    // Process additional identifiers, if any
    while (token == Token::COMMA) {
        token = Parser::GetNextToken(in, line);
        cout << token << endl;
        if (token.GetToken() != Token::IDENT) {
            ParseError(line, "IDENT expected after comma in declaration statement");
            return false;
        }
    }

    cout << token << endl;
    // Expect a colon to specify the type
    token = Parser::GetNextToken(in, line);
    if (token != Token::COLON) {
        cout << token << endl;
        ParseError(line, "Colon ':' expected in declaration statement");
        return false;
    }

    // Parse the type
    if (!Type(in, line)) {
        ParseError(line, "Invalid type in declaration statement");
        return false;
    }

    // Check for optional initialization with :=
    token = Parser::GetNextToken(in, line);
    if (token == Token::ASSOP) {
        // Parse the expression for initialization
        if (!Expr(in, line)) {
            ParseError(line, "Invalid expression in declaration statement");
            return false;
        }
    } else {
        // Push back the token if there is no initialization
        Parser::PushBackToken(token);
    }

    return true;
}





bool Type(istream& in, int& line) {
    LexItem t = getNextToken(in, line);
    if (t == INTEGER || t == REAL || t == BOOLEAN || t == STRING) {
        return true; // Successfully matched one of the valid types
    } else {
        ParseError(line, "Invalid type in declaration");
        return false;
    }
}// Type ::= INTEGER | REAL | BOOLEAN | STRING

bool Stmt(istream& in, int& line) {
    LexItem token = Parser::GetNextToken(in, line);

    if (token == Token::IDENT) {
        // Check if it's a simple statement
        if (SimpleStmt(in, line)) {
            return true;  // Successfully parsed a simple statement
        } else {
            // Handle error or return false
            return false;
        }
    } else if (token == Token::BEGIN) {
        // Check if it's a structured statement
        if (StructuredStmt(in, line)) {
            return true;  // Successfully parsed a structured statement
        } else {
            // Handle error or return false
            return false;
        }
    } else {
        // Neither a simple statement nor a structured statement
        Parser::PushBackToken(token); // Push back the token for further processing
        return false;
    }
}//Stmt rule

bool SimpleStmt(istream& in, int& line) {
    LexItem token = Parser::GetNextToken(in, line);

    if (token == Token::IDENT) {
        // It's an assignment statement
        Parser::PushBackToken(token);  // Push back the identifier token
        return AssignStmt(in, line);
    } else if (token == Token::WRITELN) {
        // It's a writeln statement
        return WriteLnStmt(in, line);
    } else if (token == Token::WRITE) {
        // It's a write statement
        return WriteStmt(in, line);
    } else {
        // None of the expected statement types
        Parser::PushBackToken(token);  // Push back the unexpected token
        ParseError(line, "Invalid simple statement");
        return false;  // Return false to indicate a parsing error
    }
}//SimpleStmt rule

bool StructuredStmt(istream& in, int& line) {
    LexItem token = Parser::GetNextToken(in, line);

    if (token == Token::IF) {
        // It's an if statement
        Parser::PushBackToken(token);
        return IfStmt(in, line);
    } else if (token == Token::BEGIN) {
        // It's a compound statement
        Parser::PushBackToken(token);
        return CompoundStmt(in, line);
    } else {
        // None of the expected structured statement types
        Parser::PushBackToken(token);
        ParseError(line, "Invalid structured statement");
        return false;
    }
}//StructuredStmt rule

bool CompoundStmt(istream& in, int& line) {
    LexItem token = Parser::GetNextToken(in, line);

    if (token != Token::BEGIN) {
        ParseError(line, "BEGIN expected");
        return false;
    }

    while (true) {
        token = Parser::GetNextToken(in, line);
        if (!Stmt(in, line)) {
            ParseError(line, "Invalid statement in compound statement");
            return false;
        }

        // Check for a semicolon to see if there's another statement
        token = Parser::GetNextToken(in, line);
        if (token != Token::SEMICOL) {
            Parser::PushBackToken(token);  // Push back token for parsing next statement
            break;  // No semicolon, the compound statement is ending
        }
    }

    if (token != Token::END) {
        ParseError(line, "END expected");
        return false;
    }

    return true;
}//CompoundStmt rule

//WriteLnStmt ::= writeln (ExprList) 
bool WriteLnStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);

    if (t != Token::WRITELN) {
        ParseError(line, "Expected 'writeln' keyword");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    
    if (t != Token::LPAREN) {
        ParseError(line, "Missing Left Parenthesis");
        return false;
    }
    
    bool ex = ExprList(in, line);
    
    if (!ex) {
        ParseError(line, "Missing expression list for WriteLn statement");
        return false;
    }
    
    t = Parser::GetNextToken(in, line);
    
    if (t != Token::RPAREN) {
        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    
    return true;
}//WriteLnStmt


bool WriteStmt(istream& in, int& line) {
    LexItem token = Parser::GetNextToken(in, line);

    if (token != Token::WRITE) {
        ParseError(line, "WRITE expected");
        return false;
    }

    token = Parser::GetNextToken(in, line);

    if (token != Token::LPAREN) {
        ParseError(line, "Left parenthesis '(' expected");
        return false;
    }

    bool ex = ExprList(in, line);

    if (!ex) {
        ParseError(line, "Invalid expression list in WRITE statement");
        return false;
    }

    token = Parser::GetNextToken(in, line);

    if (token != Token::RPAREN) {
        ParseError(line, "Right parenthesis ')' expected");
        return false;
    }

    return true;
}//WriteStmt rule

bool IfStmt(istream& in, int& line) {
    LexItem t;

    // Expect 'IF' keyword
    t = Parser::GetNextToken(in, line);
    if (t != Token::IF) {
        ParseError(line, "Expected 'IF' keyword");
        return false;
    }

    // Parse the expression
    bool ex = Expr(in, line);

    if (!ex) {
        ParseError(line, "Missing expression in IF statement");
        return false;
    }

    // Expect 'THEN' keyword
    t = Parser::GetNextToken(in, line);
    if (t != Token::THEN) {
        ParseError(line, "Expected 'THEN' keyword");
        return false;
    }

    // Parse the statement
    ex = Stmt(in, line);

    if (!ex) {
        ParseError(line, "Missing statement in IF-THEN statement");
        return false;
    }

    // Check for an optional 'ELSE' statement
    t = Parser::GetNextToken(in, line);
    if (t == Token::ELSE) {
        // Parse the 'ELSE' statement
        ex = Stmt(in, line);

        if (!ex) {
            ParseError(line, "Missing statement in IF-ELSE statement");
            return false;
        }
    } else {
        // If there is no 'ELSE', push back the token
        Parser::PushBackToken(t);
    }

    return true;
}//ifStmt rule

bool AssignStmt(istream& in, int& line) {
    LexItem t;

    // Expect a variable (Var)
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != Token::IDENT) {
        ParseError(line, "Expected a variable (IDENT) in assignment statement");
        return false;
    }

    // Expect ':='
    t = Parser::GetNextToken(in, line);
    if (t != Token::ASSOP) {
        ParseError(line, "Expected ':=' in assignment statement");
        return false;
    }

    // Parse the expression (Expr)
    bool ex = Expr(in, line);

    if (!ex) {
        ParseError(line, "Missing expression in assignment statement");
        return false;
    }

    return true;
}//AssignStmt

bool Var(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);

    if (t.GetToken() != Token::IDENT) {
        ParseError(line, "Expected an identifier (IDENT) in variable");
        return false;
    }

    return true;
}//Var rule

bool ExprList(istream& in, int& line) {
	bool status = false;
	//cout << "in ExprList and before calling Expr" << endl;
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		//cout << "before calling ExprList" << endl;
		status = ExprList(in, line);
		//cout << "after calling ExprList" << endl;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//ExprList rule

bool Expr(istream& in, int& line) {
    if (LogOrExpr(in, line)) {
        // Check for optional OR LogAndExpr
        while (true) {
            LexItem t = getNextToken(in, line);
            if (t == OR) {
                if (LogANDExpr(in, line)) {
                    // Continue parsing OR LogAndExpr
                } else {
                    ParseError(line, "Missing right operand for OR");
                    return false;
                }
            } else {
                // Push back the token that was not OR
                Parser::PushBackToken(t);
                break;
            }
        }
        return true;
    }
    return false;
}//Expr Rule

bool LogOrExpr(istream& in, int& line) {
    if (!LogANDExpr(in, line)) {
        return false;
    }

    while (true) {
        LexItem t = Parser::GetNextToken(in, line);
        if (t.GetToken() == Token::OR) {
            if (!LogANDExpr(in, line)) {
                ParseError(line, "Missing right-hand side of OR expression");
                return false;
            }
        } else {
            Parser::PushBackToken(t);
            break;
        }
    }

    return true;
}//LogOrExpr

bool LogANDExpr(istream& in, int& line) {
    if (!RelExpr(in, line)) {
        return false;
    }

    while (true) {
        LexItem t = Parser::GetNextToken(in, line);
        if (t.GetToken() == Token::AND) {
            if (!RelExpr(in, line)) {
                ParseError(line, "Missing right-hand side of AND expression");
                return false;
            }
        } else {
            Parser::PushBackToken(t);
            break;
        }
    }

    return true;
}//LogANDExpr



bool RelExpr(istream& in, int& line) {
    if (!SimpleExpr(in, line)) {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);

    if (t.GetToken() == Token::EQ || t.GetToken() == Token::LTHAN || t.GetToken() == Token::GTHAN) {
        if (!SimpleExpr(in, line)) {
            ParseError(line, "Missing right-hand side of the relational expression");
            return false;
        }
    } else {
        Parser::PushBackToken(t);
    }

    return true;
}//RelExpr rule

bool SimpleExpr(istream& in, int& line) {
    bool leftValid = Term(in, line);

    if (!leftValid) {
        return false;
    }

    LexItem token;
    while (true) {
        token = Parser::GetNextToken(in, line);
        if (token == Token::PLUS || token == Token::MINUS) {
            bool rightValid = Term(in, line);
            if (!rightValid) {
                ParseError(line, "Invalid right-hand side of simple expression");
                return false;
            }
        } else {
            Parser::PushBackToken(token);
            break;
        }
    }

    return true;
}//SimpleExpr rule


bool Term(istream& in, int& line) {
    if (!SFactor(in, line)) {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);

    while (t.GetToken() == Token::MULT || t.GetToken() == Token::DIV || t.GetToken() == Token::DIV || t.GetToken() == Token::MOD) {
        if (!SFactor(in, line)) {
            ParseError(line, "Missing right-hand side of the term");
            return false;
        }

        t = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(t);
    return true;
}//Term rule

bool SFactor(istream& in, int& line) {
    LexItem token = Parser::GetNextToken(in, line);

    // Check for unary operators
    if (token == Token::MINUS || token == Token::PLUS || token == Token::NOT) {
        bool validFactor = Factor(in, line, 1);

        if (validFactor) {
            return true;  // Valid signed factor
        }
    } else {
        Parser::PushBackToken(token);
    }

    // If it's not a unary operator, just proceed with Factor
    return Factor(in, line, 1);
}//Sfactor


bool Factor(istream& in, int& line, int sign) {
    LexItem token = Parser::GetNextToken(in, line);

    // Handle the unary operators if sign is -1
    if (sign == -1) {
        if (token == Token::MINUS) {
            // Handle negation
            return Factor(in, line, -sign);
        } else if (token == Token::PLUS) {
            // Ignore positive sign, continue with the factor
            return Factor(in, line, sign);
        }
    }

    // If the factor is an identifier, constant, or a sub-expression
    if (token == Token::IDENT || token == Token::ICONST || token == Token::RCONST
        || token == Token::SCONST || token == Token::BCONST) {
        return true;  // Valid factor
    } else if (token == Token::LPAREN) {
        // Handle sub-expression
        bool validSubExpr = Expr(in, line);
        if (validSubExpr) {
            // After sub-expression, expect a closing parenthesis
            token = Parser::GetNextToken(in, line);
            if (token == Token::RPAREN) {
                return true;  // Valid sub-expression factor
            }
        }
    }

    // If we reach here, it's an invalid factor
    ParseError(line, "Invalid factor");
    return false;
}//factor rule









