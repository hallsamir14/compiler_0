/* Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
 * parser.cpp
 * Programming Assignment 2
 * Fall 2023
*/

#include "parserInterp.h"
#include "lex.cpp"

map<string, Value> ValMap;
map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults;//Container of temporary locations of Value objects for results of expressions, variables values and constants
queue<Value> * ValQue;//declare a pointer variable to a queue of Value objects

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

bool IdentList(istream& in, int& line);


//Program is: Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
bool Prog(istream& in, int& line)
{
	bool f1, f2;
	LexItem tok = Parser::GetNextToken(in, line);
	
	
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			string identstr = tok.GetLexeme();
			if (!(defVar.find(identstr)->second))
			{
				defVar[identstr] = true;
				
			}
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL) {
				f1 = DeclPart(in, line); 
			
				if(f1) {
					tok = Parser::GetNextToken(in, line);
					if(tok == BEGIN)
					{
						f2 = CompoundStmt(in, line);
						if(!f2)
						{
							ParseError(line, "Incorrect Program Body.");
							return false;
						}
						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == DOT)
							return true;//Successful Parsing is completed
						else
						{
							ParseError(line, "Missing end of program dot.");
							return false;
						}
					}
					else
					{
						ParseError(line, "Missing program body.");
						return false;
					}
					
				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{
				
				ParseError(line-1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM Keyword.");
	return false;
}//End of Prog

//DeclPart ::= VAR DeclStmt { ; DeclStmt }
bool DeclPart(istream& in, int& line) {
	bool status = false;
	LexItem tok;

	LexItem t = Parser::GetNextToken(in, line);
	if(t == VAR)
	{
		status = DeclStmt(in, line);
		
		while(status)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				
				ParseError(line, "Missing semicolon in Declaration Statement.");
				return false;
			}
			status = DeclStmt(in, line);
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN )
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else 
		{
			ParseError(line, "Syntactic error in Declaration Block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Part.");
		return false;
	}
	
}//end of DeclBlock function


bool DeclStmt(istream& in, int& line)
{
    LexItem t;
    bool status = IdentList(in, line);
    bool flag;

    if (!status)
    {
        ParseError(line, "Incorrect identifiers list in Declaration Statement.");
        return status;
    }


    t = Parser::GetNextToken(in, line);
    if(t == COLON)
    {
        t = Parser::GetNextToken(in, line);
        if(t == INTEGER || t == REAL || t == STRING || t == BOOLEAN)
        {
            map<string, Value>::iterator it;

            if (t == INTEGER){
                for(it=TempsResults.begin(); it!=TempsResults.end(); it++){
                    SymTable[it->first] = INTEGER;
                }
            }
            if (t == REAL){
                for(it=TempsResults.begin(); it!=TempsResults.end(); it++){
                    SymTable[it->first] = REAL;
                }
            }
            if (t == STRING){
                for(it=TempsResults.begin(); it!=TempsResults.end(); it++){
                    SymTable[it->first] = STRING;
                }
            }
            if (t == BOOLEAN){
                for(it=TempsResults.begin(); it!=TempsResults.end(); it++){
                    SymTable[it->first] = BOOLEAN;
                }
            }

            t = Parser::GetNextToken(in, line);
            if(t == ASSOP)
            {
                Value v;
                flag = Expr(in, line, v);
                if(!flag)
                {
                    ParseError(line, "Incorrect initialization expression.");
                    return false;
                }
                for(it=TempsResults.begin(); it!=TempsResults.end(); it++){
                    ValMap[it->first] = v;
                }
                TempsResults.clear();
            }
            else
            {
                Parser::PushBackToken(t);
                TempsResults.clear();
            }
            return true;
        }
        else
        {
            ParseError(line, "Incorrect Declaration Type.");
            return false;
        }
    }
    else
    {
        Parser::PushBackToken(t);

        return false;
    }

}//End of DeclStmt

//IdList:= IDENT {,IDENT}
bool IdentList(istream& in, int& line) {
    bool status;
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);
    if(tok == IDENT)
    {
        //set IDENT lexeme to the type tok value
        identstr = tok.GetLexeme();
        if (!(defVar[identstr]))
        {
            defVar[identstr] = true;
            TempsResults[tok.GetLexeme()] = Value();
        }
        else
        {
            cout<<identstr<<endl;
            ParseError(line, "Variable Redefinition");
            return false;
        }

    }
    else
    {
        Parser::PushBackToken(tok);

        return true;
    }

    tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        status = IdentList(in, line);
    }

    else if(tok == COLON)
    {
        Parser::PushBackToken(tok);
        return true;
    }
    else if(tok == IDENT)
    {
        ParseError(line, "Missing comma in declaration statement.");
        return false;
    }
    else {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return status;
}//End of IdentList


//Stmt is either a simple statement or a structured one
//Stmt ::= SimpleStmt | StructuredStmt
bool Stmt(istream& in, int& line) {
    bool status = false;
    //cout << "in ContrlStmt" << endl;
    LexItem t = Parser::GetNextToken(in, line);
    if(t == WRITELN || t == WRITE || t == IDENT)
    {
        Parser::PushBackToken(t);
        status = SimpleStmt(in, line);
        if (!status)
        {
            ParseError(line, "Incorrect Simple Statement.");
            return status;
        }
        return status;
    }
    else if( t == IF || t == BEGIN)
    {
        Parser::PushBackToken(t);
        status = StructuredStmt(in, line);
        if (!status)
        {
            ParseError(line, "Incorrect Structured Statement.");
            return status;
        }
        return status;
    }
    else if(t == END)
    {
        cout << "in stmt: " << t.GetLexeme() << endl;
        Parser::PushBackToken(t);
        return false;
    }
    return status;
}//End of Stmt

//StructuredStmt ::= IfStmt | CompoundStmt
bool StructuredStmt(istream& in, int& line) {
    bool status;

    LexItem t = Parser::GetNextToken(in, line);
    switch( t.GetToken() ) {

        case IF: //Keyword consumed
            status = IfStmt(in, line);

            break;

        case BEGIN: //Keyword consumed
            status = CompoundStmt(in, line);
            break;

        default:
            ;
    }

    return status;
}//End of StructuredStmt


//CompoundStmt ::= BEGIN Stmt {; Stmt } END
bool CompoundStmt(istream& in, int& line) {
    bool status;
    LexItem tok;
    status = Stmt(in, line);
    tok = Parser::GetNextToken(in, line);
    while(status && tok == SEMICOL)
    {
        status = Stmt(in, line);
        tok = Parser::GetNextToken(in, line);
    }
    if(!status)
    {
        ParseError(line, "Syntactic error in the statement.");
        Parser::PushBackToken(tok);
        return false;
    }
    if(tok == END)
        return true;
    else if(tok == ELSE)
    {
        Parser::PushBackToken(tok);
        return true;
    }
    else
    {

        ParseError(line, "Missing end of compound statement.");
        return false;
    }
}//end of CompoundStmt



//SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
bool SimpleStmt(istream& in, int& line) {
    bool status;

    LexItem t = Parser::GetNextToken(in, line);

    switch( t.GetToken() ) {

        case WRITELN: //Keyword is consumed
            status = WriteLnStmt(in, line);

            break;

        case WRITE: //Keyword is consumed
            status = WriteStmt(in, line);
            break;

        case IDENT: //Keyword is not consumed
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);

            break;
        default:
            ;
    }

    return status;
}//End of SimpleStmt


//WriteLnStmt ::= writeln (ExprList)
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;


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
    cout<<endl;
    return ex;
}//End of WriteLnStmt

//WriteStmt ::= write (ExprList)
bool WriteStmt(istream& in, int& line) {
    LexItem t;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression list for Write statement");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    return ex;
}//End of WriteStmt

//IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
bool IfStmt(istream& in, int& line) {
    bool ex=false, status ;
    LexItem t;
    Value v;
    ex = Expr(in, line, v);
    if( !ex ) {
        ParseError(line, "Missing if statement Logic Expression");
        return false;
    }
    if (!v.IsBool()){
        ParseError(line, "Need bool for if stmt");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != THEN)
    {
        ParseError(line, "If-Stmt Syntax Error");
        return false;
    }
    bool doElse = false;
    if (v.GetBool()){
        status = Stmt(in, line);
    }
    else{
        status = true;
        doElse = true;
        while (t!= END){
            t = Parser::GetNextToken(in,line);
        }
    }
    if(!status)
    {
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if( t == ELSE ) {
        if (doElse){
            status = Stmt(in, line);
        }
        else{
            status = true;
            while (t!=END){
                t = Parser::GetNextToken(in,line);
            }
        }

        if(!status)
        {
            ParseError(line, "Missing Statement for If-Stmt Else-Part");
            return false;
        }
        return true;
    }

    Parser::PushBackToken(t);// semicolon pushed back or anything else
    return true;
}//End of IfStmt function

//Var ::= ident
bool Var(istream& in, int& line, LexItem & idtok)
{
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == IDENT){
        identstr = tok.GetLexeme();

        if (!(defVar.find(identstr)->second))
        {
            ParseError(line, "Undeclared Variable");
            return false;
        }
        idtok = tok;
        return true;
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return false;
}//End of Var

//AssignStmt:= Var := Expr
bool AssignStmt(istream& in, int& line) {
    bool varstatus = false, status = false;
    LexItem t;
    LexItem tok;
    varstatus = Var( in, line, tok);

    if (varstatus){
        t = Parser::GetNextToken(in, line);

        if (t == ASSOP){
            Value v;
            status = Expr(in, line,v);
            if(!status) {
                ParseError(line, "Missing Expression in Assignment Statement");
                return status;
            }
            if ( (SymTable[tok.GetLexeme()]==BOOLEAN && v.IsBool() ) ||
                 ( SymTable[tok.GetLexeme()]==STRING && v.IsString() ) ||
                 ( SymTable[tok.GetLexeme()]==INTEGER && v.IsInt() ) ||
                 ( SymTable[tok.GetLexeme()]==REAL && v.IsReal() ) )
            {
                ValMap[tok.GetLexeme()] = v;
            }
            else{
                if (SymTable[tok.GetLexeme()]==INTEGER && v.IsReal()){
                    ValMap[tok.GetLexeme()] = (int) v.GetReal();
                }
                else{
                    if (SymTable[tok.GetLexeme()]==REAL && v.IsInt()){
                        ValMap[tok.GetLexeme()] = (double) v.GetInt();
                    }
                    else{
                        //cout<<tok.GetLexeme()<<endl;
                        ParseError(line, "Incorrect assignment :(");
                        return false;
                    }
                }

            }

        }
        else if(t.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        }
        else {
            ParseError(line, "Missing Assignment Operator");
            return false;
        }
    }
    else {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return status;
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status = false;
    Value v;
    status = Expr(in, line, v);
    if(!status){
        ParseError(line, "Missing Expression");
        return false;
    }

    cout<<v;

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {

        status = ExprList(in, line);

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
}//ExprList

//Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream& in, int& line, Value & retVal) {
    //LogORExpr
    LexItem tok;
    Value v;
    bool t1 = LogANDExpr(in, line,v);

    if( !t1 ) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while ( tok == OR )
    {
        Value v1;
        t1 = LogANDExpr(in, line, v1);
        if( !t1 )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }


        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
        v = v || v1;

    }

    retVal = v;
    Parser::PushBackToken(tok);
    return true;
}//End of Expr/LogORExpr

//LogAndExpr ::= RelExpr {AND RelExpr }
bool LogANDExpr(istream& in, int& line, Value & retVal) {
    LexItem tok;
    Value v;
    bool t1 = RelExpr(in, line, v);

    if( !t1 ) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while ( tok == AND )
    {
        Value v1;
        t1 = RelExpr(in, line, v1);
        if( !t1 )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
        v = v && v1;

    }

    Parser::PushBackToken(tok);
    retVal = v;
    return true;
}//End of LogANDExpr

//RelExpr ::= SimpleExpr  [ ( = | < | > ) SimpleExpr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
    LexItem tok;
    Value v;
    bool t1 = SimpleExpr(in, line, v);

    if( !t1 ) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    if ( tok == LTHAN || tok == GTHAN || tok == EQ)
    {
        LexItem op = tok;
        Value v1;
        t1 = SimpleExpr(in, line, v1);
        if( !t1 )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        if (!v1.IsInt() && !v1.IsReal()){
            ParseError(line, "Comparing int and float values");
            return false;
        }
        if (op == LTHAN){
            v = (v<v1);
        }
        if (op == GTHAN){
            v = (v>v1);
        }
        if (op == EQ){
            v = (v==v1);
        }

        tok = Parser::GetNextToken(in, line);

        if(tok == LTHAN || tok == GTHAN || tok == EQ)
        {
            ParseError(line, "Illegal Comparison Expression.");
            return false;
        }
        else if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    retVal = v;
    Parser::PushBackToken(tok);
    return true;
}//End of RelExpr

//Expr:= Term {(+|-) Term}
bool SimpleExpr(istream& in, int& line, Value & retVal) {
    Value v;
    bool t1 = Term(in, line, v);
    LexItem tok;

    if( !t1 ) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while ( tok == PLUS || tok == MINUS )
    {
        Value v1;
        LexItem op = tok;
        t1 = Term(in, line, v1);
        if( !t1 )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        if (op == PLUS){
            v = v + v1;
        }
        if (op == MINUS){
            v = v - v1;
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    retVal = v;
    Parser::PushBackToken(tok);
    return true;
}//End of Expr

//Term:= SFactor {( * | / | DIV | MOD) SFactor}
bool Term(istream& in, int& line, Value & retVal) {
    Value v;
    bool t1 = SFactor(in, line,v);
    LexItem tok;

    if( !t1 ) {
        return false;
    }

    tok	= Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while ( tok == MULT || tok == DIV  || tok == IDIV || tok == MOD)
    {
        Value v1;
        LexItem op = tok;
        t1 = SFactor(in, line,v1);

        if( !t1 ) {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        if (op == MULT){
            v = v * v1;
        }
        if (op == DIV){
            if (v1.IsReal() && v1.GetReal()==0.0){
                ParseError(line, "Divide by zero");
                return false;
            }
            if (v1.IsInt() && v1.GetInt()==0){
                ParseError(line, "Divide by zero");
                return false;
            }
            v = v / v1;
        }
        if (op == IDIV){
            if (v1.IsReal() && v1.GetReal()==0.0){
                ParseError(line, "Divide by zero ");
                return false;
            }
            if (v1.IsInt() && v1.GetInt()==0){
                ParseError(line, "Divide by zero");
                return false;
            }
            v = v / v1;
        }
        if (op == MOD){
            v = v % v1;
        }

        tok	= Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    retVal = v;
    Parser::PushBackToken(tok);
    return true;
}//End of Term

//SFactor ::= [( - | + | NOT )] Factor
bool SFactor(istream& in, int& line, Value & retVal)
{
    LexItem t = Parser::GetNextToken(in, line);
    bool status;
    int sign = 0;
    if(t == MINUS )
    {
        sign = -1;
    }
    else if(t == PLUS)
    {
        sign = 1;
    }
    else if(t == NOT)
    {
        sign = -2;//sign is a NOT op for logic operands
    }
    else
        Parser::PushBackToken(t);
    Value v;
    status = Factor(in, line, v, sign);
    if (sign == -2 && !v.IsBool()){
        ParseError(line, "Illegal operand");
        return false;
    }
    if (sign ==-1 && (!v.IsReal() && !v.IsInt())){
        ParseError(line, "Illegal operand");
        return false;
    }
    if (sign ==-1 && v.IsReal()){
        v.SetReal( -1 * v.GetReal());
    }
    if (sign ==-1 && v.IsInt()){
        v.SetInt( -1 * v.GetInt());
    }
    retVal = v;
    return status;
}//End of SFactor

//Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream& in, int& line, Value & retVal,int sign) {

    LexItem tok = Parser::GetNextToken(in, line);


    if( tok == IDENT ) {
        string lexeme = tok.GetLexeme();
        if (!(defVar.find(lexeme)->second))
        {
            ParseError(line, "Using Undefined Variable");
            return false;
        }
        if (ValMap.find(lexeme)==ValMap.end()){
            ParseError(line, "Using Uninitialized Variable");
            return false;
        }

        retVal = ValMap[tok.GetLexeme()];

        return true;
    }
    else if( tok == ICONST ) {
        retVal.SetType(VINT);
        retVal.SetInt(stoi(tok.GetLexeme()));
        return true;
    }
    else if( tok == SCONST ) {
        retVal.SetType(VSTRING);
        retVal.SetString(tok.GetLexeme());
        return true;
    }
    else if( tok == RCONST ) {
        retVal.SetType(VREAL);
        retVal.SetReal(stod(tok.GetLexeme()));
        return true;
    }
    else if( tok == BCONST ) {
        retVal.SetType(VBOOL);
        retVal.SetBool(tok.GetLexeme()=="true");
        return true;
    }
    else if( tok == LPAREN ) {
        Value v;
        bool ex = Expr(in, line,v);
        if( !ex ) {
            ParseError(line, "Missing expression after (");
            return false;
        }
        if( Parser::GetNextToken(in, line) == RPAREN ){
            retVal = v;
            return ex;
        }
        else
        {
            Parser::PushBackToken(tok);
            ParseError(line, "Missing ) after expression");
            return false;
        }
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    Parser::PushBackToken(tok);

    return false;
}






