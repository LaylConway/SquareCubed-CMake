#pragma once

#include <list>
#include <memory>

namespace Tools {
	namespace Units { class IUnit; }
}

namespace Graphics {
	class ISelectionRenderer {
	public: // Initialization/Uninitialization
		virtual ~ISelectionRenderer() {}

	public: // Rendering Functions
		virtual void RenderSelectionBoxes(const std::list<std::unique_ptr<Tools::Units::IUnit>> &units) = 0;
	};
}