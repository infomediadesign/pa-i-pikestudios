#include <pscore/application.h>
#include <pscore/viewport.h>
#include <psinterfaces/layer.h>
#include <raylib.h>

PSInterfaces::Layer::Layer()
{
}

void PSInterfaces::Layer::resume()
{
	active = true;
};

void PSInterfaces::Layer::suspend()
{
	active = false;
};

const std::unique_ptr<PSCore::Renderer>& PSInterfaces::Layer::renderer()
{
	return renderer_;
}

std::vector<std::weak_ptr<PSInterfaces::IEntity>> PSInterfaces::Layer::entities() const
{
	return entities_;
}
