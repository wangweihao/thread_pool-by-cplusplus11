/*
 * =====================================================================================
 *
 *       Filename:  thread_pool.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年07月18日 00时30分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (wangweihao), 578867817@qq.com
 *        Company:  xiyoulinuxgroup
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SynQueue.h"
#include <functional>
#include <thread>
#include <memory>
#include <atomic>

const int MaxTaskCount = 100;

class ThreadPool
{
    using Task = std::function<void()>;
    ThreadPool(int numThreads = std::thread::hardware_concurrency()):
        m_queue(MaxTaskCount) 
    {
        Start(numThreads);
    }

    ~ThreadPool(void)
    {
        Stop();
    }

    void Stop()
    {
        std::call_once(m_flag, [this] { StopThreadGroup(); });
    }

    void AddTask(const Task& task)
    {
        m_queue.Put(task);
    }

    void AddTask(Task && task)
    {
        m_queue.Put(std::forward<Task>(task));
    }

    private:
        void StopThreadGroup()
        {
            m_queue.Stop();
            m_running = false;
            for(auto thread : m_threadgroup)
            {
                if(thread)
                    thread->join();
            }
            m_threadgroup.clear();
        }

        void Start(int numThreads)
        {
            m_running = true;
            for(int i = 0; i < numThreads; ++i)
            {
                m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
            }
        }

        void RunInThread()
        {
            while(m_running)
            {
                std::list<Task> list;
                m_queue.Take(list);
                for(auto &task : list)
                {
                    if(!m_running)
                        return;
                    task();
                }
            }
        }

    private:
        std::list<std::shared_ptr<std::thread>> m_threadgroup;
        SynQueue<Task>m_queue;
        std::atomic_bool m_running;
        std::once_flag m_flag;
};

int main(int argc, char *argv[])
{

	

	return EXIT_SUCCESS;
}


