#pragma once

#include "Agents.h"
#include "Players.h"
#include "World.h"

#include <CommonLib/Logger.h>
#include <Physics/Physics.h>

#include <memory>

namespace Server {
	namespace Network { class INetwork; }

	namespace Core {
		class WorldServer final {
			Utils::Logger m_Logger;

		public: // Initialization/Uninitialization
			WorldServer(Utils::ILoggingManager *loggingManager, Network::INetwork *network);
			~WorldServer();

		private: // Modules
			std::unique_ptr<Utils::ILoggingManager> m_LoggingManager;
			std::unique_ptr<Network::INetwork> m_Network;
			Physics::Physics m_Physics;
			Agents::Agents m_Agents;
			Players::Players m_Players;
			World::World m_World;

		public: // Module Accessors
			/// <summary>Gets the Logging Manager</summary>
			/// <returns>The Logging Manager</returns>
			Utils::ILoggingManager& GetLoggingManager();

			/// <summary>Gets the Network Module</summary>
			/// <returns>The Network Module</returns>
			Network::INetwork& GetNetwork();

			/// <summary>Gets the Physics Module</summary>
			/// <returns>The Physics Module</returns>
			Physics::Physics& GetPhysics();

			/// <summary>Gets the Agents Module</summary>
			/// <returns>The Agents Module</returns>
			Agents::Agents& GetAgents();

			/// <summary>Gets the Players Module</summary>
			/// <returns>The Agents Module</returns>
			Players::Players& GetPlayers();

			/// <summary>Gets the World Module</summary>
			/// <returns>The World Module</returns>
			World::World& GetWorld();

		public: // Game Loop
			/// <summary>Executes this Instance</summary>
			void Execute();
		};
	}
}