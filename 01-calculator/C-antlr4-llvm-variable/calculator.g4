grammar calculator;

program: stmt+ EOF;

stmt: 
    varDecl Semi         # declarationStatement 
    | expr Semi          # ExpressionStatement 
    | 'return' expr Semi # ReturnStatement
    ;

varDecl: 'let' ID '=' expr;

expr:
    expr ('*' | '/') expr   # MulDivExpr
    | expr ('+' | '-') expr # AddSubExpr
    | INT                   # IntegerLiteral
    | ID                    # Identifier
    | '(' expr ')'          # ParensExpr
    ;

// Lexer
INT        : [0-9]+;
ID         : [a-zA-Z_][a-zA-Z0-9_]*;
LeftParen  : '(';
RightParen : ')';
Add        : '+';
Sub        : '-';
Mul        : '*';
Div        : '/';
Semi       : ';';
WS         : [ \t\r\n]+ -> skip;