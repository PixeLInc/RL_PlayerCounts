#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "Utils.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class PlayerCount: public BakkesMod::Plugin::BakkesModPlugin
{
	virtual void onLoad();
	virtual void onUnload();
public:
	bool LoadClasses();
	void DumpPlaylists();
	void Render(CanvasWrapper canvas);

private:
	bool ClassesSafe = false;
	class UOnlineGamePopulation_X* IUOnlineGamePopulation_X = nullptr;
	class UOnlineGamePlaylists_X* IUOnlineGamePlaylists_X = nullptr;

	struct PlaylistInfo {
		std::string title;
		int id;
		int playerCount;
	};

	std::vector<PlaylistInfo> m_Playlists;
	int m_TotalPopulation;
};

