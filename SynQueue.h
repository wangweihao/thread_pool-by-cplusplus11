/*
 * =====================================================================================
 *
 *       Filename:  SynQueue.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年07月17日 23时56分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (wangweihao), 578867817@qq.com
 *        Company:  xiyoulinuxgroup
 *
 * =====================================================================================
 */

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>

template<typename T>
class SynQueue
{
    public:
        SynQueue(int maxsize):
            m_maxSize(maxsize), m_needStop(false) { }

        //添加事件
        void Put(const T&x)
        {
            Add(x);
        }
        void Put(T &&x)
        {
            Add(x);
        }

        //从队列中取事件,取所有事件
        void Take(std::list<T> &list)
        {
            //有wait方法必须用unique_lock
            //但unique_lock的性能稍低于lock_guard
            std::unique_lock<std::mutex> locker(m_mutex);
            m_notEmpty.wait(locker, [this]
                    { return m_needStop || NotEmpty(); });
            if(m_needStop)
                return;
            list = std::move(m_queue);
            m_notFull.notify_one();
        }

        //取一个事件
        void Take(T &t)
        {
            std::unique_lock<std::mutex> locker(m_mutex);
            m_notEmpty.wait(locker, [this]
                    { return m_needStop || NotEmpty(); });
            if(m_needStop)
                return;
            m_queue.pop_front();
            m_notFull.notify_one();
        }

        void Stop()
        {
            {
                std::lock_guard<std::mutex> locker(m_mutex);
                m_needStop = true;
            }
            m_notFull.notify_all();
            m_notEmpty.notify_all();
        }

        bool Empty()
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_queue.empty();
        }

        bool Full()
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_queue.size() == m_maxSize;
        } 

        size_t Size()
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_queue.size();
        }

    
    private: 
        //往队列里添加事件
        template<typename F>
        void Add(F &&x)
        {
            std::unique_lock<std::mutex> locker(m_mutex);
            m_notFull.wait(locker, [this] { 
                    return m_needStop || NotFull() ; });
            if(m_needStop)
                return;
            m_queue.push_back(std::forward<F>(x));
            m_notEmpty.notify_one();
        }

        bool NotFull() const
        {
            bool full = m_queue.size() >= m_maxSize;
            if(full)
                std::cout << "缓冲区满了...请等待" << std::endl;
            return !full;
        }

        bool NotEmpty() const
        {
            bool empty = m_queue.empty();
            if(empty)
            {
                std::cout << "缓冲区空了...请等待" << std::endl;
                std::cout << "线程ID:" << std::this_thread::get_id() << std::endl;
            }
        }

    private:
        std::mutex m_mutex;      //互斥锁
        std::list<T> m_queue;    //队列，存放任务
        std::condition_variable m_notEmpty;  //队列不为空的条件变量
        std::condition_variable m_notFull;   //队列不为满的条件变量
        int m_maxSize;           //任务队列最大长度 
        bool m_needStop;         //终止标识

};
