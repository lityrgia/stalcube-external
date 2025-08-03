#pragma once
#include "ext/imgui/imgui.h"
#include "json.h"

namespace menu {
	inline bool bMenuVisible{ true };

	inline unsigned short bToggleButton{ 0x2d };
	inline unsigned short bEndButton{ 0x23 };

	inline bool consoleAllocated{ false };
}

namespace options {
	inline bool bAimbotEnabled{ false };
	inline bool bSmoothnessEnabled{ false };
	inline bool bTriggerbotEnabled{ false };
	inline bool bFovEnabled{ false };

	inline bool bEspEnabled{ false };
	inline bool bPreviewEnabled{ false };
	inline bool bTeamVisibleEnabled{ false };
	inline bool b2DBoxEnabled{ false };
	inline bool bPlayerBoxEnabled{ false };
	inline bool bTroopBoxEnabled{ false };
	inline bool bItemBoxEnabled{ false };
	inline bool bNPCBoxEnabled{ false };
	inline bool bGameItemsEnabled{ false };
	inline bool bSnaplinesEnabled{ false };
	inline bool bHealthEnabled{ false };
	inline bool bNamesEnabled{ false };

	inline float smoothness{ 1.f };
	inline float radiusFOV{ 10.f };
	inline float snaplinesThickness{ 0.7f };
	inline float boxesThickness{ 0.7f };
	inline float boxesRounding{ 0.7f };

	inline int espButton{};
	inline int aimbotButton{};
	inline int triggerbotButton{};

	inline int triggerbotDelay{ 1 };

	inline int aimTypeButton{ 2 };

	inline int monitorX;
	inline int monitorY;
	inline int mon_posX;
	inline int mon_posY;

	inline ImVec4 playersColor{255, 0, 0, 255};
	inline ImVec4 itemsColor{255, 0, 0, 255};
	inline ImVec4 npcsColor{255, 0, 0, 255};
	inline ImVec4 troopsColor{255, 0, 0, 255};
	inline ImVec4 playersTeamColor{0, 0, 255, 255};
}

namespace j {
	inline nlohmann::json cfg;
}