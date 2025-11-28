#pragma once

namespace PSInterfaces
{
	class Layer
	{
		public:
		virtual ~Layer() = default;
		
		virtual void on_event() {};
		virtual void on_update(const int dt) = 0;
		virtual void on_render(const int dt) = 0;
	};
}