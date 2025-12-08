#pragma once

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
	};
} // namespace PSInterfaces
