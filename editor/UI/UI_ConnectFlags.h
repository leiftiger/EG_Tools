#pragma once

enum ConnectFlags : int
{
	None = 0,
	Above = 1 << 0,	// Connection should go above the states instead of below
	OffsetRight = 1 << 1,	// Connection should offset to the right
	OffsetLeft = 1 << 2,	// Connection should offset to the left
	OffsetUp = 1 << 3,	// Connection should offset upwards
	OffsetDown = 1 << 4,	// Connection should offset downwards
};