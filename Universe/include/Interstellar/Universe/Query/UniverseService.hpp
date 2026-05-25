#pragma once
#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/AABB.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/UniverseQuery.hpp"  // QueryUniverseAABB3, filters, result

namespace Interstellar::Universe {

    struct UniverseServiceConfig {
        // Phase 2 will add cache sizes etc. For now this is a thin wrapper.
    };

    class UniverseService {
    public:
        UniverseService(Seed64 masterSeed, Universe3DRecipe recipe,
            UniverseServiceConfig cfg = {})
            : master_(masterSeed), recipe_(recipe), cfg_(cfg) {
        }

        void queryAABB(const AABB3& aabb,
            const UniverseQueryFilters& filters,
            UniverseQueryResult& out) const
        {
            QueryUniverseAABB3(master_, recipe_, aabb, filters, out);
        }

        void tick(double /*dt*/) {}
        void clearCache() {}

        const Universe3DRecipe& recipe() const { return recipe_; }
        Seed64 masterSeed() const { return master_; }

    private:
        Seed64 master_;
        Universe3DRecipe recipe_;
        UniverseServiceConfig cfg_;
    };

} // namespace Interstellar::Universe
