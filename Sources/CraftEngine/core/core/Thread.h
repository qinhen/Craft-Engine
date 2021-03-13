#pragma once
#include "./Callable.h"

namespace CraftEngine
{
	namespace core
	{

		class Thread
		{
		private:
			void RunningLoop()
			{
				Command<void> default_task;
				Command<void>& task = default_task;
				while (true)
				{
					{
						std::unique_lock<std::mutex> lock(m_queueMutex);
						m_condition.wait(lock, [this]() { return !m_tasksQueue.empty() || m_destroying; });
						if (m_destroying)
							break;
						task = m_tasksQueue.front();
						m_executing = true;
					}
					task.execute();		
					{
						std::lock_guard<std::mutex> lock(m_queueMutex);
						m_tasksQueue.pop();
						if (m_tasksQueue.size() == 0)
							m_executing = false;
						m_condition.notify_one();
					}
				}
			}

		public:
			Thread()
			{
				m_thread = std::thread(&Thread::RunningLoop, this);
			}

			~Thread()
			{
				if (m_thread.joinable())
				{
					wait();
					m_queueMutex.lock();
					m_destroying = true;
					m_condition.notify_one();
					m_queueMutex.unlock();
					m_thread.join();
				}
			}

			void push(std::function<void()> task)
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				Command<void> cmd;
				cmd.wrap(task);
				m_tasksQueue.push(std::move(cmd));
				m_condition.notify_one();
			}

			void push(Command<void> task)
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				m_tasksQueue.push(std::move(task));
				m_condition.notify_one();
			}

			void clear()
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				while (m_tasksQueue.size() > 1)
					m_tasksQueue.pop();
			}

			void wait()
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_condition.wait(lock, [this]() { return m_tasksQueue.empty() && m_executing == false; });
			}

			void stop()
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				while (m_tasksQueue.size() > 1)
					m_tasksQueue.pop();
				m_condition.wait(lock, [this]() { return m_tasksQueue.empty() && m_executing == false; });
			}

			bool finished()
			{
				return m_executing == false;
			}

			uint32_t count()
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				return m_tasksQueue.size();
			}

		private:
			bool m_destroying = false;
			bool m_executing = false;
			std::thread m_thread;
			std::queue<Command<void>> m_tasksQueue;
			std::mutex m_queueMutex;
			std::condition_variable m_condition;
		};


		class ThreadPool
		{
		public:
			void init(uint32_t count)
			{
				m_threads.clear();
				assert(count <= 48);
				for (auto i = 0; i < count; i++)
					m_threads.push_back(std::unique_ptr<Thread>(new Thread()));
			}

			void wait(int32_t thread_id)
			{
				if (thread_id < threadCount())
					m_threads[thread_id]->wait();
			}

			void wait()
			{
				for (auto& thread : m_threads)
					thread->wait();
			}

			void stop(int32_t thread_id)
			{
				if (thread_id < threadCount())
					m_threads[thread_id]->stop();
			}

			void stop()
			{
				for (auto& thread : m_threads)
					thread->stop();
			}

			uint32_t threadCount() const
			{
				return m_threads.size();
			}

			void push(std::function<void()> task, int threadid = 0)
			{
				if (threadid < threadCount())
					m_threads[threadid]->push(task);
				else
					m_threads[0]->push(task);
			}

			void push(std::vector<std::function<void()>> tasks)
			{
				uint32_t threadid = 0;
				for (uint32_t taskid = 0; taskid < tasks.size(); taskid++)
				{
					threadid = taskid % threadCount();
					m_threads[threadid]->push(tasks[taskid]);
				}
			}

			void push(std::vector<Command<void>> tasks)
			{
				uint32_t threadid = 0;
				for (uint32_t taskid = 0; taskid < tasks.size(); taskid++)
				{
					threadid = taskid % threadCount();
					m_threads[threadid]->push(tasks[taskid]);
				}
			}

			bool finished() const 
			{
				for (auto& thread : m_threads)
					if (!thread->finished())
						return false;
				return true;
			}
		private:
			std::vector<std::unique_ptr<Thread>> m_threads;
		};



	}
}