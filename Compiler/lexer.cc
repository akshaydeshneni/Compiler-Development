
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
#include <string.h>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE", "INT", "REAL", "BOO", "TR", "FA", "IF", "WHILE", "SWITCH", "CASE", "PUBLIC", "PRIVATE", "NUM", "REALNUM", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private" };

    LexicalAnalyzer lexer;
    Token token;
    TokenType tempTokenType;

struct scopeResolve {
    char* scope;
    scopeResolve* next;
};

struct sTableEntry
{
    string name;
    char* scope;
    int binNo;
    int type;
    int pubpriv;
};

struct sTable
{
    sTableEntry* item;
    sTable *prev;
    sTable *next;
};

sTable* symbolTable;
char* currentScope;
char* lResolve;
char* rResolve;
scopeResolve* scopeTable;
int currentPrivPub = 0;
int currentType = 4;
int bin = 0;

int sPlus = 50;
int sMinus = 60;
int sMult = 70;
int sDiv = 80;
int sGreater = 90;
int sLess = 100;
int sGteq = 110;
int sLteq = 120;
int sEqual = 130;
int sNotEqual = 140;

void addScope(void){
    
    if(scopeTable == NULL){
        
        scopeResolve* newScopeItem = (scopeResolve *) malloc(sizeof(scopeResolve));
        newScopeItem->scope = (char *)malloc(sizeof(currentScope));
        memcpy(newScopeItem->scope,currentScope,sizeof(currentScope));
        newScopeItem->next = NULL;
        scopeTable = newScopeItem;
    }else{
        scopeResolve* tempTable = scopeTable;
        while(tempTable->next != NULL){
            tempTable = tempTable->next;
        }   
        
        scopeResolve* newScopeItem = (scopeResolve *) malloc(sizeof(scopeResolve));
        newScopeItem->scope = (char *)malloc(sizeof(currentScope));
        memcpy(newScopeItem->scope,currentScope,sizeof(currentScope));
        newScopeItem->next = NULL;
        tempTable->next = newScopeItem;
    }
}

void deleteScope(void){
        scopeResolve* tempTable = scopeTable;
        if(tempTable != NULL){
            if(tempTable->next == NULL){
                tempTable = NULL;
            }else{
                while(tempTable->next->next != NULL){
                    tempTable = tempTable->next;
                }
                currentScope = (char *)malloc(sizeof(tempTable->scope));
                memcpy(currentScope,tempTable->scope,sizeof(tempTable->scope));
                tempTable->next = NULL;   
            }
        }

}

void addList(std::string lexeme, int bin, int type)
{
    if(symbolTable == NULL)
    {

        sTable* newEntry = (sTable *) malloc(sizeof(sTable));
        sTableEntry* newItem = (sTableEntry *)malloc(sizeof(sTableEntry));

        newItem->name = lexeme;
        newItem->binNo = token.line_no;
        newItem->type = type;
        newItem->pubpriv = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = NULL;

        symbolTable = newEntry;

    }
    else
    {
        sTable* temp = symbolTable;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }

        sTable* newEntry = (sTable *) malloc(sizeof(sTable));
        sTableEntry* newItem = (sTableEntry *)malloc(sizeof(sTableEntry));

        newItem->name = lexeme;
        newItem->binNo = token.line_no;
        //memcpy(newItem->scope, currentScope.c_str(), currentScope.size()+1);
        newItem->type = type;
        newItem->pubpriv = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = temp;
        temp->next = newEntry;
    }
}

void printScope(void){
    
    scopeResolve* temp = scopeTable;
    cout << "\n Printing Scope Table \n";
    while(temp->next != NULL){
        cout << " Scope " << temp->scope << " -> ";
        temp = temp->next;
        
    }
    cout << " Scope " << temp->scope << " \n";
}

string output = "";

void printList(void)
{
    sTable* temp = symbolTable;
    int temp1;

    while(temp->next != NULL)
    {
       if(temp->item->type > 3 && temp->item->pubpriv == 0)
        {          
            temp1 = temp->item->type;
            output += temp->item->name;
            temp->item->pubpriv = 1;
            while(temp->next != NULL)
            {
                temp = temp->next;
                if(temp->item->type == temp1)
                {
                    output += ", " + temp->item->name;
                    temp->item->pubpriv = 1;
                }
                else
                {

                }
            }
            output += ": ? #";
            cout << output <<endl;
            temp->item->pubpriv = 1;
            output = "";
            temp = symbolTable;
        }
        else if(temp->item->type < 4 && temp->item->pubpriv == 0)
        {
            string lCase = keyword[(temp->item->type)-1 ];
            int temp1 = temp->item->type;
            output = temp->item->name + ": " + lCase + " #";
            cout << output <<endl;
            output = "";
            temp->item->pubpriv = 1;           

            while(temp->next != NULL  && temp->next->item->type == temp1)
            {
                temp = temp->next;
                string lCase2 = keyword[(temp->item->type)-1];
                output = temp->item->name + ": " + lCase2 + " #";
                cout << output <<endl;
                temp->item->pubpriv = 1;
                output = "";
            }
        }
        else
        {
            temp = temp->next;
        }
    }
    if(temp->item->type <= 3 && temp->item->pubpriv == 0)
    {        
        string lCase3 = keyword[(temp->item->type)-1];
        output += temp->item->name + ": " + lCase3 + " #";
        cout << output <<endl;
        output = "";
    }
    else if (temp->item->type > 3 && temp->item->pubpriv == 0)
    {
        output += temp->item->name + ":" + " ? " + "#";
        cout << output <<endl;
        output = "";
    }
    else
    {

    }
}

void deleteList(void){
    
    sTable* temp = symbolTable;
    
    if(temp!= NULL){
        while(temp->next != NULL && strcmp(temp->item->scope,currentScope) != 0){
            temp = temp->next;
            
        }
        
        if(strcmp(temp->item->scope,currentScope) == 0){
            
            //cout << "\n found Match: " << temp->item->scope << "  " << currentScope << "\n";
            if(strcmp(currentScope,"::") != 0){
            temp = temp->prev;
            temp->next = NULL;
            }else{
                temp = NULL;
            }
            //deleteList();
        }
        //printList();
    
    }
    

}

void updateList(int curType, int newType) {

    sTable* temp = symbolTable;

    if(temp == NULL){
        return;
    }
    else {
        while(temp != NULL) {
            if(temp->item->type == curType) {
                temp->item->type = newType;
                return;
            }
            if(temp->next != NULL) {
                temp = temp->next;
            }
            else
            {
                break;
            }
            
        }
    }
}

int searchList(std::string n)
{
    sTable* temp = symbolTable;
    bool found = false;
    if (symbolTable == NULL)
    {
        
        addList(n, token.line_no, currentType);
        currentType++;
        return (4);
    }
    else
    {
        while(temp->next != NULL)
        {
            if(strcmp(temp->item->name.c_str(), n.c_str()) == 0)
            {
                found = true;
                return(temp->item->type);
            }
            else
            {
                temp = temp->next;
            }
        }
        if(strcmp(temp->item->name.c_str(), n.c_str()) == 0)
        {
            found = true;
            return(temp->item->type);
        }
        if(!found)
        {
            addList(n, token.line_no, currentType);
            currentType++;
            int t = currentType-1;
            return(t);
        }
        else
        {
         
        }
    }
}


void Token::Print()
{
    cout << "{" << this->lexeme << " , "
        << reserved[(int) this->token_type] << " , "
        << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    bin = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');
    bin = line_no;

    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
        bin = line_no;
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComments()
{
    bool comments = false;
    char c;
    if(input.EndOfInput() )
    {
        input.UngetChar(c);
        return comments;
    }

    input.GetChar(c);

    if(c == '/')
    {
        input.GetChar(c);
        if(c == '/')
        {
            comments = true;
            while(c != '\n')
            {
                comments = true;
                input.GetChar(c);
            }
            line_no++;
            bin = line_no;
            SkipComments();
        }
        else
        {
            comments = false;
            cout << "Syntax Error\n";
            exit(0);
        }
    }
    else
    {
        input.UngetChar(c);
        return comments;
    }
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            input.GetChar(c);
            if(c == '.')
            {            
                input.GetChar(c);

                if(!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            }
            else
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if(c == '.')
            {               
                input.GetChar(c);

                if(!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
        if(realNUM)
        {
            tmp.token_type = REALNUM;
        }
        else
        {
            tmp.token_type = NUM;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (IsKeyword(tmp.lexeme))
        {
             tmp.token_type = FindKeywordIndex(tmp.lexeme);
        }
        else
        {
            tmp.token_type = ID;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
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
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    //SkipComments();
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    //cout << "\n Char obtained " << c << "\n";
    switch (c)
    {
        case '!':
            tmp.token_type = NOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '>':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = GTEQ;
            }
            else
            {
                input.UngetChar(c);
                tmp.token_type = GREATER;
            }
            return tmp;
        case '<':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = LTEQ;
            }
            else if (c == '>')
            {
                tmp.token_type = NOTEQUAL;
            }
            else
            {
                input.UngetChar(c);
                tmp.token_type = LESS;
            }
            return tmp;
        case '(':
            //cout << "\n I am here" << c << " \n";
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
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
            if (isdigit(c))
            {
                input.UngetChar(c);
                return ScanNumber();
            }
            else if (isalpha(c))
            {
                input.UngetChar(c);
                //cout << "\n ID scan " << c << " \n";
                return ScanIdOrKeyword();
            }
            else if (input.EndOfInput())
            {
                tmp.token_type = END_OF_FILE;
            }
            else
            {
                tmp.token_type = ERROR;
            }
            return tmp;
    }
}

// parse var_list
int parse_varlist(void){
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    
    char* lexeme = (char*)malloc(sizeof(token.lexeme)+1);
    memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size()+1);
    addList(lexeme,token.line_no,0);
    /*sTable* temp2 = symbolTable;
    while(temp2!=NULL){
        cout << "\n Symbol Table => Name: " << temp2->item->name << " Scope: " << temp2->item->scope << " Permission: " << temp2->item->pubpriv << "\n";
        if(temp2->next != NULL){
            temp2 = temp2->next;
        }else{
            break;
        }
        
    }*/
    
    // Check First set of ID
    if(token.token_type == ID){
        token = lexer.GetToken();
        //token.Print();
        if(token.token_type == COMMA){
            //cout << "\n Rule Parsed: var_list -> ID COMMA var_list \n";
            tempI = parse_varlist();
        }else if(token.token_type == COLON){
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: var_list -> ID \n";
            
        
        }else{
            cout << "\n Syntax Error \n";
            
        }
    }else{
        cout << "\n Syntax Error \n";
    }    
   
    return(0);

}

int parse_body(void);
Token token1;
/*int parse_stmt(void){
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    char* tempID;
    
    tempID = (char *)malloc((token.lexeme).length()+1);
                
    memcpy(tempID, (token.lexeme).c_str(), (token.lexeme).length()+1);
    
    if(token.token_type == ID){
      
        token1 = lexer.GetToken();
        //token1.Print();
        if(token1.token_type == LBRACE){
            tempTokenType = lexer.UngetToken(token1);
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: stmt -> scope\n";
            tempI = parse_scope();
            
        }else if(token1.token_type == EQUAL){
            token = lexer.GetToken();
            //token.Print();
            if(token.token_type == ID){
                
                searchList(tempID,0);
                if(strcmp(lResolve,"::") == 0){
                
                    cout << lResolve << tempID << " = ";
                }else{
                    cout << lResolve << "." << tempID << " = ";
                }
                free(tempID);
                free(lResolve);
                
                tempID = (char *)malloc((token.lexeme).length()+1);
                
                memcpy(tempID, (token.lexeme).c_str(), (token.lexeme).length()+1);
                
                searchList(tempID,1);
                //cout<< "\n tempID " << tempID << "\n";
                if(strcmp(rResolve,"::") == 0){
                
                    cout << rResolve << tempID << "\n";
                }else{
                    cout << rResolve << "." << tempID << "\n";
                }
                free(tempID);
                
                free(rResolve);
                token = lexer.GetToken();
                //token.Print();
                if(token.token_type == SEMICOLON){
                    //cout << "\n Rule Parsed: stmt -> ID EQUAL ID SEMICOLON \n";
                    
                }else{
                    cout << "\n Syntax Error \n";
                }
            }else{
                cout << "\n Syntax Error \n";
            }
        }else{
            cout << "\n Syntax Error \n";
        }
    
    }else{
        cout << "\n Syntax Error \n";
    }
    return(0);

}*/

int parse_unaryOperator(void)
{
    token = lexer.GetToken();

    if(token.token_type == NOT)
    {
        //cout << "\n Rule parsed: unary_operator -> NOT";
        return(3);
    }
    else
    {
        cout << "\n Syntax Error\n";
        return(0);
    }
}

int parse_binaryOperator(void){
    token = lexer.GetToken();
    //keep track of the number of bin operations in binNo
    if(token.token_type == PLUS  ){
        //return -1
		return(sPlus);
        //cout << "\n Rule parsed: binary_operator -> PLUS\n";
    }else if(token.token_type == MINUS ){
        //return -1
        //cout << "\n Rule parsed: binary_operator -> MINUS \n";
		return(sMinus);
    }else if(token.token_type == MULT){
        //cout << "\n Rule parsed: binary_operator -> MULT\n";
        //return -1
		return(sMult);
    }else if(token.token_type == DIV ){
        //return -1
        //cout << "\n Rule parsed: binary_operator -> DIV \n";
		return(sDiv);
    }else if(token.token_type == GREATER){
        // return 2
        //cout << "\n Rule parsed: binary_operator -> GREATER \n";
		return(sGreater);
    }else if(token.token_type == LESS  ){
        // return 2
        //cout << "\n Rule parsed: binary_operator -> LESS\n";
		return(sLess);
    }else if(token.token_type == GTEQ ){
        // return 2
        //cout << "\n Rule parsed: binary_operator -> GTEQ \n";
		return(sGteq);
    }else if(token.token_type == LTEQ){
        //cout << "\n Rule parsed: binary_operator -> LTEQ\n";
        // return 2
		return(sLteq);
    }else if(token.token_type == EQUAL ){
        // return 2
        //cout << "\n Rule parsed: binary_operator -> EQUAL \n";
		return(sEqual);
    }else if(token.token_type == NOTEQUAL){
        // return 2
        //cout << "\n Rule parsed: binary_operator -> NOTEQUAL \n";
		return(sNotEqual);
    }else{
        cout << "\n Syntax Error \n";
        return(-1);
    }
}

int parse_primary(void){
    token = lexer.GetToken();
    
    if(token.token_type == ID  ){
        // search list for the token. If token available then return the type of the token. if not then add the token to the list
        // make its scope = "h" and make its type = -1;

        //char* lex = (char*) malloc (sizeof(token.lexeme));
        //memcpy(lex, (token.lexeme).c_str(), sizeof(token));

		return(searchList(token.lexeme));

        //cout << "\n Rule parsed: primary -> ID\n";
    }else if(token.token_type == NUM ){
    
        //cout << "\n Rule parsed: primary -> NUM \n";
		return(1);
    }else if(token.token_type == REALNUM){
        //cout << "\n Rule parsed: primary -> REALNUM\n";
		return(2);
    }else if(token.token_type == TR ){
    
        //cout << "\n Rule parsed: primary -> TRUE \n";
		return(3);
    }else if(token.token_type == FA){
        //cout << "\n Rule parsed: primary -> FALSE \n";
		return(3);
    }else{
        cout << "\n Syntax Error \n";
        return(0);
    }
}

void helper(int currentType, int newType)
{
    sTable* temp = symbolTable;

    while(temp->next != NULL)
    {
        if(temp->item->type == currentType)
        {
            temp->item->type = newType;
        }
        temp = temp->next;
    }
    if(temp->item->type == currentType)
    {
        temp->item->type = newType;
    }
}

int parse_expression(void)
{
    //check for C2 error here
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA )
    {
        //cout << "\n Rule parsed: expression -> primary \n";
        lexer.UngetToken(token);
        tempI = parse_primary();
    }
    else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL)
    {
        //cout << "\n Rule parsed: expression -> binary_operator expression expression \n";
        int leftExp;
        int rightExp;
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_binaryOperator(); 
        leftExp = parse_expression();
        rightExp = parse_expression();     
        if((leftExp != rightExp) || (tempI != sPlus && tempI != sMinus && tempI != sMult && tempI != sDiv && tempI != sGreater && tempI != sLess && tempI != sGteq && tempI != sLteq && tempI != sEqual && tempI != sNotEqual))
        {
            
            if(tempI == sPlus || tempI == sMinus || tempI == sMult || tempI == sDiv)
            {
                if(leftExp <= 2 && rightExp > 3)
                {
                    helper(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else if(leftExp > 3 && rightExp <= 2)
                {
                    helper(leftExp, rightExp);
                    leftExp = rightExp;
                }
                else if(leftExp == 3 && rightExp > 3)
                {
                    helper(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else if(rightExp == 3 && leftExp > 3)
                {
                    helper(leftExp, rightExp);
                    leftExp = rightExp;
                }
                else if(leftExp > 3 && rightExp > 3)
                {
                    helper(rightExp, leftExp);
                    rightExp = leftExp;

                    //updateList(rightExp, 2);
                    //updateList(leftExp, 2);
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                    exit(1);
                }
            }
            else if(tempI == sGteq || tempI == sGreater || tempI == sLteq || tempI == sNotEqual || tempI == sLess || tempI == sEqual)
            {
                
                if(rightExp > 3 && leftExp > 3)
                {
                    helper(rightExp, leftExp);
                    rightExp = leftExp;
                    //updateList(rightExp, 3);
                    //updateList(leftExp, 3);
                    return(3);
                }
                else if(leftExp > 3 && rightExp <= 2)
                {
                    helper(leftExp, rightExp);
                    leftExp = rightExp;
                    //return(leftExp);
                }
                else if(leftExp <= 2 && rightExp > 3)
                {
                    helper(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else if(leftExp == 3 && rightExp > 3)
                {
                    helper(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else if(rightExp == 3 && leftExp > 3)
                {
                    helper(leftExp, rightExp);
                    leftExp = rightExp;
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else
            {
                cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                exit(1);
            }
        }
        if(tempI == sGteq || tempI == sGreater || tempI == sLteq || tempI == sLess || tempI == sEqual )
        {
            tempI = 3;
            
        }
        else
        {
            tempI = rightExp;
        }
    }
    else if(token.token_type == NOT)
    {
        //cout << "\n Rule parsed: expression -> unary_operator expression \n";
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_unaryOperator(); // return 0 for error and return 1 for NOT
        tempI = parse_expression();
        if(tempI != 3)
        {
            cout << "TYPE MISMATCH " << token.line_no << " C3"<<endl;
            exit(1);
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
        return(0);
    }
    return tempI;
}

void comp(int line_No, int token_Type)
{
    sTable* temp = symbolTable;
    while(temp->next != NULL)
    {
        if(temp->item->binNo == line_No)
        {
            temp->item->type = token_Type;
        }
        temp = temp->next;
    }
    if(temp->item->binNo == line_No)
    {
        temp->item->type = token_Type;
    }
}

int parse_assstmt(void){
    int tempI;
	int leftType;
	int rightType;

    token = lexer.GetToken();
    //cout << "\n token name " << token.lexeme << " \n";
    if(token.token_type == ID){

        leftType = searchList(token.lexeme);
        // search for the token in the searchList --> the token is available, leftType = type of the available token
        // it is not available in the searchList, add the token to the list, make its type = -1; make its scope = "h".
	//leftType = searchList(token.lexeme);
        token = lexer.GetToken();
        //cout << "\n token name " << token.lexeme << " \n";
        if(token.token_type == EQUAL){
             token = lexer.GetToken();  
             //cout << "\n token name " << token.lexeme << " \n";
             if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA || token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == LESS || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL || token.token_type == NOT){
                lexer.UngetToken(token);
               rightType= parse_expression();
			   if (leftType == 1 || leftType == 2 || leftType == 3)
			   {
				   if (leftType == rightType)
				   {

				   }
				   else {
					   if (leftType == 1 && rightType == 2)
					   {
						   cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
						   exit(1);
					   }
                       else if (leftType == 2 && rightType == 1)
					   {
						   cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
						   exit(1);
					   }
                       else if (leftType == 3 && rightType <= 2)
                       {
                           cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
                           exit(1);
                       }
                       else if (leftType <= 2 && rightType == 3)
                        {
                           cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
                           exit(1);
                       }
					   else {
						   helper(rightType, leftType);
						   rightType = leftType;
					   }
				   }
			   }
			   else {
				   helper(leftType, rightType);
				   leftType = rightType;
			   }
                //rType right type of an assigment tempI.
                //check for C1. if ltype == rtype then fine if not then TYPE MISMATCH token.line_no C1
                // if any one of lType or rType is -1 then should not throw type mismatch. 
                // if lType != -1 && rType is -1 then you search for left ID token to extract its type. searchList should return type. 
                // you have to use search list again with the right token to update the right token's type to lType 
                token = lexer.GetToken();
                //token.Print();
                if(token.token_type == SEMICOLON){
                    //cout << "\n Rule parsed: assignment_stmt -> ID EQUAL expression SEMICOLON \n";
                
                }else{
                     cout << "\n HI Syntax Error " << token.token_type << " \n";    
                 }
             }else{
                 cout << "\n Syntax Error \n";    
             }  
        }else{
            cout << "\n Syntax Error \n";    
        }  
        
    }else{
        cout << "\n Syntax Error \n";    
    }
    return(0);
}

int parse_case(void){

    int tempI;
    token = lexer.GetToken();
    if(token.token_type == CASE ){
        token = lexer.GetToken();
        if(token.token_type == NUM){
            token = lexer.GetToken();
            if(token.token_type == COLON){
                //cout << "\n Rule parsed: case -> CASE NUM COLON body";
                tempI = parse_body();
            }else{
                cout << "\n Syntax Error \n";
            }
        
        }else{
            cout << "\n Syntax Error \n";
        }
    
    }else{
        cout << "\n Syntax Error \n";
    }
}

int parse_caselist(void){
    
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == CASE){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_case();
        token = lexer.GetToken();
        if(token.token_type == CASE){
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule parsed: case_list -> case case_list \n";
            tempI = parse_caselist();
        }else if(token.token_type == RBRACE){
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule parsed: case_list -> case  \n";
        }
    }
    return(0);
}


int parse_switchstmt(void){
    int tempI;
    
    token = lexer.GetToken();
    if(token.token_type == SWITCH){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();
            // if tempI != INT then throw type error
            // else if tempI = -1 ==> parse_expresssion retunred an ID, then go and change using searchList the type of ID to 1.
            if(tempI >= 4) {

                helper(tempI, 1);
                tempI = 1;
            }
			if (tempI <= 3 && tempI != 1)
			{
				cout << "TYPE MISMATCH " << token.line_no << " C5" << endl;
				exit(1);
			}
            token = lexer.GetToken();
            if(token.token_type == RPAREN){
                token = lexer.GetToken();
                if(token.token_type == LBRACE){
                    tempI = parse_caselist();
                    token = lexer.GetToken();
                    if(token.token_type == RBRACE){
                        //cout << "\n Rule parsed: switch_stmt -> SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE \n";        
                    }else{
                        cout << "\n Syntax Error \n";
                    }   
                }else{
                    cout << "\n Syntax Error \n";
                }
                
            }else{
                cout << "\n Syntax Error \n";
            }
        }else{
            cout << "\n Syntax Error \n";
        }    
    }else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}


int parse_whilestmt(void){
    int tempI;
    
    token = lexer.GetToken();
    if(token.token_type == WHILE){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();
			
            if(tempI >= 4) {

                helper(tempI, 3);
                tempI = 3;
            }
            
            if (tempI != 3)
			{
				cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
				exit(1);
			}
            // if tempI != bool then throw type error
            // else if tempI = -1 ==> parse_expresssion retunred an ID, then go and change using searchList the type of ID to 2.
            token = lexer.GetToken();
            if(token.token_type == RPAREN){
                //cout << "\n Rule parsed: whilestmt -> WHILE LPAREN expression RPAREN body \n";
                tempI = parse_body();
                
            }else{
                cout << "\n Syntax Error \n";
            }
        }else{
            cout << "\n Syntax Error \n";
        }    
    }else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_ifstmt(void){
    int tempI;
    
    token = lexer.GetToken();
    if(token.token_type == IF){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();

            //cout << "tempI Value in parse_ifstmt " << tempI << endl;

            // if tempI != bool then throw type error
            // else if tempI = -1 ==> parse_expresssion retunred an ID, then go and change using searchList the type of ID to 2.
			if(tempI >= 4) {

                helper(tempI, 3);
                tempI = 3;
            }
            
            if (tempI != 3)
			{
				cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
				exit(1);
			}

            token = lexer.GetToken();
            if(token.token_type == RPAREN){
                //cout << "\n Rule parsed: ifstmt -> IF LPAREN expression RPAREN body \n";
                tempI = parse_body();
                
            }else{
                cout << "\n Syntax Error \n";
            }
        }else{
            cout << "\n Syntax Error \n";
        }    
    }else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmt(void){
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> assignment_stmt \n";
        tempI = parse_assstmt();        
                
    }else if(token.token_type == IF){
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> if_stmt";
        tempI = parse_ifstmt();
    }else if(token.token_type == WHILE){
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> while_stmt";
        tempI = parse_whilestmt();
    }else if(token.token_type == SWITCH){
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> switch_stmt";
        tempI = parse_switchstmt();
    }else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmtlist(void){
    
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_stmt();
        token = lexer.GetToken();
        //token.Print();
        if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH){
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: stmt_list -> stmt stmt_list \n";
            tempI = parse_stmtlist();
            
        }else if (token.token_type == RBRACE){
            tempTokenType = lexer.UngetToken(token);
            //printList();
            //cout << "\n Rule parsed: stmt_list -> stmt \n";
        }
    }else if(token.token_type == RBRACE) {

    }else{
        cout << "\n Syntax Error \n";
        
    }
    return(0);
}

int parse_body(void){
    
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    // currentScope = (char*)malloc(sizeof(token.lexeme)+1); // different from global have any arbitrary string "h"
    //memcpy(currentScope, (token.lexeme).c_str(), (token.lexeme).size()+1);
   // addScope();
    //strcpy(currentScope, lexeme);
    
        if(token.token_type == LBRACE){
            //cout << "\n Rule Parsed: scope -> ID LBRACE public_vars private_vars stmt_list RBRACE \n";
            
            tempI = parse_stmtlist();
            token = lexer.GetToken();
            //token.Print();
            if(token.token_type == RBRACE){
                //if(symbolTable != NULL){
                    
                  //  deleteList();
                //}
                //printScope();
                //deleteScope();
                //cout << "\n Rule parsed: body -> LBRACE stmt_list RBRACE \n";
                return(0);
            }else if(token.token_type == END_OF_FILE) {
                
            }else{
                cout << "\n Syntax Error\n ";
                return(0);
            }    
                    
        
        }else if(token.token_type == END_OF_FILE){
        tempTokenType = lexer.UngetToken(token);
        return(0);
        }else{
            cout << "\n Syntax Error \n ";
            return(0);
        }

}

// parse scope end

int parse_typename(void){
    token = lexer.GetToken();
    if(token.token_type == INT)
    {
        comp(token.line_no, 1);
    }
    else if(token.token_type == REAL)
    {
        comp(token.line_no, 2);
    } 
    else if (token.token_type == BOO)
    {
        comp(token.line_no, 3);
    }else{
        cout << "\n Syntax Error \n";
    }   
    return(0);// if Int ret 0 if float ret 1 if bool ret 2
}

int parse_vardecl(void){
        int tempI;
    token = lexer.GetToken();
    if(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_varlist();
        token = lexer.GetToken();
        if(token.token_type == COLON){
            tempI = parse_typename();

            //use the searchList to update the types of variables that are already in the symbolTable
           

            token = lexer.GetToken();
            if(token.token_type == SEMICOLON){
                //cout << "\n Rule parsed: var_decl -> var_list COLON type_name SEMICOLON";
            }else{
                cout << "\n Syntax Error \n";
            }
        } else{
            cout << "\n Syntax Error \n";
        } 
    }else{
        cout << "\n Syntax Error \n";
    
    }
    return(0);
}

int parse_vardecllist(void){
    int tempI;    
    token = lexer.GetToken();
    while(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_vardecl();
        token = lexer.GetToken();
        if(token.token_type != ID){
            //cout << "\n Rule Parsed: var_decl_list -> var_decl \n";
            
        }else{
            //cout << "\n Rule Parsed: var_decl_list -> var_decl var_decl_list \n";
        }
    }
    tempTokenType = lexer.UngetToken(token);
    return(0);
}


string global = "::";
// parse global vars
int parse_globalVars(void){
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    
    //check first set of var_list SEMICOLON
    if(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        //currentPrivPub = 0;
        //strcpy(currentScope,global);
        //cout << "\n Rule parsed: globalVars -> var_decl_list \n";
        tempI = parse_vardecllist();
        
    
    } else{
        cout << "Syntax Error";
    }
    return(0);

}

int parse_program(void){
    
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    while (token.token_type != END_OF_FILE)
    {
        // Check first set of global_vars scope
            if(token.token_type == ID){
            tempTokenType = lexer.UngetToken(token);
            //token1.Print();
            //cout << "\n Rule parsed: program -> global_vars scope \n";
            tempI = parse_globalVars();
            tempI = parse_body();
                  
            }else if(token.token_type == LBRACE){
                tempTokenType = lexer.UngetToken(token);
                
                //cout << "\n Rule parsed: global_vars -> epsilon \n";
                tempI = parse_body();
                
                
        
            }else if(token.token_type == END_OF_FILE){
                return(0);
            }else{
                cout << "\n Syntax Error\n";
                return(0);
            }
        token = lexer.GetToken();
        //token.Print();
    }


}
char null[] = "NULL";
int main()
{
    int i;
    //symbolTable = (sTable *) malloc(sizeof(sTable));ass
    //symbolTable = NULL;
    //scopeTable = NULL;
    //currentScope = (char *)malloc(sizeof(global)+1);
    //memcpy(currentScope,global.c_str(),global.length()+1);
    //addScope();
        i = parse_program();
        printList();
        //cout << "\n End of Program \n";
    //printList();
    //free(symbolTable);
    //token = lexer.GetToken();
    //cout << "\n Token value " << token.lexeme << "\n";
    //token.Print();
    //while(token.token_type != END_OF_FILE){
        //cout << "\n Token value " << token.token_type << "\n";
     //   token = lexer.GetToken();
        //cout << "\n Token Type " << token.token_type << " \n";
       // token.Print();
    //}
}