#include "pch.h"
#include "PlayerCount.h"

BAKKESMOD_PLUGIN(PlayerCount, "Displays playlist player counts because Psyonix sucks!", plugin_version, PLUGINTYPE_FREEPLAY)

// ==============
//  This plugin uses RLSDK, you must have this in order to compile this plugin.
// ==============

void PlayerCount::onLoad()
{
	// better to use sigs here, but this is what we have for now.
	UINT_PTR base = (UINT_PTR)GetModuleHandleA(NULL);
	auto gobjects = base + GObjects_Offset;
	auto gnames = base + GNames_Offset;

	GObjects = reinterpret_cast<TArray<UObject*>*>(gobjects);
	GNames = reinterpret_cast<TArray<FNameEntry*>*>(gnames);

	if (!(Utils::areGObjectsValid() && Utils::areGNamesValid())) {
		cvarManager->log("Invalid GObjects or GNames");
		return;
	}

	if (!LoadClasses()) {
		cvarManager->log("Failed to load RLSDK classes");
		return;
	}

	gameWrapper->RegisterDrawable(std::bind(&PlayerCount::Render, this, std::placeholders::_1));

	// give the game time to fetch things (need to find a good event for this, probably)
	gameWrapper->SetTimeout([this](GameWrapper* _gw) {
		DumpPlaylists();
	}, 5.0f);

	// update playlist stuff if we can on menu enter
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", [this](std::string eventName) {
		DumpPlaylists();
	});
}

void PlayerCount::Render(CanvasWrapper canvas) {

	if (!gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()) {
		canvas.SetColor(120, 120, 120, 150);
		
		Vector2 screen = canvas.GetSize();
		Vector2 position = { screen.X - 305, 5 };
		canvas.SetPosition(position);

		const int height = m_Playlists.size() * 15 + 45;

		canvas.FillBox(Vector2{300, height});

		canvas.SetColor(255, 255, 255, 255);
		position.X += 15;

		if (m_Playlists.size() > 0) {
			// idk if this is good, but im doin it...
			auto it = std::max_element(m_Playlists.begin(), m_Playlists.end(), [](const PlaylistInfo& lhs, const PlaylistInfo& rhs) {
				return lhs.title.length() < rhs.title.length();
			});

			if (it == m_Playlists.end())
				return;

			PlaylistInfo maxPlaylist = *it;
			const int maxStrWidth = maxPlaylist.title.length();

			for (PlaylistInfo playlist : m_Playlists) {
				position.Y += 15;
				canvas.SetColor(255, 255, 255, 255);
				canvas.SetPosition(position);
				canvas.DrawString(playlist.title + ": ");

				canvas.SetPosition(Vector2{ position.X + maxStrWidth * 8, position.Y });
				if (playlist.playerCount > 5000)
					canvas.SetColor(45, 235, 25, 255);
				else if (playlist.playerCount > 1000)
					canvas.SetColor(235, 165, 25, 255);
				else
					canvas.SetColor(245, 85, 75, 255);

				canvas.DrawString(std::to_string(playlist.playerCount));
			}

			canvas.SetPosition(Vector2{ position.X, position.Y + 25 });
			canvas.SetColor(255, 255, 255, 255);
			canvas.DrawString("Total Population:");

			canvas.SetPosition(Vector2{ position.X + 115, position.Y + 25 });

			if (m_TotalPopulation > 100000)
				canvas.SetColor(45, 235, 25, 255);
			else if (m_TotalPopulation > 50000)
				canvas.SetColor(235, 165, 25, 255);
			else
				canvas.SetColor(245, 85, 75, 255);

			canvas.DrawString(std::to_string(m_TotalPopulation));
		}
	}
}

void PlayerCount::DumpPlaylists() {
	LOG("Dumping playlists...");
	m_Playlists.clear();

	if (IUOnlineGamePopulation_X) {
		m_TotalPopulation = IUOnlineGamePopulation_X->TotalPlayers;

		for (FGetPopulationData populationData : IUOnlineGamePopulation_X->Populations) {
			const int playlistId = populationData.Playlist;
			const int playerCount = populationData.PlayerCount;

			UGameSettingPlaylist_X* playlist = IUOnlineGamePlaylists_X->GetPlaylistByID(playlistId);

			if (playlist && IUOnlineGamePlaylists_X->IsPlaylistEnabled(playlist) && playerCount > 1) {
				std::ostringstream ss;
				if (playlist->bRanked)
					ss << "[R] ";
				else
					ss << "[C] ";

				ss << playlist->Title.ToString();

				m_Playlists.push_back(PlaylistInfo{ ss.str(), playlistId, playerCount });
			}
		}
	}
	else
		LOG("[DumpPlaylists] ERROR: Game population not set, yike!");
}

bool PlayerCount::LoadClasses() {
	// Init the RLSDK classes here.

	return ClassesSafe;
}

void PlayerCount::onUnload()
{
	gameWrapper->UnregisterDrawables();
}