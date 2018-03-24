//===- MaxisHazardSchedule.cpp - Workaround pipeline hazards ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This pass is used to workaround certain pipeline hazards. For now, this
/// covers compact branch hazards. In future this pass can be extended to other
/// pipeline hazards, such as various MAXIS1 hazards, processor errata that
/// require instruction reorganization, etc.
///
/// This pass has to run after the delay slot filler as that pass can introduce
/// pipeline hazards, hence the existing hazard recognizer is not suitable.
///
/// Hazards handled: forbidden slots for MAXISR6.
///
/// A forbidden slot hazard occurs when a compact branch instruction is executed
/// and the adjacent instruction in memory is a control transfer instruction
/// such as a branch or jump, ERET, ERETNC, DERET, WAIT and PAUSE.
///
/// For example:
///
/// 0x8004      bnec    a1,v0,<P+0x18>
/// 0x8008      beqc    a1,a2,<P+0x54>
///
/// In such cases, the processor is required to signal a Reserved Instruction
/// exception.
///
/// Here, if the instruction at 0x8004 is executed, the processor will raise an
/// exception as there is a control transfer instruction at 0x8008.
///
/// There are two sources of forbidden slot hazards:
///
/// A) A previous pass has created a compact branch directly.
/// B) Transforming a delay slot branch into compact branch. This case can be
///    difficult to process as lookahead for hazards is insufficient, as
///    backwards delay slot fillling can also produce hazards in previously
///    processed instuctions.
///
//===----------------------------------------------------------------------===//

#include "Maxis.h"
#include "MaxisInstrInfo.h"
#include "MaxisSubtarget.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include <algorithm>
#include <iterator>
#include <utility>

using namespace llvm;

#define DEBUG_TYPE "maxis-hazard-schedule"

STATISTIC(NumInsertedNops, "Number of nops inserted");

namespace {

using Iter = MachineBasicBlock::iterator;
using ReverseIter = MachineBasicBlock::reverse_iterator;

class MaxisHazardSchedule : public MachineFunctionPass {
public:
  MaxisHazardSchedule() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return "Maxis Hazard Schedule"; }

  bool runOnMachineFunction(MachineFunction &F) override;

  MachineFunctionProperties getRequiredProperties() const override {
    return MachineFunctionProperties().set(
        MachineFunctionProperties::Property::NoVRegs);
  }

private:
  static char ID;
};

} // end of anonymous namespace

char MaxisHazardSchedule::ID = 0;

/// Returns a pass that clears pipeline hazards.
FunctionPass *llvm::createMaxisHazardSchedule() {
  return new MaxisHazardSchedule();
}

// Find the next real instruction from the current position in current basic
// block.
static Iter getNextMachineInstrInBB(Iter Position) {
  Iter I = Position, E = Position->getParent()->end();
  I = std::find_if_not(I, E,
                       [](const Iter &Insn) { return Insn->isTransient(); });

  return I;
}

// Find the next real instruction from the current position, looking through
// basic block boundaries.
static std::pair<Iter, bool> getNextMachineInstr(Iter Position, MachineBasicBlock * Parent) {
  if (Position == Parent->end()) {
    do {
      MachineBasicBlock *Succ = Parent->getNextNode();
      if (Succ != nullptr && Parent->isSuccessor(Succ)) {
        Position = Succ->begin();
        Parent = Succ;
      } else {
        return std::make_pair(Position, true);
      }
    } while (Parent->empty());
  }

  Iter Instr = getNextMachineInstrInBB(Position);
  if (Instr == Parent->end()) {
    return getNextMachineInstr(Instr, Parent);
  }
  return std::make_pair(Instr, false);
}

bool MaxisHazardSchedule::runOnMachineFunction(MachineFunction &MF) {

  const MaxisSubtarget *STI =
      &static_cast<const MaxisSubtarget &>(MF.getSubtarget());

  // Forbidden slot hazards are only defined for MAXISR6 but not microMAXISR6.
  if (!STI->hasMaxis32r6() || STI->inMicroMaxisMode())
    return false;

  bool Changed = false;
  const MaxisInstrInfo *TII = STI->getInstrInfo();

  for (MachineFunction::iterator FI = MF.begin(); FI != MF.end(); ++FI) {
    for (Iter I = FI->begin(); I != FI->end(); ++I) {

      // Forbidden slot hazard handling. Use lookahead over state.
      if (!TII->HasForbiddenSlot(*I))
        continue;

      Iter Inst;
      bool LastInstInFunction =
          std::next(I) == FI->end() && std::next(FI) == MF.end();
      if (!LastInstInFunction) {
        std::pair<Iter, bool> Res = getNextMachineInstr(std::next(I), &*FI);
        LastInstInFunction |= Res.second;
        Inst = Res.first;
      }

      if (LastInstInFunction || !TII->SafeInForbiddenSlot(*Inst)) {
        Changed = true;
        MIBundleBuilder(&*I)
            .append(BuildMI(MF, I->getDebugLoc(), TII->get(Maxis::NOP)));
        NumInsertedNops++;
      }
    }
  }
  return Changed;
}
