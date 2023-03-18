#include "patches.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void costumeRoomPatches() {
    patch::CodePatcher p(0x262850);
    p.WriteInst(inst::Movz(reg::W0, 0));
    p.Seek(0x2609B4);
    p.WriteInst(inst::Movz(reg::W0, 0));

    p.Seek(0x25FF74);
    p.WriteInst(inst::Movz(reg::W0, 1));
    p.Seek(0x25FF74);
    p.WriteInst(inst::Movz(reg::W0, 0));
}

void twitterIKPatches() {
    patch::CodePatcher p(0x455230);
    p.WriteInst(inst::Nop());
    p.Seek(0x4552b4);
    p.WriteInst(inst::Nop());
}

void stubSocketInit() {
    patch::CodePatcher p(0x95C498);
    p.WriteInst(inst::Nop());
}

void runCodePatches() {
    twitterIKPatches();
    stubSocketInit();
}