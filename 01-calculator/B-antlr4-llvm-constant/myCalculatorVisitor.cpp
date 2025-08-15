#include "myCalculatorVisitor.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"

myCalculatorVisitor::myCalculatorVisitor()
    : m_llvmContext(std::make_unique<llvm::LLVMContext>())
    , m_llvmModule(std::make_unique<llvm::Module>("calculator", *m_llvmContext))
    , m_llvmBuilder(std::make_unique<llvm::IRBuilder<>>(*m_llvmContext))
{
    // main
    auto *FuncType = llvm::FunctionType::get(m_llvmBuilder->getInt32Ty(), false);
    auto *MainFunc = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "main", *m_llvmModule);

    // entry
    auto *BB = llvm::BasicBlock::Create(*m_llvmContext, "entry", MainFunc);
    m_llvmBuilder->SetInsertPoint(BB);
}

std::any myCalculatorVisitor::visitProgram(calculatorParser::ProgramContext *ctx)
{
    return visit(ctx->expr());
}

std::any myCalculatorVisitor::visitMulDivExpr(calculatorParser::MulDivExprContext *ctx)
{
    llvm::Value *left = std::any_cast<llvm::Value *>(visit(ctx->expr(0)));
    llvm::Value *right = std::any_cast<llvm::Value *>(visit(ctx->expr(1)));
    if (ctx->Mul())
    {
        llvm::Value *value = m_llvmBuilder->CreateMul(left, right, "mul");
        return value;
    }
    else if (ctx->Div())
    {
        llvm::Value *value = m_llvmBuilder->CreateSDiv(left, right, "div");
        return value;
    }

    return nullptr;
}

std::any myCalculatorVisitor::visitParensExpr(calculatorParser::ParensExprContext *ctx)
{
    return visit(ctx->expr());
}

std::any myCalculatorVisitor::visitAddSubExpr(calculatorParser::AddSubExprContext *ctx)
{
    llvm::Value *left = std::any_cast<llvm::Value *>(visit(ctx->expr(0)));
    llvm::Value *right = std::any_cast<llvm::Value *>(visit(ctx->expr(1)));

    if (ctx->Add())
    {
        llvm::Value * value = m_llvmBuilder->CreateAdd(left, right, "add");
        return value;
    }
    else if (ctx->Sub())
    {
        llvm::Value *value = m_llvmBuilder->CreateSub(left, right, "sub");
        return value;
    }

    return nullptr;
}

std::any myCalculatorVisitor::visitIntegerLiteral(calculatorParser::IntegerLiteralContext *ctx)
{
    int value =  std::atoi(ctx->getText().c_str());
    return (llvm::Value *)m_llvmBuilder->getInt32(value);
}