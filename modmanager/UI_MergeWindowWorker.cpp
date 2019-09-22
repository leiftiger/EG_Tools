#include "UI_MergeWindowWorker.h"

#include <QApplication>

#include <thread>
#include <queue>

void UI_MergeWindowWorker::signalLogic(Mutex::Client &mutex)
{
	while (!mutex.shouldTerminate())
	{
		if (mutex.isWaiting())
		{

			while (!mutex.queue.empty())
			{
				emit mergeOutput(QString::fromStdString(mutex.queue.front()));
				mutex.queue.pop();
			}

			emit mergeProgress(mutex.progress);

			mutex.acknowledge();
		}
		else
		{
			QApplication::processEvents();
			std::this_thread::sleep_for(std::chrono::nanoseconds(100));
		}
	}
}

void UI_MergeWindowWorker::mergeLogic(Mutex::Server &mutex)
{
	m_merger.mergeMods(mutex);
}

UI_MergeWindowWorker::UI_MergeWindowWorker(QObject *parent, ResourceMerger &merger) : QObject(parent), m_merger(merger)
{

}

UI_MergeWindowWorker::~UI_MergeWindowWorker()
{

}

void UI_MergeWindowWorker::performMerging()
{
	Mutex::Pair pair;

	std::thread threadSignal(&UI_MergeWindowWorker::signalLogic, this, std::ref(pair.client()));
	std::thread threadMerge(&UI_MergeWindowWorker::mergeLogic, this, std::ref(pair.server()));

	threadMerge.join();

	pair.terminate();

	threadSignal.join();

	emit mergeProgress(1.1);
}
