#include "MoveBehavior.h"

#include "BaseUnitTypes.h"

#include <CommonLib/postypes.h>

namespace Server {
	namespace Units {
		namespace AI {
			// Properties

			void MoveBehavior::SetSlowdownDistance(float slowdownDistance) {
				m_SqrSlowdownDistance = slowdownDistance * slowdownDistance;
				m_InvSlowdownDistance = 1.0f / slowdownDistance;
			}

			// Initialization/Uninitialization

			MoveBehavior::MoveBehavior(float speed) :
				m_Target(),
				m_Enabled(false),

				m_SqrSlowdownDistance(0.5f * 0.5f),
				m_InvSlowdownDistance(1.0f / 0.5f),

				Speed(speed),
				Deadzone(0.01f)
			{}

			// Helper Functions

			void MoveBehavior::MoveTo(glm::vec2 target) {
				m_Target = std::move(target);
				m_Enabled = true;
			}

			// Game Loop

			void MoveBehavior::Update(const float, AIUnit &unit) {
				if (m_Enabled) {
					// Calculate Position Difference Data
					glm::vec2 posDiff(m_Target.x - unit.RigidBody.Position.x, m_Target.y - unit.RigidBody.Position.y);

					// If we're not there yet
					if (fabs(posDiff.x) > Deadzone || fabs(posDiff.y) > Deadzone) {
						// Normalize Difference so Velocity Vector Length is 1
						glm::vec2 tarVel = glm::normalize(posDiff);

						// Make sure Unit Slows down a bit in Advance
						float sqDistance = (posDiff.x * posDiff.x) + (posDiff.y * posDiff.y);
						if (sqDistance < m_SqrSlowdownDistance)
							unit.RigidBody.TargetVelocity = tarVel * Speed * (sqrtf(sqDistance) * m_InvSlowdownDistance);
						else
							unit.RigidBody.TargetVelocity = tarVel * Speed;

						// Face Towards the Target
						unit.Rotation = -atan2f(posDiff.x, posDiff.y);
					}
					else {
						unit.RigidBody.TargetVelocity = glm::vec2(0, 0);
						m_Enabled = false;
					}
				}
			}
		}
	}
}