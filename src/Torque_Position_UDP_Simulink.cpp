#include "Torque_Position_UDP_Simulink.h"

TorqueAndPositionUDPSimulink::TorqueAndPositionUDPSimulink() : record_(false), outDirectory_("output"), threadStop_(false), newData_(false)
{
	
}

TorqueAndPositionUDPSimulink::~TorqueAndPositionUDPSimulink()
{
	delete feederPositionThread;
	delete feederTorqueIDThread;
	delete _xmlInterpreter;
}


void TorqueAndPositionUDPSimulink::init(std::string& executableXMLFileName)
{
	ExecutionXmlReader xml(executableXMLFileName);

	_xmlInterpreter = new XMLInterpreter(xml.getAngleFile());
	_xmlInterpreter->readXML();

	port_ = _xmlInterpreter->getPort();
	ip_ = _xmlInterpreter->getIP();
	if (record_)
	{
		logger_ = new OpenSimFileLogger<double>(outDirectory_);
		logger_->addLog(Logger::IK, dofName_);
	}


	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//struct sockaddr_in local;
	torqueAddr_.sin_family = AF_INET;
	torqueAddr_.sin_port = htons(port_+1);
	torqueAddr_.sin_addr.s_addr = inet_addr(ip_.c_str());//INADDR_ANY;//

	torqueSock_ = socket(AF_INET, SOCK_DGRAM, 0);

	/*fiberVelAddr_.sin_family = AF_INET;
	fiberVelAddr_.sin_port = htons(port_+2);
	fiberVelAddr_.sin_addr.s_addr = inet_addr(ip_.c_str());//INADDR_ANY;//

	fiberLengthSock_ = socket(AF_INET, SOCK_DGRAM, 0);


	fiberLengthAddr_.sin_family = AF_INET;
	fiberLengthAddr_.sin_port = htons(port_+3);
	fiberLengthAddr_.sin_addr.s_addr = inet_addr(ip_.c_str());//INADDR_ANY;//

	fiberVelSock_ = socket(AF_INET, SOCK_DGRAM, 0);


	muscleForceAddr_.sin_family = AF_INET;
	muscleForceAddr_.sin_port = htons(port_+4);
	muscleForceAddr_.sin_addr.s_addr = inet_addr(ip_.c_str());//INADDR_ANY;//

	muscleForceSock_ = socket(AF_INET, SOCK_DGRAM, 0);*/
	//bind(torqueSock, (sockaddr*)&local, sizeof(local));

	/*getsockname(torqueSock, (SOCKADDR *)&local, (int *)sizeof(local));
	printf("Server: Receiving IP(s) used: %s\n", inet_ntoa(local.sin_addr));
	printf("Server: Receiving port used: %d\n", htons(local.sin_port));
	printf("Server: I\'m ready to receive a datagram...\n");*/

	feederPositionThread = new std::thread(&TorqueAndPositionUDPSimulink::positionFeed, this);
	feederTorqueIDThread = new std::thread(&TorqueAndPositionUDPSimulink::torqueIDFeed, this);

	/*char ReceiveBuf[1024];
	ZeroMemory(ReceiveBuf, sizeof(ReceiveBuf));*/


	/*if (recvfrom(torqueSock, ReceiveBuf, 1, 0, (sockaddr*)&torqueAddr_, &fromlen_) == SOCKET_ERROR)
	{
		std::cout << "error recev" << std::endl;
		closesocket(torqueSock);
		WSACleanup();
		exit(-1);
	}*/
}

void TorqueAndPositionUDPSimulink::setDofTorque(const std::vector<double>& dofTorque)
{
	std::vector<char> buffer;
	buffer.resize(sizeof(double) * dofTorque.size());
	memcpy(buffer.data(), dofTorque.data(), sizeof(double) * dofTorque.size());
	sendto(torqueSock_, buffer.data(), buffer.size(), 0, (sockaddr*)&torqueAddr_, sizeof(torqueAddr_));
}

void TorqueAndPositionUDPSimulink::setMuscleForce(const std::vector<double>& muscleForce)
{
	/*std::vector<char> buffer;
	buffer.resize(sizeof(double) * muscleForce.size());
	memcpy(buffer.data(), muscleForce.data(), sizeof(double) * muscleForce.size());
	sendto(muscleForceSock_, buffer.data(), buffer.size(), 0, (sockaddr*)&muscleForceAddr_, sizeof(muscleForceAddr_));*/
}

void TorqueAndPositionUDPSimulink::setMuscleFiberLength(const std::vector<double>& muscleFiberLength)
{
	/*std::vector<char> buffer;
	buffer.resize(sizeof(double) * muscleFiberLength.size());
	memcpy(buffer.data(), muscleFiberLength.data(), sizeof(double) * muscleFiberLength.size());
	sendto(fiberLengthSock_, buffer.data(), buffer.size(), 0, (sockaddr*)&fiberLengthAddr_, sizeof(fiberLengthAddr_));*/
}

void TorqueAndPositionUDPSimulink::setMuscleFiberVelocity(const std::vector<double>& muscleFiberVelocity)
{
	/*std::vector<char> buffer;
	buffer.resize(sizeof(double) * muscleFiberVelocity.size());
	memcpy(buffer.data(), muscleFiberVelocity.data(), sizeof(double) * muscleFiberVelocity.size());
	sendto(fiberVelSock_, buffer.data(), buffer.size(), 0, (sockaddr*)&fiberVelAddr_, sizeof(fiberVelAddr_));*/
}

const std::map<std::string, double>& TorqueAndPositionUDPSimulink::GetDataMap()
{
	std::vector<double> dataSafe;
	mtxData_.lock();
	dataSafe = data_;
	mtxData_.unlock();

	if (dataSafe.size() != 0)
		for (std::vector<std::string>::const_iterator it = dofName_.begin(); it != dofName_.end(); it++)
			dataAngle_[*it] = dataSafe[std::distance<std::vector<std::string>::const_iterator>(dofName_.begin(), it)];

	return dataAngle_;
}

const double& TorqueAndPositionUDPSimulink::GetAngleTimeStamp()
{
	mtxTime_.lock();
	timeNow_ = timeStamp_;
	mtxTime_.unlock();
	return timeNow_;
}

void TorqueAndPositionUDPSimulink::stop()
{
	threadStop_ = true;
	feederPositionThread->join();
	feederTorqueIDThread->join();
	if (record_)
	{
		logger_->stop();
		delete logger_;
	}

	closesocket(torqueSock_);
	closesocket(fiberLengthSock_);
	closesocket(fiberVelSock_);
	closesocket(muscleForceSock_);
	closesocket(torqueIDSock_);
	WSACleanup();
}

const std::map<std::string, double>& TorqueAndPositionUDPSimulink::GetDataMapTorque()
{
	
	std::vector<double> dataSafe;
	mtxDataTorqueID_.lock();
	dataSafe = dataTorqueID_;
	mtxDataTorqueID_.unlock();

	if (dataSafe.size() != 0)
		for (std::vector<std::string>::const_iterator it = dofName_.begin(); it != dofName_.end(); it++)
			dataTorque_[*it] = dataSafe[std::distance<std::vector<std::string>::const_iterator>(dofName_.begin(), it)];

	return dataTorque_;
}

void TorqueAndPositionUDPSimulink::torqueIDFeed()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	struct sockaddr_in serverInfo;
	int len = sizeof(serverInfo);
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(port_+2);
	serverInfo.sin_addr.s_addr = inet_addr(ip_.c_str());

	std::cout << "IP Torque ID: " << ip_ << " port Torque ID: " << port_ + 2 << std::endl;

	// Setup the hints address info structure
// which is passed to the getaddrinfo() function
	torqueIDSock_ = socket(AF_INET, SOCK_DGRAM, 0);

	char buffer[1];
	buffer[0] = dofName_.size();
	if (sendto(torqueIDSock_, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, len) == SOCKET_ERROR)
	{
		std::cout << "error: sendto" << std::endl;
		closesocket(torqueIDSock_);
		WSACleanup();
		exit(-1);
	}

	const int NBOFCHANNEL = dofName_.size();

	double timeInitCpy;

	while (!threadStop_)
	{
		std::vector<char> dataChar;
		std::vector<double> tempData;
		tempData.resize(NBOFCHANNEL);
		dataChar.resize(sizeof(double) * tempData.size());

		recvfrom(torqueIDSock_, dataChar.data(), NBOFCHANNEL * sizeof(double), 0, (SOCKADDR*)&serverInfo, &len);
		memcpy(tempData.data(), dataChar.data(), NBOFCHANNEL * sizeof(double));

		timeInitCpy = rtb::getTime();

		mtxDataTorqueID_.lock();
		dataTorqueID_ = tempData;
		mtxDataTorqueID_.unlock();

		if (record_)
		{
			loggerMutex_.lock();
			logger_->log(Logger::ID, timeInitCpy, tempData);
			loggerMutex_.unlock();
		}

	}
	closesocket(torqueIDSock_);
	WSACleanup();
}

void TorqueAndPositionUDPSimulink::positionFeed()
{

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	struct sockaddr_in serverInfo;
	int len = sizeof(serverInfo);
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(port_);
	serverInfo.sin_addr.s_addr = inet_addr(ip_.c_str());

	std::cout << "IP position: " << ip_ << " port position: " << port_ << std::endl;

	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	positionSock_ = socket(AF_INET, SOCK_DGRAM, 0);

	char buffer[1];
	buffer[0] = dofName_.size();
	if (sendto(positionSock_, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, len) == SOCKET_ERROR)
	{
		std::cout << "error: sendto" << std::endl;
		closesocket(positionSock_);
		WSACleanup();
		exit(-1);
	}

	

	const int NBOFCHANNEL = dofName_.size();

	double timeInitCpy;

	while (!threadStop_)
	{
		std::vector<char> dataChar;
		std::vector<double> tempData;
		tempData.resize(NBOFCHANNEL);
		dataChar.resize(sizeof(double) * tempData.size());

		recvfrom(positionSock_, dataChar.data(), NBOFCHANNEL * sizeof(double), 0, (SOCKADDR *)&serverInfo, &len);
		memcpy(tempData.data(), dataChar.data(), NBOFCHANNEL * sizeof(double));

		mtxTime_.lock();
		timeStamp_ = rtb::getTime();
		timeInitCpy = timeStamp_;
		mtxTime_.unlock();

		mtxData_.lock();
		data_ = tempData;
		mtxData_.unlock();

		if (record_)
		{
			loggerMutex_.lock();
			logger_->log(Logger::IK, timeInitCpy, tempData);
			loggerMutex_.unlock();
		}

	}
	closesocket(positionSock_);
	WSACleanup();
}

void TorqueAndPositionUDPSimulink::setDirectory(std::string outDirectory, std::string inDirectory)
{
	outDirectory_ = outDirectory;
}

#ifdef UNIX
extern "C" AngleAndComsumerPlugin* create() {
	return new TorqueAndPositionUDPSimulink;
}

extern "C" void destroy(AngleAndComsumerPlugin* p) {
	delete p;
}
#endif

#ifdef WIN32 // __declspec (dllexport) id important for dynamic loading
extern "C" __declspec (dllexport) AngleAndComsumerPlugin* __cdecl create() {
	return new TorqueAndPositionUDPSimulink;
}

extern "C" __declspec (dllexport) void __cdecl destroy(AngleAndComsumerPlugin* p) {
	delete p;
}
#endif