#pragma once

#include <QObject>

namespace query_service
{
class CData
{
public:
	struct rule
	{
		QString rule;
		QString value;
	};

	struct client
	{
		QString player_name;
		qint32 score;
	};

	struct player
	{
		unsigned char id;
		QString player_name;
		qint32 score;
		quint32 ping;
	};
};
} // namespace query_service
