//===- MaxisABIFlagsSection.cpp - Maxis ELF ABI Flags Section ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MaxisABIFlagsSection.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MaxisABIFlags.h"

using namespace llvm;

uint8_t MaxisABIFlagsSection::getFpABIValue() {
  switch (FpABI) {
  case FpABIKind::ANY:
    return Maxis::Val_GNU_MAXIS_ABI_FP_ANY;
  case FpABIKind::SOFT:
    return Maxis::Val_GNU_MAXIS_ABI_FP_SOFT;
  case FpABIKind::XX:
    return Maxis::Val_GNU_MAXIS_ABI_FP_XX;
  case FpABIKind::S32:
    return Maxis::Val_GNU_MAXIS_ABI_FP_DOUBLE;
  case FpABIKind::S64:
    if (Is32BitABI)
      return OddSPReg ? Maxis::Val_GNU_MAXIS_ABI_FP_64
                      : Maxis::Val_GNU_MAXIS_ABI_FP_64A;
    return Maxis::Val_GNU_MAXIS_ABI_FP_DOUBLE;
  }

  llvm_unreachable("unexpected fp abi value");
}

StringRef MaxisABIFlagsSection::getFpABIString(FpABIKind Value) {
  switch (Value) {
  case FpABIKind::XX:
    return "xx";
  case FpABIKind::S32:
    return "32";
  case FpABIKind::S64:
    return "64";
  default:
    llvm_unreachable("unsupported fp abi value");
  }
}

uint8_t MaxisABIFlagsSection::getCPR1SizeValue() {
  if (FpABI == FpABIKind::XX)
    return (uint8_t)Maxis::AFL_REG_32;
  return (uint8_t)CPR1Size;
}

namespace llvm {

MCStreamer &operator<<(MCStreamer &OS, MaxisABIFlagsSection &ABIFlagsSection) {
  // Write out a Elf_Internal_ABIFlags_v0 struct
  OS.EmitIntValue(ABIFlagsSection.getVersionValue(), 2);      // version
  OS.EmitIntValue(ABIFlagsSection.getISALevelValue(), 1);     // isa_level
  OS.EmitIntValue(ABIFlagsSection.getISARevisionValue(), 1);  // isa_rev
  OS.EmitIntValue(ABIFlagsSection.getGPRSizeValue(), 1);      // gpr_size
  OS.EmitIntValue(ABIFlagsSection.getCPR1SizeValue(), 1);     // cpr1_size
  OS.EmitIntValue(ABIFlagsSection.getCPR2SizeValue(), 1);     // cpr2_size
  OS.EmitIntValue(ABIFlagsSection.getFpABIValue(), 1);        // fp_abi
  OS.EmitIntValue(ABIFlagsSection.getISAExtensionValue(), 4); // isa_ext
  OS.EmitIntValue(ABIFlagsSection.getASESetValue(), 4);       // ases
  OS.EmitIntValue(ABIFlagsSection.getFlags1Value(), 4);       // flags1
  OS.EmitIntValue(ABIFlagsSection.getFlags2Value(), 4);       // flags2
  return OS;
}

} // end namespace llvm
