#include <iostream>

#include "antlr4-runtime.h"
#include "calculatorLexer.h"
#include "calculatorParser.h"

#include "myCalculatorVisitor.h"

using namespace itas109;
using namespace antlr4;

int main(int argc, const char *argv[])
{
    const char *sourceCode = "1+2*3";
    ANTLRInputStream input(sourceCode);
    calculatorLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    calculatorParser parser(&tokens);
    tree::ParseTree *tree = parser.program();

    std::string stringTree = tree->toStringTree(&parser);
    printf("Input: %s\nParse Tree: %s\n\n", sourceCode, stringTree.c_str());

    myCalculatorVisitor visitor;
    llvm::Value *result = std::any_cast<llvm::Value *>(visitor.visit(tree));

    // return
    auto module = visitor.getModule();
    auto *MainFunc = module->getFunction("main");
    if (MainFunc && !MainFunc->empty() && result)
    {
        visitor.getBuilder()->CreateRet(result);
    }

    module->print(llvm::outs(), nullptr);

    printf("\nPress Enter To Continue\n");
    (void)getchar();

    return 0;
}