#include "UnitSelect.h"

#include "Units.h"
#include "Engine.h"
#include "IGraphics.h"
#include "IGraphicsFactory.h"
#include "INetwork.h"
#include "INetworkFactory.h"
#include "IUnitSelectDispatcher.h"

#include <algorithm>

namespace Tools {
	namespace Units {
		UnitSelect::UnitSelect(Core::Engine &engine, GUI::Elements::IViewGenerator &view, Tools::Units::Units &units) :
			// External Components
			m_Units(units),
			m_Input(engine.GetInput()),

			// Internal Components
			m_InfoPanel(engine, view),
			m_Renderer(engine.GetGraphics().GetFactory().CreateSelectionRenderer()),
			m_Dispatcher(engine.GetNetwork().GetFactory().CreateUnitSelectDispatcher()),

			m_EventScope(),
			m_SelectedUnits(),
			m_ControlGroup()
		{
			// Bind Key Events
			engine.GetInput().OnMouseButtonChange.AttachMember(this, &UnitSelect::OnMouseButtonChange, m_EventScope);
			engine.GetInput().OnKeyChange.AttachMember(this, &UnitSelect::OnKeyChange, m_EventScope);

			// Get Key IDs for 1 till 0
			for (int i = 0; i < 9; i++)
				Key[i] = m_Input.GetKeyId('1' + i);
		}

		UnitSelect::~UnitSelect() {}

		void UnitSelect::Render() {
			m_Renderer->RenderUnitSelection(m_SelectedUnits);
			if (m_Input.GetCursorPosition().World != m_OriginalCursorPos && m_CurrentlyHolding) {
				glm::vec2 currentCursorPos = m_Input.GetCursorPosition().World;
				glm::vec2 bottomLeft;
				glm::vec2 size;
				
				bottomLeft.x = std::min(currentCursorPos.x, m_OriginalCursorPos.x);
				bottomLeft.y = std::min(currentCursorPos.y, m_OriginalCursorPos.y);
				size.x = fabs(currentCursorPos.x - m_OriginalCursorPos.x);
				size.y = fabs(currentCursorPos.y - m_OriginalCursorPos.y);

				m_Renderer->RenderSelectionBox(bottomLeft, size);
			}
		}

		// Internal Helper Functions

		bool UnitSelect::IsUnitSelected(const UnitID unitId) {
			for (auto unit : m_SelectedUnits) {
				if (unit.get().ID == unitId) {
					// Found it!
					return true;
				}
			}
			// Didn't find it
			return false;
		}

		void UnitSelect::Deselect(const UnitID unitId) {
			auto it = m_SelectedUnits.begin();
			while (it != m_SelectedUnits.end()) {
				if ((it)->get().ID == unitId) {
					it = m_SelectedUnits.erase(it);
				}
				else
					++it;
			}
		}

		// Event Callbacks

		void UnitSelect::OnMouseButtonChange(const Input::MouseEventArgs &args) {
			// On left click:
			if (args.MouseButton == Input::MouseButton::Left && args.IsPressed) {
				// Record starting cursor position, and set bool for holding the button to true.
				m_OriginalCursorPos = args.CursorPosition.World;
				m_CurrentlyHolding = true;
			}

			// On left click release:
			if (args.MouseButton == Input::MouseButton::Left && !args.IsPressed) {
				bool boxWasDragged = false;
				m_CurrentlyHolding = false;
				glm::vec2 endCursorPos = m_Input.GetCursorPosition().World;
				Physics::AABBData box;

				// If the cursor moved between button click and release, a box was dragged.
				if (m_OriginalCursorPos != endCursorPos) {
					box.Min.x = std::min(m_OriginalCursorPos.x, endCursorPos.x);
					box.Min.y = std::min(m_OriginalCursorPos.y, endCursorPos.y);
					box.Max.x = std::max(m_OriginalCursorPos.x, endCursorPos.x);
					box.Max.y = std::max(m_OriginalCursorPos.y, endCursorPos.y);
					boxWasDragged = true;
				}

				// Get every single unit.
				const std::list<std::unique_ptr<IUnit>>& unitList = m_Units.GetAll();
				bool noHit = true;
				
				/* If something was already selected, and the user is not holding shift for multiselection,
				   then this is just a single selection of another unit, for which the previous selection should be cleared. */
				if (!m_Input.GetKeyMods().Shift && !m_SelectedUnits.empty())
					m_SelectedUnits.clear();

				// For every single unit:
				for (const std::unique_ptr<IUnit> &unit : unitList) {
					// TODO: Needs common rigidbody interface but for now this works
					auto dUnit = static_cast<const DynamicUnit*>(unit.get());

					// If the mouse click or dragged box hits the Unit hitbox..
					if (dUnit->RigidBody.Collider->BroadphaseAABB.Contains(m_Input.GetCursorPosition().World) 
						|| (boxWasDragged && dUnit->RigidBody.Collider->BroadphaseAABB.Intersects(box))) {
						// Either way, we need to make sure the selection list isn't cleared.
						noHit = false;

						// Only select the unit if it isn't selected already..
						if (!IsUnitSelected(unit.get()->ID))
							m_SelectedUnits.push_back(*unit);

						/* Unless of course we're multiselecting,
						   for which we might just want to be removing one of the units, as long as we're clicking and not dragging, anyway. */
						else if (m_Input.GetKeyMods().Shift && !boxWasDragged)
							Deselect(dUnit->ID);
					}
				}
				// If no unit was hit at all during the click, that is a call for a selection clear.
				if (noHit && !m_Input.GetKeyMods().Shift)
					m_SelectedUnits.clear();
			}

			// On right click:
			if (args.MouseButton == Input::MouseButton::Right && args.IsPressed) {
				// Simple move order (now featuring a really lazily implemented line formation.)
				glm::vec2 pos = m_Input.GetCursorPosition().World;
				for (auto unit : m_SelectedUnits) {
					m_Dispatcher->SendMoveOrder(unit.get().ID, pos);
					pos.x -= 0.7f;
				}
			}
		}
		
		void UnitSelect::OnKeyChange(const Input::KeyChangeEventArgs &args) {
			for (int i = 0; i < 9; i++) {
				// Control + 1-9 pressed, assign controlgroup from current selection.
				if (args.KeyId == Key[i] && m_Input.GetKeyMods().Control)
					m_ControlGroup[i] = m_SelectedUnits;

				if (args.KeyId == Key[i] && !m_Input.GetKeyMods().Control) {
					/* Control not held, Shift + 1-9 pressed,
					   indicating wanting to add that controlgroup's contents to the current selection. */
					if (m_Input.GetKeyMods().Shift) {
						std::list<std::reference_wrapper<IUnit>> tempList;
						/* Check if the current selection contains any unit in the controlgroup,
						   so that it is not selected twice. */
						for (auto unit : m_ControlGroup[i]) {
							// If the unit wasn't already selected, we add it to the temporary list.
							if (!IsUnitSelected(unit.get().ID))
								tempList.push_back(unit);
						}
						/* When all checking is done, go to the end of the selection list, and add
						   the contents of the temporary list there. */
						auto it = m_SelectedUnits.end();
						m_SelectedUnits.splice(it, tempList);

						// Done and done.
					}
					/* This else is referring to the check for if shift was held.
					   If shift was not held, you are simply clicking the button of the controlgroup,
					   and this just selects the controlgroup. */
					else
						m_SelectedUnits = m_ControlGroup[i];
				}
			}
		}
	}
}