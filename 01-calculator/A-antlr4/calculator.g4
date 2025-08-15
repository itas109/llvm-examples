grammar calculator;

program: expr EOF;

expr:
    expr ('*' | '/') expr   # MulDivExpr
    | expr ('+' | '-') expr # AddSubExpr
    | INT                   # IntegerLiteral
    | '(' expr ')'          # ParensExpr
    ;

// Lexer
LeftParen  : '(';
RightParen : ')';
Add        : '+';
Sub        : '-';
Mul        : '*';
Div        : '/';
INT        : [0-9]+;
WS         : [ \t\r\n]+ -> skip;