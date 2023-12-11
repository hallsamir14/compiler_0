#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <sstream>

using namespace std;

enum ValType { VINT, VREAL, VSTRING, VBOOL, VERR };

class Value {
    ValType	T;
    bool    Btemp;
    int 	Itemp;
	double   Rtemp;
    string	Stemp;
    
       
public:
    Value() : T(VERR), Btemp(false), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(bool vb) : T(VBOOL), Btemp(vb), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(int vi) : T(VINT), Btemp(false), Itemp(vi), Rtemp(0.0), Stemp("") {}
    Value(double vr) : T(VREAL), Btemp(false), Itemp(0), Rtemp(vr), Stemp("") {}
    Value(string vs) : T(VSTRING), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(vs) {}
    
    
    ValType GetType() const { return T; }
    bool IsErr() const { return T == VERR; }
    bool IsString() const { return T == VSTRING; }
    bool IsReal() const {return T == VREAL;}
    bool IsBool() const {return T == VBOOL;}
    bool IsInt() const { return T == VINT; }
    
    int GetInt() const { if( IsInt() ) return Itemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    string GetString() const { if( IsString() ) return Stemp; throw "RUNTIME ERROR: Value not a string"; }
    
    double GetReal() const { if( IsReal() ) return Rtemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    bool GetBool() const {if(IsBool()) return Btemp; throw "RUNTIME ERROR: Value not a boolean";}
    
    void SetType(ValType type)
    {
    	T = type;
	}
	
	void SetInt(int val)
    {
    	Itemp = val;
	}
	
	void SetReal(double val)
    {
    	Rtemp = val;
	}
	
	void SetString(string val)
    {
    	Stemp = val;
	}
	
	void SetBool(bool val)
    {
    	Btemp = val;
	}
	
	
    // numeric overloaded add op to this
Value operator+(const Value& op) const {
    if (T == VINT && op.T == VINT) {
        return Value(Itemp + op.Itemp);
    } else if (T == VREAL && op.T == VREAL) {
        return Value(Rtemp + op.Rtemp);
    } else if ((T == VINT && op.T == VREAL) || (T == VREAL && op.T == VINT)) {
        return Value(GetReal() + op.GetReal());
    } else if (T == VSTRING && op.T == VSTRING) {
        return Value(Stemp + op.Stemp);
    } else {
        //cerr << "Invalid operation: Addition supports only numeric types or string concatenation." << endl;
        return Value(); // Return an invalid Value
    }
}
    
    // numeric overloaded subtract op from this
    Value operator-(const Value& op) const {
    if (T == VINT && op.T == VINT) {
        return Value(Itemp - op.Itemp);
    } else if (T == VREAL && op.T == VREAL) {
        return Value(Rtemp - op.Rtemp);
    } else if ((T == VINT && op.T == VREAL) || (T == VREAL && op.T == VINT)) {
        return Value(GetReal() - op.GetReal());
    } else {
        //cerr << "Invalid operation: Subtraction supports only numeric types." << endl;
        return Value(); // Return an invalid Value
    }
}
    
    // numeric overloaded multiply this by op
    Value operator*(const Value& op) const {
    if (T == VINT && op.T == VINT) {
        return Value(Itemp * op.Itemp);
    } else if (T == VREAL && op.T == VREAL) {
        return Value(Rtemp * op.Rtemp);
    } else if ((T == VINT && op.T == VREAL) || (T == VREAL && op.T == VINT)) {
        return Value(GetReal() * op.GetReal());
    } else {
        cerr << "Invalid operation: Multiplication supports only numeric types." << endl;
        return Value(); // Return an invalid Value
    }
}
    
    // numeric overloaded divide this by op
    Value operator/(const Value& op) const {
    if (T == VINT && op.T == VINT) {
        if (op.Itemp == 0) {
            //cerr << "Division by zero error." << endl;
            return Value(); // Return an invalid Value
        }
        return Value(Itemp / op.Itemp);
    } else if (T == VREAL && op.T == VREAL) {
        if (op.Rtemp == 0.0) {
            //cerr << "Division by zero error." << endl;
            return Value(); // Return an invalid Value
        }
        return Value(Rtemp / op.Rtemp);
    } else if ((T == VINT && op.T == VREAL) ||
               (T == VREAL && op.T == VINT)) {
        if (op.T == VINT && op.Itemp == 0) {
            //cerr << "Division by zero error." << endl;
            return Value(); // Return an invalid Value
        }
        return Value(T == VINT ? Itemp / op.Rtemp : Rtemp / op.Itemp);
    } else {
        //cerr << "Invalid operation: Division not supported for non-numeric types." << endl;
        return Value(); // Return an invalid Value
    }
}
    
    // numeric overloaded modulus this by op
    Value operator%(const Value& op) const {
    if (T == VINT && op.T == VINT) {
        if (op.Itemp == 0) {
            //cerr << "Modulus by zero error." << endl;
            return Value(); // Return an invalid Value
        }
        return Value(Itemp % op.Itemp);
    } else {
        //cerr << "Invalid operation: Modulus operation requires integer operands." << endl;
        return Value(); // Return an invalid Value
    }
}
    
    
    
    
    //overloaded equality operator of this with op
   Value operator==(const Value& op) const {
    if (T != op.T) {
        return Value(false); // Different types, return false
    }

    if (T == VINT) {
        if (op.T == VREAL) {
            return Value(false); // Comparison between int and real, return false
        }
        return Value(Itemp == op.Itemp);
    } else if (T == VREAL) {
        if (op.T == VINT) {
            return Value(false); // Comparison between real and int, return false
        }
        return Value(fabs(Rtemp - op.Rtemp) < 0.0001); // Check for approximate equality for doubles
    } else if (T == VSTRING) {
        return Value(Stemp == op.Stemp);
    } else if (T == VBOOL) {
        return Value(Btemp == op.Btemp);
    } else {
        return Value(false); // Return false for unsupported types
    }
}


	//overloaded greater than operator of this with op
    Value operator>(const Value& op) const {
    if ((T == VINT && op.T == VINT) ||
        (T == VREAL && op.T == VREAL)) {
        return Value(T == VINT ? Itemp > op.Itemp : Rtemp > op.Rtemp);
    } else {
        //cerr << "Invalid operation: Greater than comparison supports only numeric types." << endl;
        return Value(); // Return an invalid Value
    }
}

	//overloaded less than operator of this with op
	Value operator<(const Value& op) const {
    if (T == VINT && op.T == VINT) {
        return Value(Itemp < op.Itemp);
    } else if (T == VREAL && op.T == VREAL) {
        return Value(Rtemp < op.Rtemp);
    } else if ((T == VINT && op.T == VREAL) || (T == VREAL && op.T == VINT)) {
        return Value(GetReal() < op.GetReal());
    } else if (T == VSTRING && op.T == VSTRING) {
        return Value(Stemp < op.Stemp);
    } else {
        //cerr << "Invalid operation: Comparison supports only numeric types or string comparison." << endl;
        return Value(); // Return an invalid Value
    }
}
	
	//integer divide operator of this by op
	Value div(const Value& op) const {
    if (T == VINT && op.T == VINT) {
        if (op.Itemp == 0) {
            //cerr << "Division by zero error." << endl;
            return Value(); // Return an invalid Value
        }
        return Value(Itemp / op.Itemp);
    } else {
        //cerr << "Invalid operation: Integer division supports only integer types." << endl;
        return Value(); // Return an invalid Value
    }
}
	
	//Logic operations  
	//Overloaded Anding operator
    Value operator&&(const Value& op) const {
    if (T == VBOOL && op.T == VBOOL) {
        return Value(Btemp && op.Btemp);
    } else {
        //cerr << "Invalid operation: Logical AND requires boolean operands." << endl;
        return Value(); // Return an invalid Value
    }
}
    //Overloaded Oring operator
    Value operator||(const Value& oper) const {
    if (T == VBOOL && oper.T == VBOOL) {
        return Value(GetBool() || oper.GetBool());
    } else {
        //cerr << "Invalid operation: Logical OR supports only boolean types." << endl;
        return Value(); // Return an invalid Value
    }
}

	//Overloaded Not/complement operator
    Value operator!() const {
    if (T == VBOOL) {
        return Value(!GetBool());
    } else {
        //cerr << "Invalid operation: Logical NOT supports only boolean types." << endl;
        return Value(); // Return an invalid Value
    }
}

	
	
	    
    friend ostream& operator<<(ostream& out, const Value& op) {
        if( op.IsInt() ) out << op.Itemp;
		else if( op.IsString() ) out << "\"" << op.Stemp << "\"";
        else if( op.IsReal()) out << fixed << showpoint << setprecision(2) << op.Rtemp;
        else if(op.IsBool()) out << (op.GetBool()? "true" : "false");
        else out << "ERROR";
        return out;
    }
};


#endif
