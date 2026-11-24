#include "InputProtocol.hpp"
#include <cassert>
#include <iostream>

using namespace qqfo::input;

int main() {
    // Wire bytes emitted by the managed client for a fixed target and session.
    Frame bind{0x51,0x4f,0x46,0x31,1,0,1,0,1,0,0,0,0,0,0,0,
        0x33,0x22,0x11,0x00,0x55,0x44,0x77,0x66,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
        0xc8,0x01,0,0,0,0,0,0,0x7b,0,0,0};
    Target target{{0x33,0x22,0x11,0x00,0x55,0x44,0x77,0x66,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff}, 456, 123};
    auto click = bind;
    click[6] = 2;
    click[8] = 2;
    click[44] = 0x20;
    click[45] = 0x03;
    click[48] = 0xc2;
    click[49] = 0x01;
    RequestSequence connection(target);
    assert(!connection.accept(click));
    auto wrong = bind;
    wrong[16] ^= 1;
    assert(!connection.accept(wrong));
    wrong = bind;
    wrong[40]++;
    assert(!connection.accept(wrong));
    assert(connection.accept(bind)->operation == Operation::Bind);
    wrong = click;
    wrong[47] = 0xff;
    assert(!connection.accept(wrong));
    const auto accepted = connection.accept(click);
    assert(accepted && accepted->operation == Operation::Click && accepted->x == 800 && accepted->y == 450);
    assert(!connection.accept(click));
    assert(acknowledgement(click, true)[52] == 1);
    assert(acknowledgement(click, false) == click);
    std::cout << "OFFLINE_PASS " << sizeof(void*) * 8 << "-bit native wire validation and replay rejection\n";
}
