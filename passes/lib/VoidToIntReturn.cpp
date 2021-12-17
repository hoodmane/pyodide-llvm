#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
  struct VoidToIntReturn : public llvm::PassInfoMixin<VoidToIntReturn> {
    llvm::PreservedAnalyses run(llvm::Module &M,
                                llvm::ModuleAnalysisManager &);
    bool runOnModule(llvm::Module &M);
  };


  void
  handleFunction(Module &M, Function *F){
    auto &CTX = M.getContext();

    auto F_type = F->getFunctionType();
    // new type is same as old type except return value is replaced with int32
    auto newF_type = FunctionType::get(Type::getInt32Ty(CTX), F_type->params(), F_type->isVarArg());

    // Use empty name for now, we'll rename it soon.
    Function *newF =
      Function::Create(newF_type, Function::ExternalLinkage, "", M);

    // Setup for CloneFunctionInto.
    // ValueToValueMapTy it uses to convert values from the original function to the clone.
    // We need to instantiate it to map each argument to a new unbound argument
    ValueToValueMapTy VMap;
    for(auto &A : F->args()){
      VMap[&A] = UndefValue::get(A.getType());
    }

    // Return instructions in cloned function body (convenient that it returns these)
    SmallVector<ReturnInst*, 4> Returns;

    CloneFunctionInto(newF, F, VMap, CloneFunctionChangeType::LocalChangesOnly, Returns);

    // Fix up return instructions: we need to replace 
    // return void;
    // with
    // return 0;
    auto zero = ConstantInt::getSigned(Type::getInt32Ty(CTX), 0);
    for(auto &R : Returns){
      // New return instruction is inserted directly before R.
      ReturnInst::Create(CTX, zero, R);
      R->eraseFromParent();
    }
    F->replaceAllUsesWith(newF);

    // We need name to outlast F, so add "" to it to force a copy.
    auto name = F->getName() + "";

    F->eraseFromParent();
    newF->setName(name);
  }

  bool VoidToIntReturn::runOnModule(Module &M) {
    std::vector<Function*> functions_to_modify;
    for (auto &F : M) {
      // Make a list of functions with void return types.
      if(!F.getReturnType()->isVoidTy()){
        continue;
      };
      functions_to_modify.push_back(&F);
    }
    for (auto F : functions_to_modify){
      handleFunction(M, F);
    }

    // return value is true if we changed stuff, false if not
    return !functions_to_modify.empty();
  }


  PreservedAnalyses VoidToIntReturn::run(llvm::Module &M,
                                        llvm::ModuleAnalysisManager &) {
    bool Changed =  runOnModule(M);

    return (Changed ? llvm::PreservedAnalyses::none()
                    : llvm::PreservedAnalyses::all());
  }

  llvm::PassPluginLibraryInfo getVoidToIntReturnPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "inject-func-call", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
              PB.registerPipelineEarlySimplificationEPCallback(
                  [](ModulePassManager & MPM, auto O) {
                    MPM.addPass(VoidToIntReturn());
                  });
            }};
  }
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getVoidToIntReturnPluginInfo();
}
