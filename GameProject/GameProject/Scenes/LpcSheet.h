#pragma once

// Layout of the standard LPC (Liberated Pixel Cup) character sheet:
// 64x64 frames, 13 columns. Every animation uses 4 rows, one per direction,
// in this order: up, left, down, right.
namespace Lpc
{
	constexpr int FRAME_SIZE = 64;

	constexpr int SPELLCAST_ROW = 0;    constexpr int SPELLCAST_FRAMES = 7;
	constexpr int THRUST_ROW    = 4;    constexpr int THRUST_FRAMES    = 8;
	constexpr int WALK_ROW      = 8;    constexpr int WALK_FRAMES      = 9;    // frame 0 = standing, 1-8 = walking
	constexpr int SLASH_ROW     = 12;   constexpr int SLASH_FRAMES     = 6;
	constexpr int SHOOT_ROW     = 16;   constexpr int SHOOT_FRAMES     = 13;
	constexpr int HURT_ROW      = 20;   constexpr int HURT_FRAMES      = 6;    // one row only

	enum Dir { Up = 0, Left = 1, Down = 2, Right = 3 };
}
