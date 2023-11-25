#pragma once
class Asset
{
protected:
	bool loaded = false;

public:
	virtual ~Asset() {}

	bool IsLoaded() const { return loaded; }
};

