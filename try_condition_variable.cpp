/*
 * =====================================================================================
 *
 *       Filename:  try.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年07月18日 13时10分52秒
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
#include <memory>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>

using f = std::function<void()>;
using Sp = std::shared_ptr<std::thread>;

std::mutex m_mutex;
std::condition_variable m_notify;

void tfunc(std::queue<f> *q)
{
    std::lock_guard<std::mutex> locker(m_mutex);
    if(q->size() == 0)
    {
        return;
    }
    auto f = q->back();
    q->pop();
    f();
    std::cout << "notify one" << std::endl;
    m_notify.notify_one();
    std::cout << "queue size:" << q->size() << std::endl;
   // std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
}

int main(int argc, char *argv[])
{
    //任务队列
    std::queue<f>m_queue;
    for(int i = 0; i < 10; i++)
    {
        m_queue.push([i](){
                std::cout << "我是一个函数对象" << std::endl;
                std::cout << "我要执行啦~" << std::endl;
                });
    }

    std::vector<std::shared_ptr<std::thread>>tvec;
    for(int i = 0; i < 10; i++)
    {
        tvec.push_back(std::make_shared<std::thread>(tfunc, &m_queue));
        tvec[i]->join();
    }

    std::this_thread::sleep_for(std::chrono::seconds(11));

	return EXIT_SUCCESS;
}


