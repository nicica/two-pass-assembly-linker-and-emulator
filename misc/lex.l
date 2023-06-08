%{
#include "pars.tab.h"

%}

%option noyywrap


%%

<<EOF>> {return EOFS;}
"$" {return DOL;}
"+" {return PLUS;}
"[" {return LB;}
"]" {return RB;}
[0-9]+ { yylval.name=malloc(strlen(yytext)+1); strcpy(yylval.name,yytext);return LIT; }
[0][x][a-fA-F0-9]{1,8} { yylval.name=malloc(strlen(yytext)+1); strcpy(yylval.name,yytext);return LIT; }
"%r0"|"%r1"|"%r2"|"%r3"|"%r4"|"%r5"|"%r6"|"%r7"|"%r8"|"%r9"|"%r10"|"%r11"|"%r12"|"%r13"|"%r14"|"%r15"|"%sp"|"%pc"|"%status"|"%handler"|"%cause" {
    yylval.name=malloc(strlen(yytext)+1);
    strcpy(yylval.name, yytext);
    return REG;
}
# {  }
#.+ {  }
[a-zA-Z0-9_]+: { yylval.name=malloc(strlen(yytext)+1); strcpy(yylval.name,yytext); yylval.name[strlen(yylval.name)-1]='\0'; return LABEL; }
".global"|".extern"|".section"|".word"|".skip"|".ascii"|".equ"|".end" { yylval.name=malloc(strlen(yytext)+1); strcpy(yylval.name,yytext);return ASM;}

"halt"|"int"|"iret"|"call"|"ret"|"jmp"|"beq"|"bne"|"bgt"|"push"|"pop"|"xchg"|"add"|"sub"|"mul"|"div"|"not"|"and"|"or"|"xor"|"shl"|"shr"|"ld"|"st"|"csrrd"|"csrwr" { yylval.name=malloc(strlen(yytext)+1); strcpy(yylval.name, yytext); return INS; }
"," { return COMA;}
[a-zA-Z0-9_]+ { yylval.name=malloc(strlen(yytext)+1); strcpy(yylval.name,yytext);return OPR; }
[ \t\r]+ { /* Ignore whitespace */ }
\n { return EOL; }

%%

