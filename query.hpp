#pragma once

#include <QObject>
#include <QUdpSocket>

#include "requests_required.hpp"
#include "request_response_type.hpp"
#include "data.hpp"

namespace reader
{
	template <typename T>
	union byte_value
	{
		T value;
		char bytes[sizeof( T )];
	};

	template <typename T>
	T read(QByteArray &array, quint32 &offset);
	template <>
	QString read(QByteArray &array, quint32 &offset);

	QString read_limited(QByteArray &array, quint32 &offset);
} // namespace reader

namespace query_service
{
class CQuery : public QObject
{
	Q_OBJECT

public:
	CQuery(const QString ip, const quint16 port);
	~CQuery();

	void fetchData(request_response_type request_response_type);

	bool getHasPassword() { return has_password; }
	quint16 getPlayerCount() { return player_count; }
	quint16 getMaxPlayers() { return max_players; }
	QString getHostname() { return hostname; }
	QString getGamemode() { return gamemode; }
	QString getLanguage() { return language; }

	QVector<CData::rule> getRules() { return rules; }
	QVector<CData::client> getClients() { return clients; }
	QVector<CData::player> getPlayers() { return players; }

	long long getPing() { return ping; }

public slots:
	void readPendingDatagrams();

private:
	QUdpSocket *socket;
	CRequestsRequired requests_required = CRequestsRequired(true);

	bool sendQuery(request_response_type request_response_type);

	template<typename T>
	T read(QByteArray &array, quint32 &offset)
	{
		return reader::read<T>(array, offset);
	}

	QString ip;
	quint16 port;
	QByteArray sentData;

	bool has_password;
	quint16 player_count;
	quint16 max_players;
	QString hostname;
	QString gamemode;
	QString language;

	QVector<CData::rule> rules;
	QVector<CData::client> clients;
	QVector<CData::player> players;

	long long ping;
	QDateTime timestamp[2];
};
} // namespace query_service
