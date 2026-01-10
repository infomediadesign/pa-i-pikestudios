#include <pscore/time.h>

#include <thread>

void PSCore::DeltaTimeManager::calc_delta_t()
{
	m_current_t	   = std::chrono::steady_clock::now();
	m_update_t = m_current_t - m_prev_t;

	// calculate the delta time
	if ( m_target_fps > 0 ) // We want a fixed frame rate
	{
		m_wait_t = std::chrono::duration<double>((1.0 / m_target_fps) - m_update_t.count());
		if ( m_wait_t.count() > 0.0 ) {
			std::this_thread::sleep_for(m_wait_t);
			m_current_t = std::chrono::steady_clock::now();
			m_delta_t	= m_current_t - m_prev_t;
		}
	} else
		m_delta_t = m_update_t;

	m_prev_t = m_current_t;
}
void PSCore::DeltaTimeManager::set_target_fps(int fps)
{
	m_target_fps = fps;
}
int PSCore::DeltaTimeManager::target_fps()
{
	return m_target_fps;
}

std::chrono::duration<double> PSCore::DeltaTimeManager::delta_t()
{
	return m_delta_t;
}
