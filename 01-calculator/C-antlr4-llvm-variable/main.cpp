#include <iostream>

#include "antlr4-runtime.h"
#include "calculatorLexer.h"
#include "calculatorParser.h"

#include "myCalculatorVisitor.h"

#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/GenericValue.h"

using namespace itas109;
using namespace antlr4;

int main(int argc, const char *argv[])
{
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter(); // LLVM ERROR: Target does not support MC emission!


    const char *sourceCode = R"(
let a = 1;
let b = 2;
let ret = a + b * 3;
return ret;
)";
    ANTLRInputStream input(sourceCode);
    calculatorLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    calculatorParser parser(&tokens);
    tree::ParseTree *tree = parser.program();

    std::string stringTree = tree->toStringTree(&parser);
    printf("Input: %s\nParse Tree: %s\n\n", sourceCode, stringTree.c_str());

    myCalculatorVisitor visitor;
    visitor.visit(tree);

    auto module = visitor.getModule();
    module->print(llvm::outs(), nullptr);

    // execution eginge
    llvm::ExecutionEngine *engine = llvm::EngineBuilder(std::move(module)).setEngineKind(llvm::EngineKind::JIT).setMCJITMemoryManager(std::unique_ptr<llvm::SectionMemoryManager>()).create();
    // main function pointer
    typedef int (*funcMainType)();
    funcMainType mainFunc = (funcMainType)engine->getFunctionAddress("main");
    // call main function
    printf("main result:%d\n", mainFunc());

    printf("\nPress Enter To Continue\n");
    (void)getchar();

    return 0;
}