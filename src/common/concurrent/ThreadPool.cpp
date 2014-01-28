#include "ThreadPool.h"
#include <stdexcept>
#include <typeinfo>

#include <boost/chrono.hpp>

namespace rocket { namespace concurrent {

ThreadPool::ThreadPool(int numberOfWorkers) : isShuttingDown(false), mainThreadId(boost::this_thread::get_id()) {
    auto worker = [this] {
        while (true) {
            std::unique_ptr<Task> task;
            {
                boost::lock_guard<boost::mutex> lock(taskQueueMutex);

                if (!taskQueue.empty()) {
                    task.swap(taskQueue.front());
                    taskQueue.pop_front();
                } else if (isShuttingDown) {
					break;
				}
            }

			if (task.get() != nullptr ) {
#ifdef THREAD_POOL_DEBUG
				LOGD("Executing task " << task.get());
#endif
            	(*task.get())();
			} else {
				boost::this_thread::yield();
			}
        }
    };

    for (int i = 0; i < numberOfWorkers; ++i) {
        workerThreads.push_back(boost::thread(worker));
    }
}

ThreadPool::~ThreadPool() {
    isShuttingDown = true;

	try {
    	for (auto& workerThread : workerThreads) {
        	workerThread.join();
    	}
	} catch(...) {
		abort();
	}
} 

}}
