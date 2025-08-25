#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdint>
#include <cmath>
#include <random>
#include <glm/vec2.hpp>

namespace Interstellar::Universe {

    struct SectorID {
        int64_t x{ 0 }, y{ 0 };
        bool operator==(const SectorID& o) const noexcept { return x == o.x && y == o.y; }
    };

    struct SectorIDHash {
        size_t operator()(const SectorID& s) const noexcept {
            // 64-bit mix, then fold
            auto h1 = std::hash<int64_t>{}(s.x);
            auto h2 = std::hash<int64_t>{}(s.y);
            uint64_t k = (uint64_t)h1 ^ (rotl((uint64_t)h2, 32) + 0x9e3779b97f4a7c15ULL + ((uint64_t)h1 << 6) + ((uint64_t)h1 >> 2));
            return static_cast<size_t>(k ^ (k >> 33));
        }
        static inline uint64_t rotl(uint64_t v, int r) { return (v << r) | (v >> (64 - r)); }
    };

    struct SectorData {
        // instance-local positions relative to sector origin (centered at (0,0))
        std::vector<glm::vec2> instances;
    };

    class SectorStreamer {
    public:
        SectorStreamer(uint64_t masterSeed,
            float    sectorSize,
            int      instancesPerSector,
            int      radiusInSectors)
            : m_seed(masterSeed)
            , m_sectorSize(sectorSize)
            , m_instancesPerSector(instancesPerSector)
            , m_radius(radiusInSectors) {
        }

        void setRadius(int r) { m_radius = r; }
        int  radius() const { return m_radius; }
        float sectorSize() const { return m_sectorSize; }

        // Collect all visible instance positions (x,y pairs) given camera center.
        // Output format: [x0,y0, x1,y1, ...] suitable for GLInstancedSubmit.
        void collect(const glm::vec2& cameraCenter, std::vector<float>& outXY) {
            const SectorID cam = sectorOf(cameraCenter);
            // Desired set
            std::unordered_set<SectorID, SectorIDHash> wanted;
            for (int dy = -m_radius; dy <= m_radius; ++dy) {
                for (int dx = -m_radius; dx <= m_radius; ++dx) {
                    wanted.insert(SectorID{ cam.x + dx, cam.y + dy });
                }
            }
            // Create missing sectors
            for (const auto& sid : wanted) {
                if (m_cache.find(sid) == m_cache.end()) {
                    m_cache.emplace(sid, generateSector(sid));
                }
            }
            // Evict sectors not wanted (simple sweep)
            for (auto it = m_cache.begin(); it != m_cache.end(); ) {
                if (wanted.find(it->first) == wanted.end()) it = m_cache.erase(it);
                else ++it;
            }
            // Aggregate visible instances into outXY
            outXY.clear();
            outXY.reserve(estimateTotalInstances());
            for (const auto& [sid, data] : m_cache) {
                const glm::vec2 origin = sectorOrigin(sid);
                for (const auto& p : data.instances) {
                    outXY.push_back(origin.x + p.x);
                    outXY.push_back(origin.y + p.y);
                }
            }
        }

    private:
        SectorID sectorOf(const glm::vec2& pos) const {
            // Centered sectors: each sector spans [-S/2, +S/2) in both axes
            const float S = m_sectorSize;
            auto to_cell = [S](float v)->int64_t {
                // Shift so that sector 0 spans [-S/2, +S/2)
                return static_cast<int64_t>(std::floor((v + 0.5f * S) / S));
                };
            return SectorID{ to_cell(pos.x), to_cell(pos.y) };
        }

        glm::vec2 sectorOrigin(const SectorID& s) const {
            // Center of the sector in world coords
            return glm::vec2(s.x * m_sectorSize, s.y * m_sectorSize);
        }

        SectorData generateSector(const SectorID& s) const {
            SectorData d;
            d.instances.reserve(m_instancesPerSector);

            // Deterministic PRNG per sector from master seed + coords
            uint64_t h = hashCombine(m_seed, (uint64_t)(s.x), (uint64_t)(s.y));
            std::mt19937_64 rng(h);

            // Place instances uniformly within the sector square, centered at (0,0)
            std::uniform_real_distribution<float> uni(-0.5f * m_sectorSize, 0.5f * m_sectorSize);
            for (int i = 0; i < m_instancesPerSector; ++i) {
                d.instances.emplace_back(uni(rng), uni(rng));
            }
            return d;
        }

        size_t estimateTotalInstances() const {
            // rough upper bound
            const int side = m_radius * 2 + 1;
            return static_cast<size_t>(side * side * m_instancesPerSector);
        }

        static uint64_t hashCombine(uint64_t seed, uint64_t a, uint64_t b) {
            // simple 64-bit mix
            auto mix = [](uint64_t x) {
                x ^= x >> 33; x *= 0xff51afd7ed558ccdULL;
                x ^= x >> 33; x *= 0xc4ceb9fe1a85ec53ULL;
                x ^= x >> 33; return x;
                };
            uint64_t h = seed;
            h ^= mix(a + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
            h ^= mix(b + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
            return mix(h);
        }

    private:
        uint64_t m_seed;
        float    m_sectorSize;
        int      m_instancesPerSector;
        int      m_radius;

        std::unordered_map<SectorID, SectorData, SectorIDHash> m_cache;
    };

} // namespace Interstellar::Universe
