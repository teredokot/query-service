#include "requests_required.hpp"

query_service::CRequestsRequired::CRequestsRequired(bool initial_value) : values(request_response_type::num_of_items), available(values.size()), last_request_time(values.size())
{
	QDateTime now = QDateTime::currentDateTimeUtc();
	for(qint32 i = 0; i < values.size(); i++)
	{
		values[i] = initial_value;
		last_request_time[i] = now;
		available[i] = true;
	}
}

bool &query_service::CRequestsRequired::operator [](request_response_type request_response_type)
{
	return values[request_response_type];
}

QChar query_service::CRequestsRequired::getOpCode(request_response_type request_response_type)
{
	return op_codes[request_response_type];
}

QDateTime query_service::CRequestsRequired::getLastRequestTime(request_response_type request_response_type)
{
	return last_request_time[request_response_type];
}

void query_service::CRequestsRequired::setLastRequestTime(request_response_type request_response_type)
{
	last_request_time[request_response_type] = QDateTime::currentDateTimeUtc();
}

void query_service::CRequestsRequired::lock(request_response_type request_response_type)
{
	available[request_response_type] = false;
	values[request_response_type] = false;
}
