#pragma once

#include <QObject>

#include "ResourceMerger.h"

class UI_MergeWindowWorker : public QObject
{
	Q_OBJECT

protected:

	ResourceMerger &m_merger;

	void signalLogic(Mutex::Client &mutex);
	void mergeLogic(Mutex::Server &mutex);

public:
	UI_MergeWindowWorker(QObject *parent, ResourceMerger &merger);
	~UI_MergeWindowWorker();

public slots:
	void performMerging();

signals:
	void mergeOutput(QString str);
	void mergeProgress(double progress);
};
