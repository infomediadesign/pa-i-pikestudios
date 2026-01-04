#pragma once

namespace PSInterfaces {

    class IMediator;

	class IEntity {
		public:
		~IEntity() = default;

		virtual void update(float dt) = 0;

		void set_mediator(IMediator* med) {
		    this->mediator_ = med;
		}

		protected:
		IMediator* mediator_ = nullptr;
	};
}
