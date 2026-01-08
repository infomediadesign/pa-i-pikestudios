#pragma once

#include <psinterfaces/entity.h>
#include <psinterfaces/events.h>

namespace PSInterfaces {
	class IRenderable : public virtual PSInterfaces::IEntity
	{
	public:
		virtual ~IRenderable() = default;

		virtual void render() = 0;

		int z_index()
		{
			return z_index_;
		};

		void propose_z_index(int i)
		{
			z_index_ = i;
			notify_all({.name = "propose_z_index"});
		}

	protected:
		int z_index_ = 0;
	};
}; // namespace PSInterfaces
