AXIS�Хå�����ɤμ�����ˡ�ˤĤ���
ʸ�ա������� ���� ��Ƿ

[�Ϥ����]
- ���Ѥ���LLVM�ΥС�������6.0.0
- MIPS�ΥХå�����ɤ�١����˼���


[̾����AXIS����MAXIS���ѹ�������ͳ]
̾����AXIS�ˤ���ȼ���ץ����ǥ���ѥ��뤷���ݡ�sub̿�᤬�ʤ��Ȥ������顼��ȯ��������
MAXIS�ˤ�������꤬��褷���Τǡ��ǽ��ʸ�������פȻפ��롥
MIPS��¸�����ɤ����ƽ�����AXIS�Ȥ���̾���Ǥ�����פ��⤷��ʤ���
AXIS��MAXIS�Ȥ����������롥


[�Хå������&�ե��ȥ���ɶ���]
http://releases.llvm.org/����LLVM/Clang/LLD���������ɤ��롥
Clang��LLD��llvm/tools/�ʲ������֡�


����Ū�ˤϡ�Mips�Ƚ񤤤Ƥ���ս��Maxis���ɲý�����������
$ grep mips * -Ri
��(��ʸ������ʸ���ζ��̤ʤ���)mips��ʸ����򸡺�����

[�ִ��롼��]
̾�������֤äƤ���ؿ����������Maxis�Ȥ���ʸ�����ؿ�������롥
[�ץ������Ǥ�ifʸ�Ǥ�ʬ����ˡ]
if (Config->EMachine == EM_MAXIS)�Ȥ������˽�

�ե�����̾��Mips�Ƚ񤤤Ƥ����Τϥ��ԡ�����Maxis���ѹ�����
Mips.cpp -> Maxis.cpp


[�Хå�����ɤμ�����ˡ]

����LLVM�Υ��ߥåȥ������ͤˤʤ롥
https://github.com/pflab-ut/llvm/commits/master


- ̿����ɲä�����ˡ
1. ADD/SUB̿������MaxisInstrInfo.td�ե�������Խ�����
2. XNOR/NAND̿���1����ˡ�Ȱ��˰ʲ��Υե������XOR̿��򥳥ԡ��Ƚ���
include/llvm/CodeGen/GlobalISel/IRTranslator.h
include/llvm/CodeGen/ISDOpcodes.h
include/llvm/CodeGen/TargetOpcodes.def
include/llvm/Target/GenericOpcodes.td
include/llvm/Target/GlobalISel/SelectionDAGCompat.td
include/llvm/Target/TargetSelectionDAG.td
lib/Target/Maxis/MaxisInstrInfo.td
lib/Target/Maxis/MaxisSchedule.td
��LLVM�ΥС������ˤ�ä�XOR̿��μ����ե����뤬�Ѥ���ǽ����
����ΤǥС��������ˡ�$ grep XOR * -Ri�פ�¹Ԥ��롥


- DelaySlot��1����0�ˤ�����ˡ
lib/Target/Maxis/MaxisInstrInfo.td�ե������hasDelaySlot = 1��hasDelaySlot = 0�ˤ���


- llvm-objdump�ǽ��Ϥ���̿���alias̾���ѹ�������ˡ
lib/Target/Maxis/InstPrinter/MaxisInstPrinter.cpp��
printAlias���дؿ����ѹ�����


- x86��xnor̿�᤬����Τǡ��ʲ��Υե������xnor��_xnor�ˤ��Ƥ��롥
include/llvm/Target/GlobalISel/SelectionDAGCompat.td
include/llvm/Target/TargetSelectionDAG.td
lib/Target/Maxis/MaxisInstrInfo.td


[�ե��ȥ����]

�ѿ���align���ѹ�������ϡ�
tools/clang/lib/Basic/Targets/Maxis.h

��������tools/clang/include/clang/Basic/TargetInfo.h��
getCharAlign/getShortAlign�ؿ���virtual�ˤ��ʤ����ѹ��Ǥ��ʤ���

