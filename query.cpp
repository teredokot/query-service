#include "query.hpp"

#include <QTextCodec>

query_service::CQuery::CQuery(const QString ip, const quint16 port) : ip(ip), port(port)
{
	socket = new QUdpSocket(this);
	if(!socket->bind(port, QUdpSocket::ShareAddress))
		qDebug() << "query_service::CQuery::CQuery: Failed to bind socket";
	else
	{
		if(socket->socketDescriptor() == -1)
			qDebug() << "query_service::CQuery::CQuery: Failed to obtain UDP socket descriptor, network sensing is not possible";
		else
		{
			if(!connect(socket, &QUdpSocket::readyRead, this, &CQuery::readPendingDatagrams))
				qDebug() << "query_service::CQuery::CQuery: Failed to connect multicast socket readyRead() to handler";
		}
	}
}

query_service::CQuery::~CQuery()
{
	socket->close();
	delete socket;
}

void query_service::CQuery::fetchData(request_response_type request_response_type)
{
	requests_required[request_response_type] = true;
	sendQuery(request_response_type);
}

void query_service::CQuery::readPendingDatagrams()
{
	while (socket->hasPendingDatagrams())
	{
		QByteArray datagram(static_cast<qint32>(socket->pendingDatagramSize()), Qt::Uninitialized);
		socket->readDatagram(datagram.data(), datagram.size());
		timestamp[1] = QDateTime::currentDateTimeUtc();

		if(datagram.length() > 10 && datagram.startsWith(sentData))
		{
			switch(datagram[10])
			{
			case 'p':
				ping = timestamp[1].toMSecsSinceEpoch() - timestamp[0].toMSecsSinceEpoch();
				requests_required[request_response_type::ping] = false;
				break;
			case 'i':
			{
				quint32 offset = 12;
				has_password = datagram[11]; // Password
				player_count = read<quint16>(datagram, offset); // Player count
				max_players = read<quint16>(datagram, offset); // Max players
				hostname = read<QString>(datagram, offset); // Hostname
				gamemode = read<QString>(datagram, offset); // Gamemode
				language = read<QString>(datagram, offset); // Language
				requests_required[request_response_type::information] = false;
				break;
			}
			case 'r':
			{
				quint32 offset = 11;
				quint16 count = read<quint16>(datagram, offset);
				for(qint32 i = 0; i < count; i++)
				{
					CData::rule rl;
					rl.rule = reader::read_limited(datagram, offset);
					rl.value = reader::read_limited(datagram, offset);
					rules.push_back(rl);
				}
				requests_required[request_response_type::rules] = false;
				break;
			}
			case 'c':
			{
				quint32 offset = 11;
				quint16 count = read<quint16>(datagram, offset);
				for(qint32 i = 0; i < count; i++)
				{
					CData::client cl;
					cl.player_name = reader::read_limited(datagram, offset);
					cl.score = read<qint32>(datagram, offset);
					clients.push_back(cl);
				}
				requests_required[request_response_type::clients] = false;
				break;
			}
			case 'd':
			{
				quint32 offset = 11;
				quint16 count = read<quint16>(datagram, offset);
				for(qint32 i = 0; i < count; i++)
				{
					CData::player pl;
					pl.id = read<unsigned char>(datagram, offset);
					pl.player_name = reader::read_limited(datagram, offset);
					pl.score = read<qint32>(datagram, offset);
					pl.ping = read<quint32>(datagram, offset);
					players.push_back(pl);
				}
				requests_required[request_response_type::detailed_clients] = false;
				break;
			}
			}
		}
	}
	sentData.clear();
}

bool query_service::CQuery::sendQuery(request_response_type request_response_type)
{
	bool ret = false;
	requests_required.setLastRequestTime(request_response_type);
	QStringList host = ip.split(".");
	QChar op_code = requests_required.getOpCode(request_response_type);
	QByteArray datagram;

	datagram.append("SAMP");
	for(qint32 i = 0; i < 4; i++)
		datagram.append(static_cast<char>(host.at(i).toInt()));
	datagram.append(static_cast<char>(port & 0xFF));
	datagram.append(static_cast<char>(port >> 8 & 0xFF));
	datagram.append(op_code);
	if(op_code == 'p')
		for (qint32 i = 0; i < 4; i++)
			datagram.append(static_cast<char>((qrand() % 8) + 1));

	timestamp[0] = QDateTime::currentDateTimeUtc();

	sentData.append(datagram);

	if(socket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress(ip), port) == -1)
		qWarning() << "query_service::CQuery::sendQuery: Unable to write data to " << ip << ":" << port;
	else
		ret = socket->waitForReadyRead(1000);
	return ret;
}

template<typename T> inline
T reader::read(QByteArray &array, quint32 &offset)
{
	byte_value<T> v;
	for (quint32 i = 0; i < sizeof(T); ++i){
		v.bytes[i] = array[offset + i];
	}
	offset += sizeof(T);
	return v.value;
}

template<> inline
QString reader::read(QByteArray &array, quint32 &offset)
{
	byte_value<uint> v;
	v.bytes[0] = array[offset++];
	v.bytes[1] = array[offset++];
	v.bytes[2] = array[offset++];
	v.bytes[3] = array[offset++];

	char *buf = new char[v.value + 1];
	for (quint32 i = 0; i < v.value; ++i)
		buf[i] = array[offset + i];
	buf[v.value] = 0;
	offset += v.value;
	QTextCodec *codec = QTextCodec::codecForName("cp1251");
	QString ret = codec->toUnicode(buf);
	delete[] buf;
	return ret;
}

QString reader::read_limited(QByteArray &array, quint32 &offset)
{
	quint32 len = static_cast<quint32>(array[offset++]);

	char *buf = new char[len + 1];
	for (quint32 i = 0; i < len; ++i)
		buf[i] = array[offset + i];
	buf[len] = 0;
	offset += len;
	QTextCodec *codec = QTextCodec::codecForName("cp1251");
	QString ret = codec->toUnicode(buf);
	delete[] buf;
	return ret;
}
