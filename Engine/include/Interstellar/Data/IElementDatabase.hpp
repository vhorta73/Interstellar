#pragma once

#include <string>
#include <unordered_map>
#include "Interstellar/Data/ElementData.hpp"

namespace Interstellar::Data {

    /**
     * @ingroup InterstellarData
     * @brief Interface for retrieving chemical element data.
     * 
     * @since 1.0
     */
    class IElementDatabase {
    public:
        IElementDatabase() = default;
        virtual ~IElementDatabase() = default;

        IElementDatabase(const IElementDatabase&) = delete;
        IElementDatabase& operator=(const IElementDatabase&) = delete;

        /**
         * @brief Get a pointer to element data by symbol (e.g., "H", "Fe").
         * 
         * @param symbol The atomic symbol.
         * 
         * @return Pointer to ElementData, or nullptr if not found.
         *         Caller does not own the returned pointer.
         * 
         * @since 1.0
         */
        [[nodiscard]] virtual const ElementData* get(const std::string& symbol) const = 0;

        /**
         * @brief Get a reference to the full symbol-to-element map.
         * 
         * @return Map of all loaded element definitions.
         * 
         * @since 1.0
         */
        [[nodiscard]] virtual const std::unordered_map<std::string, ElementData>& all() const noexcept = 0;

        /**
        * @brief Check if an element exists by symbol.
        *
        * @param symbol The atomic symbol.
        * @return True if the element exists.
        * 
        * @since 1.0
        */
        [[nodiscard]] virtual bool has(const std::string& symbol) const {
            return get(symbol) != nullptr;
        }
    };
}