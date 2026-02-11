//
// Created by rikab on 29/01/2026.
//
#pragma once 
#include <psinterfaces/layer.h>
#include "scorelayer.h"

class DeathScreenLayer : public PSInterfaces::Layer
{
	public:
		void on_update(float dt) override;
		void on_render() override;

		void set_score_should_be_saved(bool should_be_saved);
		bool score_should_be_saved() const;
		void set_score_layer_instance(ScoreLayer* score_layer);

	private:
		bool m_score_should_be_saved = false;
		bool m_name_entered				   = false;
		ScoreLayer* m_score_layer_instance = nullptr;
};


