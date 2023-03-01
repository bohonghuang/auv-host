//
// Created by Qff on 2023/3/1.
//

#include "block.h"

namespace auv::algorithm
{

void Block::start() noexcept
{
	if (m_is_running)
		return;

	m_is_running = true;
	m_thread = std::thread(
		[this]()
		{
			while (m_is_running) {
				// this->run();
			}
		});
}

void Block::stop() noexcept
{

}

const AlgorithmResult &auv::algorithm::Block::test_once() noexcept
{
	// this->run();
	return m_result;
}

AlgorithmResult auv::algorithm::Block::getResult() noexcept
{
	AlgorithmResult result;
	m_mutex.lock();
	result = m_result;
	m_mutex.unlock();

	return result;
}

}