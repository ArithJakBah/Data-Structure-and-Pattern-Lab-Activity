#include "ShieldStack.h"

ShieldStack::ShieldStack()
	: fLayers(MAX_LAYERS), fPoints(0)
{
}

bool ShieldStack::addLayer(int aDurability)
{
	if (aDurability <= 0)
	{
		return false;
	}
	if (!pushLayer(ShieldLayer(aDurability)))
	{
		return false;                  // stack is full
	}
	fPoints += aDurability;
	return true;
}

int ShieldStack::absorb(int aDamage)
{
	while (aDamage > 0 && !layersEmpty())
	{
		ShieldLayer layer = popLayer();                 // the top (newest) layer

		int taken = layer.durability < aDamage ? layer.durability : aDamage;
		layer.durability -= taken;
		aDamage -= taken;
		fPoints -= taken;

		if (layer.durability > 0)
		{
			pushLayer(layer);                           // still standing: back on top
		}
		// otherwise the layer is used up and stays popped
	}
	return aDamage;
}

void ShieldStack::clear()
{
	while (!layersEmpty())
	{
		popLayer();
	}
	fPoints = 0;
}

int ShieldStack::layers() const
{
	return layerCount();
}

int ShieldStack::points() const
{
	return fPoints;
}
