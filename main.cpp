#include <QCoreApplication>
#include "query.hpp"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	query_service::CQuery q("46.174.53.176", 7777);

	// Ping
	q.fetchData(query_service::request_response_type::ping);
	qDebug() << q.getPing();

	// Information
	q.fetchData(query_service::request_response_type::information);
	qDebug() << "password: " << q.getHasPassword();
	qDebug() << "player count: " << q.getPlayerCount();
	qDebug() << "max players: " << q.getMaxPlayers();
	qDebug() << "hostname: " << q.getHostname();
	qDebug() << "gamemode: " << q.getGamemode();
	qDebug() << "language: " << q.getLanguage();

	// Rules
	q.fetchData(query_service::request_response_type::rules);
	QVector<query_service::CData::rule> rules(q.getRules());

	foreach(query_service::CData::rule rl, rules)
		qDebug() << rl.rule << " : " << rl.value;

	// Clients
	q.fetchData(query_service::request_response_type::clients);
	QVector<query_service::CData::client> clients(q.getClients());

	foreach(query_service::CData::client cl, clients)
		qDebug() << cl.player_name << " : " << cl.score;

	// Detailed clients
	q.fetchData(query_service::request_response_type::detailed_clients);
	QVector<query_service::CData::player> players(q.getPlayers());

	foreach(query_service::CData::player pl, players)
		qDebug() << pl.id << " : " << pl.player_name << " : " << pl.score << " : " << pl.ping;

	return a.exec();
}
