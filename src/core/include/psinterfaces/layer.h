#pragma once

#include <memory>
#include <pscore/renderer.h>

namespace PSInterfaces {
	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void on_event() {};
		virtual void on_update(const float dt) = 0;
		virtual void on_render() = 0;

		void suspend()
		{
			active = false;
		};

		void resume()
		{
			active = true;
		};

	protected:
		bool active = true;
		std::unique_ptr<PSCore::Renderer> renderer_ = std::make_unique<PSCore::Renderer>();
	};
} // namespace PSInterfaces
