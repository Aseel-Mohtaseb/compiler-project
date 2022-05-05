//compilerProject infile.txt outfile.txt
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 128
#define NONE -1
#define EOS '\0'
#define NUM 256//
#define DIV 257
#define MOD 258
#define ID 259 //
#define DONE 260
#define program 261// PROGRAM
#define infix 262
#define postfix 263
#define begin 264// BEGIN
#define end 265// END

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
#define RELOP 281
#define ADDOP 282
#define MULOP 283
#define NOT 284
// #define 

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
int numOfVars = 0;
int vars[];

int lexan();
void parse();
void expr();//
void term();
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

/*form of symbol table entry */
struct entry{
    char *lexptr;
    int token;
};

struct entry symtable[SYMMAX];

struct entry keywords[] = {
    "div", DIV,
    "mod", MOD,
    "program", program,
    "infix", infix,
    "postfix", postfix,
    "begin", begin,
    "end", end,

    // "id", ID,
    "input", INPUT,
    "output", OUTPUT,
    "CONST", CONST,
    "VAR", VAR,
    //EPSILON
    "integer", INTEGER,
    "real", REAL,
    "char", CHAR,
    "boolean", BOOLEAN,
    "if", IF,
    "then", THEN,
    "while", WHILE,
    "do", DO,
    "writeln", WRITELN,
    "else", ELSE,
    //
    //
    //
    "not", NOT,

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
  //block();

  
}

//Header -> program id(input,output) ;
void header(){
  if(lookahead == program){
      match(program);
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
  // id = num;
}

//ConstantDefinitionsRest -> ConstantDefinition ConstantDefinitionsRest  | <epsilon>
void constantDefinitionsRest(){
  constantDefinition();
  constantDefinitionsRest();

  //else return;
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

//VariableDeclaration -> IdentifierList : Type ;
void variableDeclaration(){
  identifierList();
  match(':');
  type();
  match(';');
}

// IdentifierList  -> id IdentifierListRest
void identifierList(){
  match(ID);
  identifierListRest();
}

// identifierListRest ->  , id IdentifierListRest | <epsilon>
void identifierListRest(){
  if(lookahead == ','){
    match(',');
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
    fprintf(outfile, "real ");
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


void expr()
{
  int t;
  term();
  while(1)
    switch(lookahead){
      case'+' :case '-':
        t=lookahead;
        match(lookahead);term();emit(t,NONE);
        continue;
      default:
        return;
    }
}

void term()
{
  int t;
  factor();
  while(1)
    switch(lookahead){
        case '*' : case '/': case DIV: case MOD: case '%': case '\\':
            t=lookahead;
            match(lookahead); factor(); emit(t,NONE);
            continue;
        default:
            return ;
    }
}
void factor(){
  switch(lookahead){
    case '(':
      match('(');expr();match(')');break;
    case NUM:
      emit(NUM,tokenval);match(NUM);break;
    case ID:
      emit(ID,tokenval);match(ID);break;
    default:
    error("syntax error in factor");
  }
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
            fprintf(outfile,"%d ", tval); 
            break;
        case ID:
            fprintf(outfile,"%s ", symtable[tval].lexptr);
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
