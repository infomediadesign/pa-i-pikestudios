#pragma once

namespace PSInterfaces {
	class IRenderable
	{
	public:
		~IRenderable() = default;

		virtual void render() = 0;
	};
}; // namespace PSInterfaces
