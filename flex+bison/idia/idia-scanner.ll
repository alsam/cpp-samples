%{

#include <iostream>
#include <fstream>
#include <sstream>
#include "errmsg.h"
#include "geom.h"
#include "idia-parser.hpp"

fileinfo* fi;
int tokPos;
int charPos;

extern "C" int yywrap(void)
{
  charPos=1;
  return 1;
}

void setlexer(std::string const& filename) 
{
  if (filename != "") {
    fi = new fileinfo(filename);
    yyin = fopen(filename.c_str(), "r");
  } else {
    fi = new fileinfo("<stdin>");
    yyin = stdin;
  }
  tokPos = charPos = 1;
}

position lexerPos()
{
  position p;
  p.init(fi, tokPos);
  return p;
}
  
namespace {
position here() { return lexerPos(); }

void adjust()
{
  tokPos = charPos;
  charPos += yyleng;
  yylval.pos = here();
} 

void makesymbol(bool op=true, std::string text = yytext)
{
  adjust();
//  yylval.ps.pos=here();
//  yylval.ps.sym=op ? symbol::opTrans(text) : symbol::literalTrans(text);
}

void newline()
{
  fi->newline();
  charPos = tokPos = 1;
}


Number_type atoq(const char *s)
{
  Number_type a = 0;
  int e = 0;
  int c;
  while ((c = *s++) != '\0' && isdigit(c)) {
    a = a*10 + (c - '0');
  }
  if (c == '.') {
    while ((c = *s++) != '\0' && isdigit(c)) {
      a = a*10 + (c - '0');
      e = e-1;
    }
  }
  if (c == 'e' || c == 'E') {
    int sign = 1, i = 0;
    c = *s++;
    if (c == '+')
      c = *s++;
    else if (c == '-') {
      c = *s++;
      sign = -1;
    }
    while (isdigit(c)) {
      i = i*10 + (c - '0');
      c = *s++;
    }
    e += i*sign;
  }
  while (e > 0) {
    a *= 10;
    e--;
  }
  while (e < 0) {
    a /= 10;
    e++;
  }
  return a;
}



}
%}

LETTER [_A-Za-z]
DIGIT  [0-9]
SP     [ \t\f\v]
ENDL \\?(\r\n|\n|\r)

%%

{SP}               {adjust(); continue;}
{ENDL}             {adjust(); newline(); continue;}
\/\/[^\n]*         {adjust(); continue;}




","                {adjust(); return ','; }
":"                {adjust(); return ':'; }
";"                {adjust(); return ';'; }
"("                {adjust(); return '('; }
")"                {adjust(); return ')'; }
"["                {adjust(); return '['; }
"]"                {adjust(); return ']'; }
"="                {adjust(); return '='; }

"+"                {makesymbol(); return '+'; }
"-"                {makesymbol(); return '-'; }
"*"                {makesymbol(); return '*'; }
"/"                {makesymbol(); return '/'; }
"%"                {makesymbol(); return '%'; }
"^"                {makesymbol(); return '^'; }
"**"               {makesymbol(true,"^"); return '^'; }
"--"               {makesymbol(); return JOIN; }

"print"            {makesymbol(); return PRINT;}
"params"           {makesymbol(); return PARAMS;}
"boundaries"       {makesymbol(); return BOUNDARIES;}
"dirichlet"        {makesymbol(); return DIRICHLET;}
"planars"          {makesymbol(); return PLANARS;}
"conformals"       {makesymbol(); return CONFORMALS;}
"conductors"       {makesymbol(); return CONDUCTORS;}
"eps"              {makesymbol(); return EPS;}
"upto"             {makesymbol(); return UPTO;}
"diel"             {makesymbol(); return DIEL;}
"net"              {makesymbol(); return NET;}
"shape"            {makesymbol(); return SHAPE;}

{DIGIT}*\.?{DIGIT}+([eE][-+]?{DIGIT}+)? {
  adjust();
  //yylval.dval=atof(yytext);
  yylval.dval = new Number_type(atoq(yytext));

  dlog() << "matched number: " << yytext << " : " << *yylval.dval << std::endl;
  return NUM; }


{LETTER}({LETTER}|[0-9])* {adjust(); yylval.id=new std::string(yytext); return ID; }

