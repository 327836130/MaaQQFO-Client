#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <mutex>
#include <optional>

namespace qqfo::state {

struct Session {
    std::uint32_t pid;
    std::uint64_t process_creation;
    std::uint64_t generation;

    bool operator==(const Session& other) const noexcept {
        return pid == other.pid && process_creation == other.process_creation && generation == other.generation;
    }
};

class MemorySource {
public:
    virtual ~MemorySource() = default;
    virtual bool current(Session session) const noexcept = 0;
    // Return true only for an exact read from the bound session.
    virtual bool read(Session session, std::uint32_t address, void* buffer, std::size_t size) noexcept = 0;
};

struct WorldPosition { std::int32_t x, y; };
struct C1Module { std::uint32_t base, size; };
enum class ReadStatus { Ready, SessionUnavailable, ReadFailed, RoleLayoutMismatch, SceneChanged };

struct Snapshot {
    Session session;
    std::uint64_t sequence;
    std::chrono::steady_clock::time_point sampled_at;
    ReadStatus status;
    std::optional<WorldPosition> player;
    std::optional<std::uint32_t> map_resource;
};

// Decode only the fields confirmed by the C1 client capture. The caller owns
// build identification and supplies a read-only source for that exact session.
class GameStateReader {
public:
    GameStateReader(MemorySource& source, Session session, C1Module module,
                    std::chrono::milliseconds freshness) noexcept
        : source_(source), session_(session), module_(module), freshness_(freshness) {}

    Snapshot sample() {
        const std::lock_guard<std::mutex> lock(mutex_);
        if (!source_.current(session_))
            return failed(ReadStatus::SessionUnavailable);
        const auto identity = scene();
        if (!identity)
            return failed(ReadStatus::ReadFailed);
        const auto now = std::chrono::steady_clock::now();
        const auto vtable = read_at<std::uint32_t>(identity->role);
        if (!vtable)
            return failed(ReadStatus::ReadFailed);
        if (*vtable != module_.base + role_vtable)
            return failed(ReadStatus::RoleLayoutMismatch);
        if (cached_ && identity_ == identity && now - cached_->sampled_at < freshness_ &&
            source_.current(session_))
            return *cached_;
        const auto player = read_at<WorldPosition>(identity->role, 0x23ec);
        if (!player)
            return failed(ReadStatus::ReadFailed);
        const auto confirmed = scene();
        if (!source_.current(session_))
            return failed(ReadStatus::SessionUnavailable);
        if (!confirmed || *confirmed != *identity || read_at<std::uint32_t>(identity->role) != vtable)
            return failed(ReadStatus::SceneChanged);

        identity_ = identity;
        cached_ = Snapshot{session_, ++sequence_, now, ReadStatus::Ready, player, identity->resource};
        return *cached_;
    }

    void invalidate() {
        const std::lock_guard<std::mutex> lock(mutex_);
        cached_.reset();
        identity_.reset();
    }

private:
    static constexpr std::uint32_t manager_roots = 0x2c9110;
    static constexpr std::uint32_t role_vtable = 0x1c4220;

    struct Scene {
        std::array<std::uint32_t, 2> roots;
        std::uint32_t map, role, resource;
        std::array<std::uint32_t, 7> grid;
        bool operator==(const Scene& other) const noexcept {
            return roots == other.roots && map == other.map && role == other.role &&
                resource == other.resource && grid == other.grid;
        }
        bool operator!=(const Scene& other) const noexcept { return !(*this == other); }
    };

    template <typename T>
    std::optional<T> read_at(std::uint32_t base, std::uint32_t offset = 0) noexcept {
        if (base < 0x10000 || static_cast<std::uint64_t>(base) + offset + sizeof(T) >
            static_cast<std::uint64_t>((std::numeric_limits<std::uint32_t>::max)()) + 1)
            return std::nullopt;
        T value{};
        if (!source_.read(session_, base + offset, &value, sizeof(value)))
            return std::nullopt;
        return value;
    }

    std::optional<Scene> scene() noexcept {
        if (module_.size < manager_roots + 8 || static_cast<std::uint64_t>(module_.base) + module_.size >
            (std::numeric_limits<std::uint32_t>::max)())
            return std::nullopt;
        const auto roots = read_at<std::array<std::uint32_t, 2>>(module_.base, manager_roots);
        if (!roots)
            return std::nullopt;
        const auto manager = (*roots)[0] ^ (*roots)[1];
        const auto links = read_at<std::array<std::uint32_t, 22>>(manager, 0x12c);
        if (!links || (*links)[0] < 0x10000 || (*links)[17] < 0x10000)
            return std::nullopt;
        const auto grid = read_at<std::array<std::uint32_t, 7>>((*links)[0]);
        if (!grid || !(*grid)[0] || !(*grid)[1] ||
            static_cast<std::uint64_t>((*grid)[0]) * (*grid)[1] > 4u * 1024u * 1024u ||
            !(*grid)[2] || !(*grid)[3] || (*grid)[6] < 0x10000)
            return std::nullopt;
        return Scene{*roots, (*links)[0], (*links)[17], (*links)[21], *grid};
    }

    Snapshot failed(ReadStatus status) {
        cached_.reset();
        identity_.reset();
        return {session_, ++sequence_, std::chrono::steady_clock::now(), status, std::nullopt, std::nullopt};
    }

    MemorySource& source_;
    Session session_;
    C1Module module_;
    std::chrono::milliseconds freshness_;
    std::mutex mutex_;
    std::uint64_t sequence_ = 0;
    std::optional<Snapshot> cached_;
    std::optional<Scene> identity_;
};

} // namespace qqfo::state
