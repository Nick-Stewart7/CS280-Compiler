/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include <string>
#include "val.h"
using std::vector;
using std::map;
using std::string;


// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

// a "forward declaration" for a class to hold values
//class Val;


class ParseTree {
protected:
    int			linenum;
	ParseTree	*left;
	ParseTree	*right;
    
public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}
    void runtime_err(int line, string msg) const{
        //int error_count = 0;
	    //int error_count = 0;
        cout << "RUNTIME ERROR at "<<line << ": " << msg << endl; 
        exit(0);
        //error_count++;	    
    }

	int GetLineNumber() const { return linenum; }

	int Traverse(int input, int (ParseTree::*func)(void)) {
		if( left ) input += left->Traverse(input, func);
		if( right ) input += right->Traverse(input, func);
		return Visit(input, func);
	}

	virtual int Visit(int input, int (ParseTree::*func)(void)) {
		return input;
	}

	int MaxDepth() const {
        int lc = 0, rc = 0;
		if( left ) lc = left->MaxDepth();
		if( right ) rc += right->MaxDepth();
		if( lc > rc ) return lc + 1;
        return rc + 1;
	}

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }
    virtual int IsBang() const { return 0; }
    virtual bool IsLet() const { return false; }
    

	int BangCount() const {
		int cnt = 0;
		if( left ) cnt += left->BangCount();
		if( right ) cnt += right->BangCount();
		cnt += IsBang();
		return cnt;
	}

	int CheckLetBeforeUse(map<string,bool>& var) {
        int cnt = 0;
		if( left ) cnt += left->CheckLetBeforeUse(var);
		if( right ) cnt += right->CheckLetBeforeUse(var);
		if( IsLet() )
			var[ this->GetId() ] = true;
        if( IsIdent() && var.find(GetId()) == var.end() ) {
            cout << "UNDECLARED VARIABLE " << GetId() << endl;
            cnt++;
        }
        return cnt;

	}
    virtual Val Eval(map<string,Val>& syms) const { return Val(); }
};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}

    Val Eval(map<string,Val>& symbols) const{
        left->Eval(symbols);
        if( right )
            right->Eval(symbols);
        return Val();
    }
//	int Visit(int input, int (ParseTree::*func)(void)) {
//		for( auto s : statements ) {
//			input += s->Visit(input, func);
//		}
//		return input;
//	}
};

class Let : public ParseTree {
	string id;

public:
	Let(Lex& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) {}
    
    string GetId() const { return id; }
	bool IsLet() const { return true; }
    
    Val Eval(map<string,Val>& symbols) const{
        
        auto L = left->Eval(symbols);
        
        
        if( L.isErr() ){
            runtime_err(linenum, L.GetErrMsg());
            //return Val();
        }
            //return Val();
            //return L;
        if(symbols.find(id) == symbols.end()){
            symbols.insert(std::pair<string, Val>(id, L));
            //cout<<L;
            //cout<<id;
            //cout<<L;
            return Val();           
        }
        else{
            symbols[id] = L;       
        }
        
        
        return Val();
    }
};

class Print : public ParseTree {
public:
	Print(int line, ParseTree *e) : ParseTree(line, e) {}
    
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        if(L.isErr() ){
            runtime_err(linenum, L.GetErrMsg());
            //return Val();
            //return L;
        }
        std::cout<<L;    
        return Val();
    }
};

class Loop : public ParseTree {

public:
	Loop(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}
    
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        if(L.isInt()){
            while(L.ValInt() != 0){
                auto R = right->Eval(symbols);
                L = left->Eval(symbols);
            }
            return Val();
            //return L;   
        }
        runtime_err(linenum, "Expression is not an integer");
        return Val();
    }
};

class If : public ParseTree {

public:
	If(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}
    
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        if(L.isInt()){
            if(L.ValInt() != 0){
                auto R = right->Eval(symbols);
                //L = left->Eval(symbols);
            }
            return Val();
            //return L;   
        }
        runtime_err(linenum, "Expression is not an integer");
        return Val();
    }
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        if( L.isErr() ){
            runtime_err(linenum, L.GetErrMsg());
            //return Val();
        }
        auto R = right->Eval(symbols);
        if( R.isErr() ){
            runtime_err(linenum, R.GetErrMsg());
            //return Val();
        }
        auto answer = L + R;
        if( answer.isErr() ){
            runtime_err(linenum, answer.GetErrMsg());
            //return Val();
            //return answer;
        }
        
        //cout<<linenum<<endl;
        return answer;
    }
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        if( L.isErr() ){
            runtime_err(linenum, L.GetErrMsg());
            //return Val();
        }
        auto R = right->Eval(symbols);
        if( R.isErr() ){
            runtime_err(linenum, R.GetErrMsg());
            //return Val();
        }
        auto answer = L - R;
        if( answer.isErr() ){
            runtime_err(linenum, answer.GetErrMsg());
            //return Val();
        }
            //return answer;
        
        return answer;
    }
};

class TimesExpr : public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        if( L.isErr()){ 
            runtime_err(linenum, L.GetErrMsg());
            //return Val();
        }
            //return L;
        auto R = right->Eval(symbols);
        if( R.isErr() ){
            runtime_err(linenum, R.GetErrMsg());
            //return Val();
        }
            //return R;
        auto answer = L * R;
        if( answer.isErr() ){
            runtime_err(linenum, answer.GetErrMsg());
            //return Val();
        }
            //return answer;
        
        //cout<<linenum<<endl;
        return answer;
    }
};

class DivideExpr : public ParseTree {
public:
	DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        if( L.isErr() ){
            runtime_err(linenum, L.GetErrMsg());
            //return Val();
        }
            //return L;
        auto R = right->Eval(symbols);
        if( R.isErr() ){
            
            runtime_err(linenum, R.GetErrMsg());
            //return Val();
        }
            //return R;
        auto answer = L / R;
        if( answer.isErr() ){
            runtime_err(linenum, answer.GetErrMsg());
            //return Val();
        }
            //return answer;
        
        return answer;
    }
};

class BangExpr : public ParseTree {
public:
	BangExpr(int line, ParseTree *l) : ParseTree(line,l) {}
    virtual int IsBang() const { return 1; }
    
    Val Eval(map<string,Val>& symbols) const{
        auto L = left->Eval(symbols);
        //cout<<"asd"<<L.ValInt()<<endl;
        if( L.isErr() ){
            runtime_err(linenum, L.GetErrMsg());
            //return Val();
        }
            //return L; 
        auto answer = !L;
        if( answer.isErr() ){
            runtime_err(linenum, answer.GetErrMsg());
            //return Val();
        }
            //return answer;   
        return answer;
    }
};

class IConst : public ParseTree {
	int val;

public:
	IConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
    Val Eval(map<string,Val>& symbols) const{
        return Val(val);
    }
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}
    Val Eval(map<string,Val>& symbols) const{
        return Val(val);
    }
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Lex& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
    
    Val Eval(map<string,Val>& symbols) const{
        return symbols[id];        
    }
        
};

#endif /* PARSETREE_H_ */
