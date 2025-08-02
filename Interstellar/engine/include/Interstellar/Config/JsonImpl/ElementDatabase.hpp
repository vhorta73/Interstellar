#include "Interstellar/Data/IElementDatabase.hpp"
#include "Interstellar/Data/ElementData.hpp"

namespace Interstellar::Config::JsonImpl {

    using Interstellar::Data::ElementData;
    using Interstellar::Data::IElementDatabase; // <-- Add this line to bring IElementDatabase into scope

    class ElementDatabase : public IElementDatabase {
    public:
        bool loadFromFile(const std::string& path);

        const ElementData* get(const std::string& symbol) const override;
        const std::unordered_map<std::string, ElementData>& all() const noexcept override;

    private:
        std::unordered_map<std::string, ElementData> data_;
    };

}
