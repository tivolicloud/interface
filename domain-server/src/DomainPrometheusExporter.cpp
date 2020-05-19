#include <QUrl>

#include "DomainPrometheusExporter.h"
#include "HTTPConnection.h"
#include "DependencyManager.h"
#include "LimitedNodeList.h"
#include "DomainServerNodeData.h"

QString DomainPrometheusExporter::generateMeticsForNode(
	const SharedNodePointer& node
) {
	QString nodeType = NodeType::getNodeTypeName(
		static_cast<NodeType_t>(node->getType())
	);

	return nodeType;
}

bool DomainPrometheusExporter::handleRequest(HTTPConnection* connection) {
	QString test = QString("");
	
	auto nodeList = DependencyManager::get<LimitedNodeList>();

	nodeList->eachNode(
		[&](const SharedNodePointer& node) {
			test += generateMeticsForNode(node) + "\n";
		}
	);
	
	connection->respond(
		HTTPConnection::StatusCode200, 
		test.toUtf8(), 
		qPrintable("text/plain")
	);

	return true;
}