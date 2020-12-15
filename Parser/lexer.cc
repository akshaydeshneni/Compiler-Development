/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <stdio.h>
#include <stack>
#include <string.h>
#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE", "EQUAL", "COLON", "COMMA",
     "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 2
string keyword[] = {"public", "private"};

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

struct SymbolTableItem{
	char* name;
	char* scope;
	int publicPrivate;
};

struct SymbolTable{
	SymbolTableItem* item;
	SymbolTable* prev;
	SymbolTable* next;
};

struct SymbolTable* symboltable;
struct SymbolTable* tempNode;
struct SymbolTable* newNode;

char* currentScope = "::";
string lex1 = "";
string lex2 = "";
char* t = "::";
int pubPriv = 0;

struct store{
	string lhs;
	string rhs;
	char* lScope;
	char* rScope;
};

vector< struct store> storeList;

void syntax_error() 
{
	cout << "Syntax Error" << endl;
	exit(1);
}

void searchAndAssign(Token t1, Token t2){

	struct SymbolTable* table1 = newNode;
	struct SymbolTable* table2 = newNode;
	char* lScope = (char*)malloc(sizeof(char)*20);
	char* rScope = (char*)malloc(sizeof(char)*20);

	while(table1 != NULL) {

		if(table1->item->name == t1.lexeme) {

			if(table1->item->publicPrivate == 2 && currentScope != table1->item->scope) {

				table1 = table1->prev;
				continue;
			}
			else {

				lScope = table1->item->scope;
				break;
			}
		}

				table1 = table1->prev;
	}

	while(table2 != NULL) {

		if(table2->item->name == t2.lexeme) {

			if(table2->item->publicPrivate == 2 && currentScope != table2->item->scope) {

				table2 = table2->prev;
				continue;
			}
			else {

				rScope = table2->item->scope;
				break;
			}
		}

				table2 = table2->prev;
	}

	if(strlen(lScope) == 0) {
		lScope = new char[2];
		lScope[0] = '?';
		lScope[1] = '\0';
	}

	if(strlen(rScope) == 0) {
		rScope = new char[2];
		rScope[0] = '?';
		rScope[1] = '\0';
	}

	struct store temp_node;
	temp_node.lScope = lScope;
	temp_node.lhs = t1.lexeme;
	temp_node.rScope = rScope;
	temp_node.rhs = t2.lexeme;

	storeList.push_back(temp_node);

}

void deleteList(char* lexeme) 
{

	if(symboltable ==  NULL || newNode == NULL || tempNode == NULL) {
		return;
	}

	while(newNode->item->scope == lexeme) {

		tempNode->next = NULL;
		newNode->next = NULL;

		if(newNode->prev != NULL){

			newNode=newNode->prev;
			tempNode->next=newNode;
			newNode->next = NULL;
			currentScope = newNode->item->scope;
		}
		else if(tempNode == newNode) {

			tempNode = NULL;
			newNode = NULL;
			return;
		}
		else {

			tempNode->next = NULL;
			newNode->prev = NULL;
			newNode=tempNode;
			tempNode->next = NULL;
		}
	}

	currentScope = newNode->item->scope;
}

void printlist()
{
for(int i=0; i< storeList.size(); ++i){
	
	char* lscopeChar = storeList[i].lScope;
	char* rscopeChar = storeList[i].rScope;
	string lscope(lscopeChar);
	string rscope(rscopeChar);
	string lhs = storeList[i].lhs;
	string rhs = storeList[i].rhs;
	
    if((lscope != "::") && (rscope != "::")){ 
        
        cout << lscope << "." << lhs << " = " << rscope << "." << rhs << endl;  
    }
    else if((lscope != "::") && (rscope == "::")) {
    	
    	cout << lscope << "." << lhs << " = " << rscope << rhs << endl;
    }
    else if((lscope == "::") && (rscope != "::")) {

    	cout << lscope << lhs << " = " << rscope << "." << rhs << endl;
    }
    else {

    	cout << lscope << lhs << " = " << rscope << rhs << endl;
    }
}
}

void addToList(char* lexeme)
{
	if(symboltable == NULL) {

		symboltable = (SymbolTable*) malloc(sizeof(SymbolTable));
		struct SymbolTableItem* newItem = (SymbolTableItem*) malloc(sizeof(SymbolTableItem));
		symboltable->item = newItem;
		symboltable->next = NULL;
		symboltable->prev = NULL;
		int len = strlen(lexeme);
		symboltable->item->name = new char[len+1];
		strcpy(symboltable->item->name, lexeme);
		symboltable->item->name[len] = '\0';
		symboltable->item->scope = currentScope;
		symboltable->item->publicPrivate = pubPriv;
		newNode = symboltable;
		tempNode = symboltable;
	}
	else {

		tempNode = symboltable;
		while (tempNode->next != NULL) {
			tempNode = tempNode->next;
		}

		newNode = (SymbolTable*) malloc (sizeof(SymbolTable));
		struct SymbolTableItem* newItem = (SymbolTableItem*) malloc(sizeof(SymbolTableItem));
		newNode->item = newItem;
		newNode->next = NULL;
		newNode->prev = tempNode;
		tempNode->next = newNode;
		int len = strlen(lexeme);
		newNode->item->name = new char[len+1];
		strcpy(newNode->item-> name, lexeme);
		newNode->item->name[len] = '\0';
		newNode->item->scope = currentScope;
		newNode->item->publicPrivate = pubPriv;

	} 
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

LexicalAnalyzer lexer;
Token temp1;
Token temp2;
Token temp3;

void LexicalAnalyzer::parse_program()
{
	
	temp1 = lexer.GetToken();

	if(temp1.token_type == ID) {
		
		temp2 = lexer.GetToken();
		if(temp2.token_type == COMMA) {

			lexer.UngetToken(temp2);
			lexer.UngetToken(temp1);
			lexer.parse_globalVars();
			lexer.parse_scope();
			return;
		}
		else if(temp2.token_type == SEMICOLON) {
			
			lexer.UngetToken(temp2);
			lexer.UngetToken(temp1);
			lexer.parse_globalVars();
			lexer.parse_scope();
			return;

		}
		else if(temp2.token_type = LBRACE) {

			lexer.UngetToken(temp2);
			lexer.UngetToken(temp1);
			lexer.parse_scope();
		}
		else {
			
			syntax_error();
		}

	}
	else {
		
		syntax_error();
	}
}

void LexicalAnalyzer::parse_globalVars() 
{
	
	temp1 = lexer.GetToken();
	

	currentScope = "::";

	if(temp1.token_type == ID) {

		lexer.UngetToken(temp1);
		lexer.parse_varlist();
		temp1 = lexer.GetToken();
		

		if(temp1.token_type == SEMICOLON) {
			
			return;
		}
		else {
			syntax_error();
		}
	}
	else {
		syntax_error();
	}
}

void LexicalAnalyzer::parse_varlist()
{
	
	temp1 = lexer.GetToken();
	

	char* lexeme = (char*) malloc (sizeof(temp1.lexeme));
	memcpy(lexeme, (temp1.lexeme).c_str(), sizeof(temp1));
	addToList(lexeme);

	if(temp1.token_type == ID) {

		temp1 = lexer.GetToken();
		

		if(temp1.token_type == COMMA) {

			lexer.parse_varlist();
		} 
		else if(temp1.token_type == SEMICOLON) {

			lexer.UngetToken(temp1);
			return;
		}
		else {

			syntax_error();
		}  

	}
	else {

		syntax_error();
	}
}

void LexicalAnalyzer::parse_scope()
{	
	
	temp1 = lexer.GetToken();
	

	if(temp1.token_type == ID) {
		
		string lex1 = temp1.lexeme;
		t= (char*)malloc(lex1.size() + 1);
		strcpy(t, lex1.c_str());
		currentScope = t;
		addToList(currentScope);

		temp1 = lexer.GetToken();
		
		if(temp1.token_type == LBRACE) {
			
			
			lexer.parse_publicVars();
			lexer.parse_privateVars();
			lexer.parse_stmtlist();

			temp1 = lexer.GetToken();
			
				if(temp1.token_type == RBRACE) {

					deleteList(currentScope);
					return;
				}
				else {

					syntax_error();
				}
		}
		else {

			syntax_error();
		}
	}
	else {

		syntax_error();
	}
}
	

void LexicalAnalyzer::parse_stmtlist()
{
	temp1 = lexer.GetToken();

	if(temp1.token_type == ID) {

		lexer.UngetToken(temp1);
		lexer.parse_stmt();
		temp1 = lexer.GetToken();

		if(temp1.token_type == ID) {

			lexer.UngetToken(temp1);
			lexer.parse_stmtlist();
		}
		else if(temp1.token_type == RBRACE) {

			lexer.UngetToken(temp1);
			return;
		}
		else if(temp1.token_type == LBRACE) {

			temp2 = lexer.GetToken();

			if(temp2.token_type == ID) {

				UngetToken(temp2);
				deleteList(currentScope);
				parse_stmt();
			}
			else if(temp2.token_type == PUBLIC) {

				UngetToken(temp2);
				deleteList(currentScope);
				parse_publicVars();
			}
			else if(temp2.token_type == PRIVATE) {

				UngetToken(temp2);
				deleteList(currentScope);
				parse_privateVars();
			}
			else if(temp2.token_type == RBRACE) {
				UngetToken(temp2);
				return;
			}
			else {

				syntax_error();
			}

		}
		else {

			syntax_error();
		}
	}
	else {

		syntax_error();
	}
}

void LexicalAnalyzer::parse_stmt()
{

	temp1 = lexer.GetToken();
	

	if(temp1.token_type == ID) {

		temp2 = lexer.GetToken();
		

		if(temp2.token_type == LBRACE) {
			
			lexer.UngetToken(temp2);
			lexer.UngetToken(temp1);
			lexer.parse_scope();
		}
		else if(temp2.token_type == EQUAL) {

			temp3 = lexer.GetToken();
			
			
			if(temp3.token_type == ID) {

				searchAndAssign(temp1, temp3);

				temp1 = lexer.GetToken();
			

				if(temp1.token_type == SEMICOLON) {

					
					return;
				}
				else {

					syntax_error();
				}
			}
			else {

				syntax_error();
			}
		}
		else {

			syntax_error();
		}
	}
	else {

		syntax_error();
	}
}

void LexicalAnalyzer::parse_publicVars()
{	
	
	temp1 = lexer.GetToken();
	

	if(temp1.token_type == PUBLIC) {

		pubPriv = 1;
		temp1 = lexer.GetToken();
		

		if(temp1.token_type == COLON) {

			temp1 = lexer.GetToken();
			
			
			if(temp1.token_type == ID) {

				lexer.UngetToken(temp1);
				lexer.parse_varlist();
				temp1 = lexer.GetToken();
				

				if(temp1.token_type == SEMICOLON) {
					return;
				}
				else {

					syntax_error();
				}
			}
			else {

				syntax_error();
			}
		}
		else {

			syntax_error();
		}
	}
	else if(temp1.token_type == PRIVATE) {

		lexer.UngetToken(temp1);
	}
	else if(temp1.token_type == ID) {

		lexer.UngetToken(temp1);
	}
	else {

		syntax_error();
	}
}

void LexicalAnalyzer::parse_privateVars()
{	
	temp1 = lexer.GetToken();
	
	if(temp1.token_type == PRIVATE) {

		pubPriv = 2;
		temp1 = lexer.GetToken();
	

		if(temp1.token_type == COLON) {

			temp1 = lexer.GetToken();
			
			
			if(temp1.token_type == ID) {

				lexer.UngetToken(temp1);
				lexer.parse_varlist();
				temp1 = lexer.GetToken();
				

				if(temp1.token_type == SEMICOLON) {

					return;
				}
				else {

					syntax_error();
				}
			}
			else {

				syntax_error();
			}
		}
		else {

			syntax_error();
		}
	}
	else if(temp1.token_type == ID) {

		lexer.UngetToken(temp1);
		
	}
	else {

		syntax_error();
	}
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComment()
{
	char c;
  	bool comment = false;
    
    input.GetChar(c);
  	
  	if(input.EndOfInput()){
  		
  		input.UngetChar(c);
  		return comment;
  	}
  
    
    if(c=='/'){
    		
   		input.GetChar(c);
         	
        if(c=='/'){
         	
         	comment = true;
         
           	while (c != '\n'){
           			
           		comment = true;
           		input.GetChar(c);
           	}

         	line_no +=1;
         	SkipComment();      
        }
        else{
         		
         	comment = false;
         	exit(1);
        }
    }
    else {
     		
    	input.UngetChar(c);    
     	return comment;
    }

}


bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}


TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    
    SkipSpace();
    SkipComment();
    SkipSpace();

    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                tmp.token_type = ERROR;
                return tmp;
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput()) {
                tmp.token_type = END_OF_FILE;
            }
            else {
                tmp.token_type = ERROR;
            }

            return tmp;
    }
}

int main()
{
    lexer.parse_program();
    printlist();
}
