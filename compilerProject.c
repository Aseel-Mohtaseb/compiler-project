//compilerProject infile.txt outfile.txt
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 128
#define MAXTEMPSIZE 50
#define NONE -1
#define EOS '\0'
#define NUM 256
#define DIV 257
#define MOD 258
#define ID 259 
#define DONE 260
#define PROGRAM 261
#define BEGIN 264
#define END 265 

#define INPUT 266
#define OUTPUT 267
#define CONST 268
#define VAR 269
#define EPSILON 270
#define INTEGER 271
#define REAL 272
#define CHAR 273
#define BOOLEAN 274
#define IF 275
#define THEN 276 
#define WHILE 277
#define DO 278
#define WRITELN 279
#define ELSE 280
#define NOT 281
#define GREATEREQUAL 282 //>=
#define LESSEQUAL 283 //<=
#define NOTEQUAL 284 //<>
#define OR 285
#define AND 286

#define STRMAX 999
#define SYMMAX 100

FILE *infile, *outfile;

int tokenval = NONE;
int lineno=1;
char lexbuf[BSIZE];
int lookahead;
char lexemes[STRMAX];
int lastchar = -1;
int lastentry = 0;
int tempIndex = 0;
int temp[MAXTEMPSIZE];// To store tokenVals

int lexan();
void parse();
void term();
void termRest();
void factor();

void match(int t);
void emit(int t, int tval);
int insert(char s[], int tok);
void init();
void error(char *m);  
int lookup(char s[]);
void openFiles(char* infileName, char* outfileName);

void header();
void declarations();
void variableDeclarations();
void constantDefinitions();
void declarationsPrime();
void declarationsPrime();
void constantDefinitions();
void constantDefinition();
void constantDefinitionsRest();
void variableDeclarations();
void variableDeclarationsRest();
void variableDeclaration();
void identifierList();
void identifierListRest();
void type();
void block();
void statements();
void statementsRest();
void statement();
void elseClause();
void expressionList();
void expressionListRest();
void expression();
void expressionPrime();
void simpleExpression();
void simpleExpressionRest();

/*form of symbol table entry */
struct entry{
    char *lexptr;
    int token;
};

struct entry symtable[SYMMAX];

struct entry keywords[] = {
    "DIV", DIV,
    "MOD", MOD,
    "PROGRAM", PROGRAM,
    "BEGIN", BEGIN,
    "END", END,
    "Input", INPUT,
    "Output", OUTPUT,
    "CONST", CONST,
    "VAR", VAR,
    "integer", INTEGER,
    "real", REAL,
    "char", CHAR,
    "boolean", BOOLEAN,
    "IF", IF,
    "THEN", THEN,
    "WHILE", WHILE,
    "DO", DO,
    "WRITELN", WRITELN,
    "ELSE", ELSE,
     ">=", GREATEREQUAL,
    "<=", LESSEQUAL,
    "<>", NOTEQUAL,
    "OR", OR,
    "not", NOT,
    "AND", AND,
    0, 0,
};

int lexan(){
  int t;
  while(1){
    t=getc(infile);
    if (t==' '|| t=='\t');
    else if (t=='\n')
      lineno=lineno+1;
    else if(isdigit(t)){
      ungetc(t,infile);
      fscanf(infile ,"%d",&tokenval);
      return NUM;
    } 
    else if (isalpha(t)){
      int p,b=0;
      while (isalnum(t)){
        lexbuf[b]=t;
        t=getc(infile);
        b = b+1;
        if(b>=BSIZE)
          error("compiler error");
      }
      lexbuf[b]=EOS;
      if(t!=EOF){
        ungetc(t,infile);
      }
      p=lookup(lexbuf);
      if (p==0){
        p=insert(lexbuf,ID);
      }

      tokenval = p;
      return symtable[p].token;
    }
    else if (t==EOF)
      return DONE;
      
    else if (t == '#') 
        while (t != '\n')
            t = getc(infile);
    else {
      tokenval = NONE;
      return t;
    }
  }
}


//Program -> Header Declarations Block.
//parese = program
void parse()
{
  lookahead = lexan();
    // if(lookahead == program){
    //     match(program);
    //     match(ID);
    //     match('(');
    //     match(infix);
    //     match(',');
    //     match(postfix);
    //     match(')');
    //     match(begin);

    //     fprintf(outfile,"program ");
    //     fprintf(outfile,"%s", symtable[8].lexptr);
    //     fprintf(outfile,"(infix,postfix) \nbegin\n");

    //     while(lookahead!=end){
    //         expr();match(';');
    //         emit(';',NONE);
    //     }
    //     match(end); 
    //     fprintf(outfile,"end");
    // }
    // else {
    //     error("syntax errorr");
    // }

  header();
  declarations();
  fprintf(outfile, "\nvoid main(void)");
  block();  

}

//Header -> program id(input,output) ;
void header(){
  if(lookahead == PROGRAM){
      match(PROGRAM);
  }else error("'PROGRAM' is missing");
  if(lookahead == ID){
      match(ID);
  }else error("ID is missing");
  if(lookahead == '('){
      match('(');
  }else error("'(' is missing");
  if(lookahead == INPUT){
    match(INPUT);
  }else error("'Input' is missing");
  if(lookahead == ','){
    match(',');
  }else error("',' is missing");
  if(lookahead == OUTPUT){
    match(OUTPUT);
  }else error("'Output' is missing");
  if(lookahead == ')'){
    match(')');
  }else error("')' is missing");
  if(lookahead == ';'){
    match(';');
  }else error("';' is missing");

  fprintf(outfile, "#include <iostream>\nusing namespace std;\n");

}

/*
Declarations -> VAR VariableDeclarations |
	CONST ConstantDefinitions DeclarationsPrime
	| <epsilon>
*/
void declarations(){
  if (lookahead == VAR)
  {
    match(VAR);
    variableDeclarations();
  }
  else if (lookahead == CONST){
    match(CONST);
    constantDefinitions();
    declarationsPrime();
  }
  else return;
  

}

//DeclarationsPrime -> VAR VariableDeclarations | <epsilon>
void declarationsPrime(){
  if (lookahead == VAR)
  {
    match(VAR);
    variableDeclarations();
  }
  else return;
  
}

//ConstantDefinitions -> ConstantDefinition ConstantDefinitionsRest
void constantDefinitions(){
  constantDefinition();
  constantDefinitionsRest();
}

//ConstantDefinition -> id=num;
void constantDefinition(){
  fprintf(outfile, "const double ");
  emit(ID, tokenval);
  match(ID);
  match('=');
  fprintf(outfile, "=");
  emit(NUM, tokenval);
  match(NUM);
  fprintf(outfile, ";\n");
  match(';');
  
}

//ConstantDefinitionsRest -> ConstantDefinition ConstantDefinitionsRest  | <epsilon>
void constantDefinitionsRest(){
  if (lookahead == ID)
  {
    constantDefinition();
    constantDefinitionsRest();
  }else return;
}

// VariableDeclarations  -> VariableDeclaration VariableDeclarationsRest
void variableDeclarations(){
  variableDeclaration();
  variableDeclarationsRest();
}

// VariableDeclarationsRest -> VariableDeclaration VariableDeclarationsRest | <epsilon>
void variableDeclarationsRest(){
  if (lookahead == ID) // not sure
  {  
    variableDeclaration();
    variableDeclarationsRest();
  }else return;
}

//X,Y,Z: integer; A,B: real; C: char; F: boolean;


//VariableDeclaration -> IdentifierList : Type ;
void variableDeclaration(){
  tempIndex = 0;
  identifierList();
  match(':');
  type();
  match(';');
  for (int i = 0; i < tempIndex; i++)
  {
    emit(ID, temp[i]);
    if (i != tempIndex-1)
      fprintf(outfile, ",");
    else
      fprintf(outfile, ";");
  }

}

// IdentifierList  -> id IdentifierListRest
void identifierList(){
  temp[tempIndex] = tokenval;
  tempIndex++;
  match(ID);
  identifierListRest();
}

// identifierListRest ->  , id IdentifierListRest | <epsilon>
void identifierListRest(){
  if(lookahead == ','){
    match(',');
    temp[tempIndex] = tokenval;
    tempIndex++;
    match(ID);
    identifierListRest();
  }
  else return;
}

//Type -> integer | real | char | boolean
void type(){
  if (lookahead == INTEGER)
  {
    match(INTEGER);
    fprintf(outfile, "int ");
  }
  else if (lookahead == REAL)
  {
    match(REAL);
    fprintf(outfile, "float ");
  }
  else if (lookahead == CHAR)
  {
    match(CHAR);
    fprintf(outfile, "char ");
  }
  else if (lookahead == BOOLEAN)
  {
    match(BOOLEAN);
    fprintf(outfile, "bool ");
  }
  else error("Syntax error in type");
  
}

//Block  ->  begin Statements end
void block(){
  fprintf(outfile, "\n{");
  match(BEGIN);
  statements();  
  fprintf(outfile, "\n}");
  match(END);
}

// Statements  ->   Statement StatementsRest
void statements(){
  statement();
  statementsRest();
}

// StatementsRest -> ;  Statement StatementsRest | <epsilon>
void statementsRest(){
  if (lookahead == ';')
  {
    match(';');
    statement();
    statementsRest();
  }else return;
  
}

/*
Statement -> id := Expression
                        | Block
                        | if Expression then Statement ElseClause
                        | while Expression do Statement
                        | writeln(SimpleExpression)
                        | <epsilon>
*/
void statement(){
  fprintf(outfile, "\n\t");
  if (lookahead == ID){
    emit(ID, tokenval);
    match(ID);
    fprintf(outfile, "=");
    match(':');
    match('=');
    expression();
    fprintf(outfile, ";");
  } else if(lookahead == BEGIN){
    block();
  } else if(lookahead == IF){
    fprintf(outfile, "if (");
    match(IF);
    expression();
    fprintf(outfile, ")");
    match(THEN);
    statement();
    elseClause();
  } else return;
  
}

//ElseClause -> else Statement | <epsilon>
void elseClause(){
  if (lookahead == ELSE){
    fprintf(outfile, "else ");
    match(ELSE);
    statement();
  } else return;
  
}

// ExpressionList -> Expression ExpressionListRest
void expressionList(){
  expression();
  expressionListRest();
}

// ExpressionListRest ->  , Expression ExpressionListRest | <epsilon>
void expressionListRest(){
  if(lookahead == ','){
    fprintf(outfile, ", ");
    match(',');
    expression();
    expressionListRest();
  } else return;
}

// Expression -> SimpleExpression ExpressionPrime
void expression(){
  simpleExpression();
  expressionPrime();
}

// ExpressionPrime -> relop SimpleExpression  | <epsilon>
void expressionPrime(){
  switch(lookahead){
    case '=': case '>': case '<': case GREATEREQUAL: case LESSEQUAL: case NOTEQUAL:
      emit(lookahead, NONE);
      match(lookahead);
      simpleExpression();
      break;
    default:
      return;
  }
}

// SimpleExpression -> Term SimpleExpressionRest | addop Term SimpleExpressionRest
void simpleExpression(){
  switch(lookahead){
    case '+': case '-': case OR:
      emit(lookahead, NONE);
      match(lookahead);
      term();
      simpleExpressionRest();
      break;
    case ID: case NUM: case '(': case NOT: //factor
      term();
      simpleExpressionRest();
      break;
    default: 
      error("error in simpleExpression");
    }
}

// SimpleExpressionRest -> addop Term SimpleExpressionRest | <epsilon>
void simpleExpressionRest(){
  switch (lookahead){
    case '+': case '-': case OR:
      emit(lookahead, NONE);
      match(lookahead);
      term();
      simpleExpressionRest();
      break;
    default:
      return;
  }
}
// Term -> Factor TermRest
void term(){
  factor();
  termRest();
}

// TermRest ->  mulop Factor TermRest | <epsilon>
void termRest(){
  switch (lookahead){
    case '*': case '/': case DIV: case MOD: case AND:
      emit(lookahead, NONE);
      match(lookahead);
      factor();
      termRest();
      break;
    default:
      return;
  }
}

// Factor -> id | num | ( Expression ) | not Factor 
void factor(){
  if (lookahead == ID){
    emit(ID, tokenval);
    match(ID);
  } else if (lookahead == NUM){
    emit(NUM, tokenval);
    match(NUM);
  } else if (lookahead == '('){
    match('(');
    fprintf(outfile, "(");
    expression();
    match(')');
    fprintf(outfile, ")");
  } else if (lookahead == '('){
    match('(');
    fprintf(outfile, "(");
    expression();
    match(')');
    fprintf(outfile, ")");
  } else if (lookahead == NOT){
    match(NOT);
    fprintf(outfile, "!");
    factor();
  } else error("Error in factor");
}

void match(int t)
{
  if (lookahead==t){
    lookahead=lexan();
  }
  else error ("syntax error in match");
}

void emit(int t, int tval)
{
    switch(t) {
        case '+': case '-': case '*': case'/': case '%': case '\\':
            fprintf(outfile,"%c ", t); break;
        case ';':
            fprintf(outfile,"%c \n", t); break;
        case DIV:
            fprintf(outfile,"DIV "); break;
        case MOD:
            fprintf(outfile,"MOD "); break;
        case NUM:
            fprintf(outfile,"%d", tval); 
            break;
        case ID:
            fprintf(outfile,"%s", symtable[tval].lexptr);
            break;
        default:
            fprintf(outfile,"token %d, tokenval %d ", t, tval);
    }
}


int lookup(char s[])
{
    int p;
    for (p = lastentry ; p>0 ; p = p-1)
        if (strcmp(symtable[p].lexptr, s) == 0)
            return p;
    return 0;
}

int insert(char s[], int tok)
{
    int len;
    len = strlen(s);
    if (lastentry + 1 >= SYMMAX)
        error("symbol table full");
    if (lastchar + len + 1 >= STRMAX)
        error("lexemes array full");
    lastentry = lastentry + 1;
    symtable[lastentry].token = tok;
    symtable[lastentry].lexptr = &lexemes[lastchar + 1];
    lastchar = lastchar + len + 1;
    strcpy(symtable[lastentry].lexptr, s);
    return lastentry;
}

// loads keywords into symtable
void init()
{ 
    struct entry *p;
    for (p = keywords; p->token; p++)
        insert(p->lexptr, p->token);
}

void error(char *m)
{
    fprintf(stderr, "line %d: %s\n", lineno, m);
    exit(1);
}

void openFiles(char* infileName, char* outfileName)
{
    //to read 
    infile = fopen(infileName, "r");

    if(infile == NULL)
    error("Input file name does not exist");
    
    //to write 
    outfile = fopen(outfileName, "w");

}



int main(int argc, char **argv)
{
    openFiles(argv[1], argv[2]);
    init();
    parse();
    fclose(infile);
    fclose(outfile);
    exit(0); 
    return 0;
}
