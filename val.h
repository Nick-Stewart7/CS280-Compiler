#ifndef VAL_H
#define VAL_H

#include <string>
using namespace std;

class Val {
    int        i;
    string    s;
    enum ValType { ISINT, ISSTR, ISERR } vt;
    
public:
    Val() : i(0), vt(ISERR) {}
    Val(int i) : i(i), vt(ISINT) {}
    Val(string s) : i(0), s(s), vt(ISSTR) {}
    Val(ValType vt, string errmsg) : i(0), s(errmsg), vt(ISERR) {}
    
    ValType getVt() const { return vt; }
    
    bool isErr() const { return vt == ISERR; }
    bool isInt() const { return vt == ISINT; }
    bool isStr() const { return vt == ISSTR; }
    
    int ValInt() const {
        if( isInt() ) return i;
        string s = "This Val is not an Int";
        throw s;
    }
    string ValString() const {
        if( isStr() ) return s;
        string s = "This Val is not a Str";
        throw s;
    }
    
    friend ostream& operator<<(ostream& out, const Val& v) {
        if (v.isErr()){
            out << v.GetErrMsg();
        }
        if (v.isInt()){
            out << v.ValInt();
        }
        if (v.isStr()){
            out << v.ValString();
        }
        return out;
    }
    
    string GetErrMsg() const {
        if( isErr() ) return s;
        string s = "This Val is not an Error";
        throw s;
    }
   
    Val operator+(const Val& op) const {
        if( isInt() && op.isInt() )
            return ValInt() + op.ValInt();
        if( isStr() && op.isStr() )
            return ValString() + op.ValString();
        return Val(ISERR, "Type mismatch on operands of +");
        //string s ="Type mismatch on operands of +";
        //throw(s);
    }
    
    Val operator-(const Val& op) const {
        if( isInt() && op.isInt() )
            return ValInt() - op.ValInt();
        return Val(ISERR, "Type mismatch on operands of -");
        //string s = "Type mismatch on operands of -";
        //throw s;
    }
    
    Val operator*(const Val& op) const {
        if( isInt() && op.isInt() )
            return ValInt() * op.ValInt();      
        if( isStr() && op.isInt()){
            int i = op.ValInt();
            if(i < 0){
                return Val(ISERR, "Cannot multiply string by negative int");
                //string s = "Cannot multiply string by negative int";
                //throw s;
                //return Val(ISERR, "Cannot mutiply string by negative int");
            }    
            string s = ValString();
            string result = "";
            for(int j = 0; j < i; j++){
                result += s;   
            }
            return result;
        }
        if(isInt() && op.isStr()){
            int i = ValInt();
            string s = op.ValString();
            string result = "";
            for(int j = 0; j < i; j++){
                result += s;   
            }
            return result;       
        }
        return Val(ISERR, "Type mismatch on operands of *");
        //string s ="Type mismatch on operands of *";
        //throw(s);
    }
    
    Val operator/(const Val& op) const {
        if( isInt() && op.isInt() ){
            if(op.ValInt() == 0){
                return Val(ISERR, "Divide by zero error");
                //string s ="Divide by zero error";
                //throw(s);
            }
            return ValInt() / op.ValInt();
               
        }
        return Val(ISERR, "Type mismatch on operands of /");
        //string s = "Type mismatch on operands of /";
        //throw s;
    }
    
    Val operator!() const {
        if(isInt()){
            bool minun = false;
            int val = ValInt();
            if(val < 0){
                val = -val;
                minun = true;   
            }
            else{
                minun = false;   
            } 
            int len = to_string(val).length();
            string result = "";
            for(int i = 0; i < len; i++){
                int last = val % 10;
                result += to_string(last);
                val = val/10;      
            }
            int rev = stoi(result);
            if(minun){
                return -rev;   
            }
		    return rev;   
        }
        if(isStr()){
            string val = ValString();
            int len = val.length();
            int last = len-1;
            string result = "";
            for(int i = 0; i < len; i++){ 
                result += val[last];
                last --;      
            }
            return result;
        }
        return Val(ISERR, "Type mismatch on operands of !");
        //string s = "Type mismatch on operands of !";
        //throw s;
    }
    
};

#endif