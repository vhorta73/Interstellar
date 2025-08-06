#pragma once

#include "Interstellar/Data/IElementDatabase.hpp"
#include "Interstellar/Data//ElementData.hpp"
#include <unordered_map>
#include <string>

namespace Interstellar::Config::JsonImpl {

    using Interstellar::Data::ElementData;
    using Interstellar::Data::IElementDatabase;

    /**
     * @ingroup InterstellarConfig
     * @brief JSON-backed implementation of IElementDatabase for managing chemical elements.
     *
     * Provides a mapping from element symbols to their corresponding data, and
     * loads them from a JSON file source.
     *
     * @since 1.0
     */
    class ElementDatabase : public IElementDatabase {
    public:
        /**
         * @brief Loads element data from a JSON file.
         *
         * @param path Path to the JSON file (e.g., "data/elements.json")
         * @return true if loading was successful and valid, false otherwise.
         */
        bool loadFromFile(const std::string& path);

        /**
         * @brief Retrieves a pointer to an ElementData by its atomic symbol.
         *
         * @param symbol Element symbol (e.g., "H", "Fe").
         * @return Pointer to the ElementData if found; nullptr otherwise.
         */
        const ElementData* get(const std::string& symbol) const override;

        /**
         * @brief Returns the full map of all loaded element data.
         *
         * @return const reference to the internal element map.
         */
        const std::unordered_map<std::string, ElementData>& all() const noexcept override;

    private:
        std::unordered_map<std::string, ElementData> data_; ///< Internal symbol-to-data map
    };

} // namespace Interstellar::Config::JsonImpl
