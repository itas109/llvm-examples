#include "myCalculatorVisitor.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"

myCalculatorVisitor::myCalculatorVisitor()
    : m_llvmContext(std::make_unique<llvm::LLVMContext>())
    , m_llvmModule(std::make_unique<llvm::Module>("calculator", *m_llvmContext))
    , m_llvmBuilder(std::make_unique<llvm::IRBuilder<>>(*m_llvmContext))
{
}

std::any myCalculatorVisitor::visitProgram(calculatorParser::ProgramContext *ctx)
{
    // main
    auto *FuncType = llvm::FunctionType::get(m_llvmBuilder->getInt32Ty(), false);
    auto *MainFunc = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "main", *m_llvmModule);

    // entry
    auto *BB = llvm::BasicBlock::Create(*m_llvmContext, "entry", MainFunc);
    m_llvmBuilder->SetInsertPoint(BB);

    // visit stmt
    for (auto stmt : ctx->stmt())
    {
        visit(stmt);
    }

    // return 0
    if (nullptr == m_llvmBuilder->GetInsertBlock()->getTerminator())
    {
        m_llvmBuilder->CreateRet(llvm::ConstantInt::get(*m_llvmContext, llvm::APInt(32, 0)));
    }

    return nullptr;
}

std::any myCalculatorVisitor::visitDeclarationStatement(calculatorParser::DeclarationStatementContext *ctx)
{
    return visit(ctx->varDecl());
}

std::any myCalculatorVisitor::visitExpressionStatement(calculatorParser::ExpressionStatementContext *ctx)
{
    return visit(ctx->expr());
}

std::any myCalculatorVisitor::visitParensExpr(calculatorParser::ParensExprContext *ctx)
{
    return visit(ctx->expr());
}

std::any myCalculatorVisitor::visitReturnStatement(calculatorParser::ReturnStatementContext *ctx)
{
    llvm::Value *value = std::any_cast<llvm::Value *>(visit(ctx->expr()));
    m_llvmBuilder->CreateRet(value);

    return value;
}

std::any myCalculatorVisitor::visitVarDecl(calculatorParser::VarDeclContext *ctx)
{
    std::string varName = ctx->ID()->getText();
    llvm::AllocaInst *variable = m_llvmBuilder->CreateAlloca(m_llvmBuilder->getInt32Ty(), nullptr, varName);
    llvm::Value *value = std::any_cast<llvm::Value *>(visit(ctx->expr()));
    m_llvmBuilder->CreateStore(value, variable);

    m_symbolTable[varName] = variable;

    return value;
}

std::any myCalculatorVisitor::visitIdentifier(calculatorParser::IdentifierContext *ctx)
{
    // get variable from symbol table
    std::string varName = ctx->ID()->getText();
    llvm::AllocaInst *alloca = m_symbolTable[varName];
    if (nullptr == alloca)
    {
        llvm::errs() << "Error: Undefined variable " << varName << "\n";
        return nullptr;
    }

    // load variable
    return (llvm::Value*)m_llvmBuilder->CreateLoad(alloca->getAllocatedType(), alloca, varName);
}

std::any myCalculatorVisitor::visitIntegerLiteral(calculatorParser::IntegerLiteralContext *ctx)
{
    int value = std::stoi(ctx->getText());
    return (llvm::Value *)m_llvmBuilder->getInt32(value);
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