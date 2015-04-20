/*
 * File:  scanner.l
 * ----------------
 * Lex inupt file to generate the scanner for the compiler.
 */

%{
#include "simple.h"
#include <stdio.h>

    static int curLineNum, curColNum;
    static char curLine[512];
    static void DoBeforeEachAction();
#define YY_USER_ACTION DoBeforeEachAction();
    extern struct yyltype yylloc;
%}
%option noyywrap
%option stack
%s N C
%x COPY

DIGIT             ([0-9])
HEX_DIGIT         ([0-9a-fA-F])
HEX_INTEGER       (0[Xx]{HEX_DIGIT}+)
INTEGER           ({DIGIT}+)

BEG_STRING        (\"[^"\n]*)
STRING            ({BEG_STRING}\")
IDENTIFIER        ([a-zA-Z][a-zA-Z_0-9]*)
OPERATOR          ([+\-*/%=<>\\,.;!(){}\[\]])
BEG_COMMENT       ("/*")
END_COMMENT       ("*/")
SINGLE_COMMENT    ("//"[^\n]*)
WHITE             ([ \t]*)

%%             /* BEGIN RULES SECTION */
		      
<COPY>.*           {strncpy(curLine, yytext, sizeof(curLine));
			  curColNum = 1;
			  yy_pop_state(); yyless(0); }
<COPY><<EOF>>        { yy_pop_state(); }
<*>\n                { curLineNum++; curColNum = 1;
			   if(YYSTATE != COPY) yy_push_state(COPY);
                      } 
{WHITE}                { /* ignore space & tabs in normal or comment
			    */  
		     }


 /* -------------------- Comments ----------------------------- */
{BEG_COMMENT}          { yy_push_state(C); }
<C>{END_COMMENT}       { yy_pop_state(); }
<C><<EOF>>             { fprintf(stderr, "Input ends with unterminated comment.");
                                    return 0; }
<C>[^*\n/]*            { /* grab all non-star, non-slash, non-newline */}
<C>.                   { /* ignore everything else that doesn't match */ }
<N>{SINGLE_COMMENT}    { /* skip to end of line for // comment */ } 


 /* --------------------- Keywords ------------------------------- */
<N>"if"                { return T_If;          }
<N>"else"              { return T_Else;        }
<N>"print"             { return T_Print;       }
<N>"get_int"           { return T_ReadInteger; }
<N>"get_secret_int"    { return T_ReadSecretInt; }
<N>"assert"            { return T_Assert;      }
<N>"mem"               { return T_Mem; } 

 /* -------------------- Operators ----------------------------- */
<N>"<="                { return T_LessEqual;   }
<N>">="                { return T_GreaterEqual;}
<N>"=="                { return T_Equal;       }
<N>"!="                { return T_NotEqual;    }
<N>"&&"                { return T_And;         }
<N>"||"                { return T_Or;          }
<N>"="                 { return T_Assign; }
<N>"?"                 { return T_QM; }
<N>":"                 { return T_Colon; }
<N>{OPERATOR}          { return yytext[0];     }


 /* -------------------- Constants ------------------------------ */
<N>{INTEGER}           { yylval.integerConstant = strtol(yytext, NULL, 10);
                         return T_IntConstant; }
<N>{HEX_INTEGER}       { yylval.integerConstant = strtol(yytext, NULL, 16);
                         return T_IntConstant; }
<N>{STRING}            { yylval.stringConstant = strdup(&yytext[1]); 
    yylval.stringConstant[strlen(yytext)-2] = 0;
                         return T_StringConstant; }

 /* -------------------- Identifiers --------------------------- */
<N>{IDENTIFIER}        {
                        /* The lexer records the identifier name in yylval.
                         * The parser is reponsible for looking up the name
                         * in the appropriate scope(s) to find the decl.
                         */
                        yylval.identifier = strdup(yytext);

                         return T_Identifier; }


 /* -------------------- Default rule (error) -------------------- */
<N>.                   { fprintf(stderr,
                         "Unrecognized char: '%c at line %x character %x'", 
                        yytext[0], yylloc.first_line, yylloc.first_column); }

%%


void inityylex()
{
    yy_flex_debug =0;
    BEGIN(N);  // Start in Normal state
    yy_push_state(COPY);
    curLineNum = 1;
    curColNum = 1;
}

static void DoBeforeEachAction()
{
    yylloc.first_line = curLineNum;
    yylloc.first_column = curColNum;
    yylloc.last_column = curColNum + yyleng - 1;
    curColNum += yyleng;
}

const char *GetLineNumbered(int num) {
    return (num == curLineNum) ? curLine : NULL;
}
