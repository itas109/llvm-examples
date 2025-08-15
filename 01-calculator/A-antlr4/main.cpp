#include <iostream>

#include "antlr4-runtime.h"
#include "calculatorLexer.h"
#include "calculatorParser.h"

#include "ParseTreeProperty.h" // annotate

#include "calculatorBaseVisitor.h" // visitor
#include "calculatorBaseListener.h" // listener

using namespace itas109;
using namespace antlr4;

class myCalculatorVisitor : public calculatorBaseVisitor
{
    virtual std::any visitProgram(calculatorParser::ProgramContext *ctx) override
    {
        return visit(ctx->expr());
    }

    virtual std::any visitMulDivExpr(calculatorParser::MulDivExprContext *ctx) override
    {
        int left = std::any_cast<int>(visit(ctx->expr(0)));
        int right = std::any_cast<int>(visit(ctx->expr(1)));
        if (ctx->Mul())
        {
            return left * right;
        }
        else if (ctx->Div())
        {
            return left / right;
        }

        return 0;
    }

    virtual std::any visitParensExpr(calculatorParser::ParensExprContext *ctx) override
    {
        return visit(ctx->expr());
    }

    virtual std::any visitAddSubExpr(calculatorParser::AddSubExprContext *ctx) override
    {
        int left = std::any_cast<int>(visit(ctx->expr(0)));
        int right = std::any_cast<int>(visit(ctx->expr(1)));
        if (ctx->Add())
        {
            return left + right;
        }
        else if (ctx->Sub())
        {
            return left - right;
        }

        return 0;
    }

    virtual std::any visitIntegerLiteral(calculatorParser::IntegerLiteralContext *ctx) override
    {
        return std::atoi(ctx->getText().c_str());
    }
};

class myCalculatorListener : public calculatorBaseListener
{
public:
    // annotate parse tree
    tree::ParseTreeProperty<int> result;
    void setValues(tree::ParseTree *node, int value)
    {
        result.put(node, value);
    }
    int getValues(tree::ParseTree *node)
    {
        return result.get(node);
    }

    virtual void exitProgram(calculatorParser::ProgramContext *ctx) override
    {
        setValues(ctx, getValues(ctx->expr()));
    }

    virtual void exitMulDivExpr(calculatorParser::MulDivExprContext *ctx) override
    {
        int left = getValues(ctx->expr(0));
        int right = getValues(ctx->expr(1));
        if (ctx->Mul())
        {
            setValues(ctx, left * right);
        }
        else if (ctx->Div())
        {
            setValues(ctx, left / right);
        }
    }

    virtual void exitParensExpr(calculatorParser::ParensExprContext *ctx) override
    {
        setValues(ctx, getValues(ctx->expr()));
    }

    virtual void exitAddSubExpr(calculatorParser::AddSubExprContext *ctx) override
    {
        int left = getValues(ctx->expr(0));
        int right = getValues(ctx->expr(1));
        if (ctx->Add())
        {
            setValues(ctx, left + right);
        }
        else if (ctx->Sub())
        {
            setValues(ctx, left - right);
        }
    }

    virtual void exitIntegerLiteral(calculatorParser::IntegerLiteralContext *ctx) override
    {
        setValues(ctx, std::atoi(ctx->getText().c_str()));
    }
};

int main(int argc, const char *argv[])
{
    const char *sourceCode = "1+2*3";
    ANTLRInputStream input(sourceCode);
    calculatorLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    calculatorParser parser(&tokens);
    tree::ParseTree *tree = parser.program();

    std::string stringTree = tree->toStringTree(&parser);
    printf("Input: %s\nParse Tree: %s\n", sourceCode, stringTree.c_str());

    myCalculatorVisitor visitor;
    int result = std::any_cast<int>(visitor.visit(tree));
    printf("result(visitor): %d\n", result);

    myCalculatorListener listener;
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
    printf("result(listener): %d\n", listener.getValues(tree));

    printf("\nPress Enter To Continue\n");
    (void)getchar();

    return 0;
}