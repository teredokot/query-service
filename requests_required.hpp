#pragma once

#include <QVector>
#include <QDateTime>

#include "request_response_type.hpp"

namespace query_service
{
class CRequestsRequired
{
public:
	CRequestsRequired(bool initial_value);
	bool &operator [](request_response_type request_response_type);

	QChar getOpCode(request_response_type request_response_type);

	QDateTime getLastRequestTime(request_response_type request_response_type);
	void setLastRequestTime(request_response_type request_response_type);

	void lock(request_response_type request_response_type);

private:
	QVector<bool> values;
	QVector<bool> available;
	QVector<QDateTime> last_request_time;
	QVector<QChar> op_codes { 'p', 'i', 'r', 'c', 'd' };
};
} // namespace query_service
