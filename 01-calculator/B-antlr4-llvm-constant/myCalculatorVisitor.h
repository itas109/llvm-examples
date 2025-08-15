#ifndef MY_CALCULATOR_VISITOR_H
#define MY_CALCULATOR_VISITOR_H

#include "calculatorBaseVisitor.h" // visitor

// llvm
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

using namespace itas109;
using namespace antlr4;

class myCalculatorVisitor : public calculatorBaseVisitor
{
public:
    myCalculatorVisitor();
    ~myCalculatorVisitor() = default;

    std::unique_ptr<llvm::Module> getModule()
    {
        return std::move(m_llvmModule);
    }

    llvm::IRBuilder<> *getBuilder()
    {
        return m_llvmBuilder.get();
    }

    virtual std::any visitProgram(calculatorParser::ProgramContext *ctx) override;
    virtual std::any visitMulDivExpr(calculatorParser::MulDivExprContext *ctx) override;
    virtual std::any visitParensExpr(calculatorParser::ParensExprContext *ctx) override;
    virtual std::any visitAddSubExpr(calculatorParser::AddSubExprContext *ctx) override;
    virtual std::any visitIntegerLiteral(calculatorParser::IntegerLiteralContext *ctx) override;

private:
    std::unique_ptr<llvm::LLVMContext> m_llvmContext;
    std::unique_ptr<llvm::Module> m_llvmModule;
    std::unique_ptr<llvm::IRBuilder<>> m_llvmBuilder;
};

 #endif