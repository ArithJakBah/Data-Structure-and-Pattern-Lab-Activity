#pragma once
#include "Stack.h"

// One protective layer. It soaks up damage until its durability runs out.
struct ShieldLayer
{
	int durability;

	ShieldLayer() noexcept : durability(0) {}
	explicit ShieldLayer(int aDurability) noexcept : durability(aDurability) {}
};

// Layers are pushed on top of each other. Damage always hits the TOP (newest) layer first,
// and whatever it cannot stop falls through to the layer below, then to the HP.
class ShieldStack
{
public:
	static constexpr int MAX_LAYERS = 5;

private:
	Stack<ShieldLayer> fLayers;
	int fPoints;                       // total durability of all layers (for the screen)

	// ---- the only four lines that touch your Stack class. ----
	// If your Stack uses other names (e.g. peek/top), change them here and nothing else.
	bool        pushLayer(const ShieldLayer& aLayer) { return fLayers.push(aLayer); }
	ShieldLayer popLayer()                           { return fLayers.pop(); }
	bool        layersEmpty() const                  { return fLayers.isEmpty(); }
	int         layerCount() const                   { return fLayers.size(); }

public:
	ShieldStack();

	bool addLayer(int aDurability);    // false if the stack is full or the durability is not positive
	int  absorb(int aDamage);          // returns the damage that got through
	void clear();

	int  layers() const;
	int  points() const;
};
