#pragma once

#include "HTTPConnection.h"
#include "Node.h"

class DomainPrometheusExporter {
public:
	typedef enum {
		Untyped,
		Counter,
		Gauge,
		Histogram,
		Summary
	} MetricType;

	static bool handleRequest(HTTPConnection* connection);
private:
	static QString generateMeticsForNode(const SharedNodePointer& node);
};