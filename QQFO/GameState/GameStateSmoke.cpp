#include "GameStateReader.hpp"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace qqfo::state;
using namespace std::chrono_literals;

class CapturedMemory final : public MemorySource {
public:
    Session active{21, 100, 1};
    std::map<std::uint32_t, unsigned char> bytes;
    std::optional<std::uint32_t> move_map_after_position;
    std::uint32_t position_address = 0;
    unsigned positions_read = 0;

    bool current(Session session) const noexcept override { return active == session; }
    bool read(Session session, std::uint32_t address, void* buffer, std::size_t size) noexcept override {
        if (!current(session))
            return false;
        auto result = static_cast<unsigned char*>(buffer);
        for (std::size_t i = 0; i < size; ++i) {
            const auto found = bytes.find(address + static_cast<std::uint32_t>(i));
            if (found == bytes.end())
                return false;
            result[i] = found->second;
        }
        if (address == position_address) {
            ++positions_read;
            if (move_map_after_position)
                bytes.at(*move_map_after_position) ^= 1;
        }
        return true;
    }

    template <typename T>
    void put(std::uint32_t address, const T& value) {
        unsigned char raw[sizeof(T)]{};
        std::memcpy(raw, &value, sizeof(T));
        for (std::size_t i = 0; i < sizeof(T); ++i)
            bytes[address + static_cast<std::uint32_t>(i)] = raw[i];
    }
};

static void synthetic() {
    constexpr C1Module module{0x400000, 0x700000};
    constexpr std::uint32_t manager = 0x2000000;
    constexpr std::uint32_t role = 0x3000000;
    CapturedMemory first;
    first.position_address = role + 0x23ec;
    first.put(0x6c9110, std::array<std::uint32_t, 2>{0x1ffffff, manager ^ 0x1ffffff});
    std::array<std::uint32_t, 22> links{};
    links[0] = 0x5000000;
    links[17] = role;
    links[21] = 37;
    first.put(manager + 0x12c, links);
    const std::array<std::uint32_t, 7> grid{200, 100, 32, 32, 6400, 3200, 0x6000000};
    first.put(links[0], grid);
    first.put(role, module.base + 0x1c4220);
    first.put(role + 0x23ec, WorldPosition{-8087, 1572});
    CapturedMemory second = first;
    second.active.pid = 22;
    second.put(role + 0x23ec, WorldPosition{90, 100});

    GameStateReader reader(first, first.active, module, 10s);
    GameStateReader other(second, second.active, module, 10s);
    const auto initial = reader.sample();
    assert(initial.status == ReadStatus::Ready && initial.player->x == -8087 && initial.map_resource == 37u);
    assert(other.sample().player->x == 90);
    assert(reader.sample().sequence == initial.sequence && first.positions_read == 1);
    first.put(manager + 0x180, std::uint32_t{38});
    assert(reader.sample().map_resource == 38u && first.positions_read == 2);
    reader.invalidate();
    first.move_map_after_position = manager + 0x12c;
    const auto unstable = reader.sample();
    assert(unstable.status == ReadStatus::SceneChanged && !unstable.player);
    first.move_map_after_position.reset();
    first.put(manager + 0x12c, links[0]);
    assert(reader.sample().status == ReadStatus::Ready);
    first.put(role, std::uint32_t{0});
    assert(reader.sample().status == ReadStatus::RoleLayoutMismatch);
    first.bytes.erase(role);
    assert(reader.sample().status == ReadStatus::ReadFailed);
    first.active.process_creation++;
    const auto expired = reader.sample();
    assert(expired.status == ReadStatus::SessionUnavailable && !expired.map_resource);
    std::cout << "OFFLINE_PASS: C1 coordinates, map change, cache, role layout and session isolation\n";
}

static void replay(const wchar_t* path) {
    std::ifstream input(path);
    CapturedMemory capture;
    C1Module module{};
    std::int32_t x = 0, y = 0;
    std::uint32_t map = 0;
    assert(input >> module.base >> module.size >> x >> y >> map);
    std::uint32_t address = 0;
    std::string raw;
    while (input >> address >> raw) {
        assert(raw.size() % 2 == 0);
        for (std::size_t i = 0; i < raw.size(); i += 2)
            capture.bytes[address + static_cast<std::uint32_t>(i / 2)] =
                static_cast<unsigned char>(std::stoul(raw.substr(i, 2), nullptr, 16));
    }
    GameStateReader reader(capture, capture.active, module, 0ms);
    const auto sample = reader.sample();
    assert(sample.status == ReadStatus::Ready && sample.player->x == x && sample.player->y == y && sample.map_resource == map);
    std::cout << "OFFLINE_PASS: archived C1 player (" << x << ", " << y << "), map resource " << map << '\n';
}

int wmain(int argc, wchar_t** argv) {
    synthetic();
    if (argc == 2)
        replay(argv[1]);
    return argc <= 2 ? 0 : 2;
}
