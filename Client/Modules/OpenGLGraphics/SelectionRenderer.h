#pragma once

#include <ClientBase/ISelectionRenderer.h>

namespace Graphics { class ITileArray; }

namespace OpenGLGraphics {
	class SelectionRenderer : public Graphics::ISelectionRenderer {
	public: // Initialization/Uninitialization
		SelectionRenderer();

	public: // Rendering Functions
		void RenderSelectionBoxes(const std::list<std::unique_ptr<Tools::Units::IUnit>> &units);
	};
}