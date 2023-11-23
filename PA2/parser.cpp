#include "parser.h"
#include "lex.h"
//#include "GivenParserPart.cpp"
#include <iostream>

using namespace std;

map<string, bool> defVar;
map<string, Token> SymTable;

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


//WriteLnStmt ::= writeln (ExprList)
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;
    //cout << "in WriteStmt" << endl;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression list for WriteLn statement");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    //Evaluate: print out the list of expressions values

    return ex;
}//End of WriteLnStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status;
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
}


extern bool Prog(istream& in, int& line){
    LexItem t = Parser::GetNextToken(in,line);

    if (t.GetToken()!=PROGRAM){
        ParseError(line,"Program Failed");
        return false;
    }
    t = Parser::GetNextToken(in,line);

    if (t.GetToken()!=IDENT){
        ParseError(line,"Program ident failed");
        //cout<<line<< ": Program ident failed"<<endl;
        return false;
    }
    t = Parser::GetNextToken(in,line);

    if (t.GetToken()!=SEMICOL){
        ParseError(line,"Missing Semicolon");
        //cout<<line<< ": Missing Semicolon"<<endl;
        return false;
    }

    if (!DeclPart(in,line)){
        ParseError(line,"Incorrect Declaration Section.");
         return false;
    }

    if (!CompoundStmt(in,line)){
        ParseError(line,"CompundStmt Failed");
        return false;
    }

    t = Parser::GetNextToken(in,line);

    if (t.GetToken()!=DOT) {
        ParseError(line,"Missing Dot");
        return false;
    }

return true;
}

extern bool DeclPart(istream& in, int& line) {

    LexItem t = Parser::GetNextToken(in,line);

    if (t!=VAR){
        ParseError(line,"Var Missing");
        return false;
    }

    if (!DeclStmt(in,line)){
        ParseError(line,"Syntactic error in Declaration Block.");
        return false;
    }
    t = Parser::GetNextToken(in,line);

    if (t!=SEMICOL){
        ParseError(line,"Semicolon Missing.");
        return false;
    }

    t = Parser::GetNextToken(in,line);

    while (t==IDENT){
        Parser::PushBackToken(t);
        if (!DeclStmt(in,line)){
            ParseError(line,"Syntactic error in Declaration Block.");
            return false;
        }
        t = Parser::GetNextToken(in,line);

        if (t!=SEMICOL){
            cout<<t.GetLexeme()<<endl;
            ParseError(line,"Semolon Missing.");
            return false;
        }
        t = Parser::GetNextToken(in,line);
    }
    Parser::PushBackToken(t);
    return true;
}

extern bool DeclStmt(istream& in, int& line){

    LexItem t = Parser::GetNextToken(in,line);

    if (t!=IDENT){
        ParseError(line,"IDENT Missing.");
        return false;
    }
    if (defVar[t.GetLexeme()]){
        ParseError(line,"Redef Error.");
        return false;
    }

    defVar[t.GetLexeme()]=true;

    t = Parser::GetNextToken(in,line);

    while(t==COMMA){

        t = Parser::GetNextToken(in,line);

        if (t!=IDENT){
            ParseError(line,"IDENT Missing.");
            return false;
        }
        if (defVar[t.GetLexeme()]){
            ParseError(line,"Redef Error.");
            return false;
        }
        defVar[t.GetLexeme()]=true;
        t = Parser::GetNextToken(in,line);
    }

    if (t==IDENT){
        ParseError(line,"Missing Comma.");
        return false;
    }

    if (t!=COLON){
        ParseError(line,"Colon Missing.");
        return false;
    }

    t = Parser::GetNextToken(in,line);

    if (t!=INTEGER && t!=REAL && t!=BOOLEAN && t!=STRING){
        //cout<<t.GetLexeme()<<endl;
        ParseError(line,"Incorrect Declaration Type.");
        return false;
    }

    t = Parser::GetNextToken(in,line);

    if (t==ASSOP){
        if (!Expr(in,line)){
            ParseError(line,"Expr failure");
            return false;
        }
    }
    else{
        Parser::PushBackToken(t);
    }
    return true;
}

extern bool Stmt(istream& in, int& line){

   LexItem t = Parser::GetNextToken(in,line);

    if (t==BEGIN || t == IF){
        Parser::PushBackToken(t);
        if (!StructuredStmt(in,line)){
            ParseError(line,"Structure Statement Failure.");
            return false;
        }
    }
    else{
        if (t == IDENT || t == WRITE || t== WRITELN){
        Parser::PushBackToken(t);
        if (!SimpleStmt(in,line)){
            ParseError(line,"Structure Statement Failure.");
            return false;
        }
        }

    }
    return true;
}

extern bool SimpleStmt(istream& in, int& line){

    LexItem t = Parser::GetNextToken(in,line);

    if (t==IDENT){
        Parser::PushBackToken(t);
        if (!AssignStmt(in,line)){
            ParseError(line,"Assign Statement Failure.");
            return false;
        }
    }
    else{
        if (t==WRITE){
            //Parser::PushBackToken(t);
            if (!WriteStmt(in,line)){
                ParseError(line,"Write Statement Failure.");
                return false;
            }
        }
        else{
            if (t==WRITELN){
                //Parser::PushBackToken(t);
                if (!WriteLnStmt(in,line)){
                    ParseError(line,"WriteLn Statement Failure.");
                    return false;
                }
            }
            else{
                ParseError(line,"Shouldn't be here.");
                return false;
            }
        }
    }
    return true;
}

extern bool StructuredStmt(istream& in, int& line){

    LexItem t = Parser::GetNextToken(in,line);

    if (t==IF){
        Parser::PushBackToken(t);
        if (!IfStmt(in,line)){
            ParseError(line,"If Statement Failure.");
            return false;
        }
    }
    else{
        if (t==BEGIN){
            Parser::PushBackToken(t);
            if (!CompoundStmt(in,line)){
                ParseError(line,"Compound Statement Failure.");
                return false;
            }
        }
    }
    return true;

}

extern bool CompoundStmt(istream& in, int& line) {

    LexItem t = Parser::GetNextToken(in,line);

    if (t!=BEGIN){
        ParseError(line,"Missing Begin in CMPNDSTMT.");
        return false;
    }

    if (!Stmt(in,line)){
        ParseError(line,"STMT Failure.");
        return false;
    }
    t = Parser::GetNextToken(in,line);

    while (t==SEMICOL){
        if (!Stmt(in,line)){
            ParseError(line,"STMT Failure.");
            return false;
        }
        t = Parser::GetNextToken(in,line);
    }

    if (t!=END){
        ParseError(line,"END Missing.");
        return false;
    }

    return true;
}

extern bool WriteStmt(istream& in, int& line) {
    LexItem t;
    //cout << "in WriteStmt" << endl;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression list for WriteLn statement");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    //Evaluate: print out the list of expressions values

    return ex;
}

extern bool IfStmt(istream& in, int& line) {

    LexItem t = Parser::GetNextToken(in, line);

    if (t!=IF){
        ParseError(line, "Missing IF.");
        return false;
    }
  //  t = Parser::GetNextToken(in, line);
    if (!Expr(in,line)){
        ParseError(line, "EXPR Failure.");
        return false;
    }

    t = Parser::GetNextToken(in, line);

    if (t!=THEN){
        ParseError(line, "THEN Missing.");
        return false;
    }

    if (!Stmt(in,line)){
        ParseError(line, "STMT Failure.");
        return false;
    }

    t = Parser::GetNextToken(in, line);

    if (t==ELSE){
        if (!Stmt(in,line)) {
            ParseError(line, "STMT Failure.");
            return false;
        }
    }
    else{
        Parser::PushBackToken(t);
    }
return true;
}

extern bool AssignStmt(istream& in, int& line){
    LexItem t = Parser::GetNextToken(in, line);
    if (t!=IDENT){
       // cout<<"Curr lex: "<<t.GetLexeme()<<endl;
        ParseError(line, "IDENT Missing Failure.");
        return false;
    }
    if (!defVar[t.GetLexeme()]){
        ParseError(line, "Undec Var Failure.");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t!=ASSOP){
        ParseError(line, "ASSOP Missing Failure.");
        return false;
    }
    if (!Expr(in,line)){
        ParseError(line, "EXPR Failure.");
        return false;
    }
    return true;
}

extern bool Expr(istream& in, int& line){

    if (!LogANDExpr(in,line)){
        ParseError(line, "LogANDExpr Failure.");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);

    while (t==OR){
        if (!LogANDExpr(in,line)){
            ParseError(line, "LogANDExpr Failure.");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

extern bool LogANDExpr(istream& in, int& line){

    if (!RelExpr(in,line)){
        ParseError(line, "RelExpr Failure.");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);

    while (t==AND){
        if (!RelExpr(in,line)){
            ParseError(line, "RelExpr Failure.");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

extern bool RelExpr(istream& in, int& line){
    if (!SimpleExpr(in,line)){
        ParseError(line, "SimpleExpr Failure.");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);

    if (t==EQ || t==GTHAN || t==LTHAN){
        if (!SimpleExpr(in,line)){
            ParseError(line, "SimpleExpr failure.");
            return false;
        }
    }
    else{
        Parser::PushBackToken(t);
    }
    return true;

}

extern bool SimpleExpr(istream& in, int& line){
    if (!Term(in,line)){
        ParseError(line, "Term failed.");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);

    while (t == PLUS || t==MINUS){
        if (!Term(in,line)){
            ParseError(line, "Term fail.");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

extern bool Term(istream& in, int& line){
    if (!SFactor(in,line)){
        ParseError(line, "SFctor failure.");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    while (t==MULT || t ==DIV || t==IDIV || t==MOD){
        if (!SFactor(in,line)){
            ParseError(line, "SFactor failure.");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

extern bool SFactor(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t == PLUS || t == MINUS || t == NOT) {
        if (!Factor(in, line, 0)) {
            ParseError(line, "Factor failure.");
            return false;
        }

    }
    else{
        Parser::PushBackToken(t);
        if (!Factor(in, line, 0)) {
            ParseError(line, "Factor failure.");
            return false;
        }
    }
    return true;
}


extern bool Factor(istream& in, int& line, int sign) {
    LexItem t = Parser::GetNextToken(in, line);

    if (t==LPAREN){
        if (!Expr(in,line)){
            ParseError(line, "Expr failure.");
            return false;
        }
        t= Parser::GetNextToken(in,line);
        if (t!= RPAREN){
            ParseError(line, "Missing Rpar failure.");
            return false;
        }
        return true;
    }
    else{
        if (t!=IDENT && t!=ICONST && t!=RCONST && t!= SCONST && t!=BCONST){
            ParseError(line, "idk error failure.");
            return false;
        }
    }
    return true;
}