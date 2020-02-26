/*
 * lex.cpp
 *
 * CS280 - Fall 2019
 */

#include <cctype>
#include <map>
using std::map;

#include "lex.h"

static map<Token,string> tokenPrint = {
		{ IF, "IF" },
		{ LET, "LET" },
		{ PRINT, "PRINT" },
		{ LOOP, "LOOP" },
		{ BEGIN, "BEGIN" },
		{ END, "END" },

		{ ID, "ID" },

		{ INT, "INT" },
		{ STR, "STR" },

		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ STAR, "STAR" },
		{ SLASH, "SLASH" },
		{ BANG, "BANG" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },
            
		{ SC, "SC" },

		{ ERR, "ERR" },

		{ DONE, "DONE" },
};


ostream& operator<<(ostream& out, const Lex& tok) {
	Token tt = tok.GetToken();
	out << tokenPrint[ tt ];
	if( tt == ID || tt == INT || tt == STR || tt == ERR ) {
		out << "(" << tok.GetLexeme() << ")";
	}
	return out;
}


static map<string,Token> kwmap = {
		{ "if", IF },
		{ "let", LET },
		{ "print", PRINT },
		{ "loop", LOOP },
		{ "begin", BEGIN },
		{ "end", END },
};

Lex
id_or_kw(const string& lexeme, int linenum)
{
	Token tt = ID;

	auto kIt = kwmap.find(lexeme);
	if( kIt != kwmap.end() )
		tt = kIt->second;

	return Lex(tt, lexeme, linenum);
}

Lex
getNextToken(istream& in, int& linenum)
{
	enum LexState { BEGIN, INID, INSTRING, ININT, INCOMMENT } lexstate = BEGIN;
	string lexeme;
	char ch;
    bool sawEscape;

    while(in.get(ch)) {
		switch( lexstate ) {
		case BEGIN:
            if( ch == '\n' )
                linenum++;
                
			if( isspace(ch) )
				continue;

			lexeme = ch;

			if( isalpha(ch) ) {
				lexstate = INID;
			}
			else if( ch == '"' ) {
				lexstate = INSTRING;
                sawEscape = false;
			}
			else if( isdigit(ch) ) {
				lexstate = ININT;
			}
			else if( ch == '/' && in.peek() == '/' ) {
				lexstate = INCOMMENT;
			}
			else {
				Token tt = ERR;
				switch( ch ) {
				case '+':
					tt = PLUS;
					break;
				case '-':
					tt = MINUS;
					break;
				case '*':
					tt = STAR;
					break;
				case '/':
					tt = SLASH;
					break;
				case '!':
					tt = BANG;
					break;
				case '(':
					tt = LPAREN;
					break;
				case ')':
					tt = RPAREN;
					break;
				case ';':
					tt = SC;
					break;
				}

				return Lex(tt, lexeme, linenum);
			}
			break;

		case INID:
			if( isalpha(ch) || isdigit(ch) ) {
				lexeme += ch;
			}
			else {
				in.putback(ch);
				return id_or_kw(lexeme, linenum);
			}
			break;

		case INSTRING:
            if( sawEscape ) {
                // last character was an escape, what's this one??
                sawEscape = false;
                if( ch == 'n' ) ch = '\n';
                // otherwise... it's unchanged
                lexeme += ch;
                break;
           }
                
            if( ch == '\\' ) {
                sawEscape = true;
                break;
            }
                
			lexeme += ch;
			if( ch == '\n' ) {
				return Lex(ERR, lexeme, linenum);
			}
			if( ch == '"' ) {
				lexeme = lexeme.substr(1, lexeme.length()-2);
				return Lex(STR, lexeme, linenum);
			}
			break;

		case ININT:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else {
				in.putback(ch);
				return Lex(INT, lexeme, linenum);
			}
			break;

		case INCOMMENT:
			if( ch == '\n' ) {
                ++linenum;
				lexstate = BEGIN;
			}
			break;
		}

	}

	if( in.eof() )
		return Lex(DONE, "", linenum);
	return Lex(ERR, "some strange I/O error", linenum);
}


