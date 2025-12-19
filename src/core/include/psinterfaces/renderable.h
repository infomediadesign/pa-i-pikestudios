#pragma once

#include <psinterfaces/entity.h>

namespace PSInterfaces {
	class IRenderable : public virtual PSInterfaces::IEntity
	{
	public:
		~IRenderable() = default;

		virtual void render() = 0;
	};
}; // namespace PSInterfaces
