#ifndef TORQUE_POSITIO_UDP_SIMULINK_H
#define TORQUE_POSITIO_UDP_SIMULINK_H

#include <AngleAndComsumerPlugin.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <chrono>
#include <ctime>
#include "OpenSimFileLogger.h"
#include <thread>
#include "ExecutionXmlReader.h"
#include "XMLInterpreter.h"
#include <mutex>
#include <GetTime.h>

#ifdef WIN32
class __declspec(dllexport) TorqueAndPositionUDPSimulink : public AngleAndComsumerPlugin {
#endif
#ifdef UNIX
	class  TorqueAndPositionUDPSimulink : public AngleAndComsumerPlugin {
#endif
	public:
		TorqueAndPositionUDPSimulink();
		~TorqueAndPositionUDPSimulink();

		void init(std::string& executableXMLFileName);

		void setDofName(const std::vector<std::string>& dofName)
		{
			dofName_ = dofName;
		}

		void setDofTorque(const std::vector<double>& dofTorque);

		void setMuscleForce(const std::vector<double>& muscleForce);
		void setMuscleFiberLength(const std::vector<double>& muscleFiberLength);
		void setMuscleFiberVelocity(const std::vector<double>& muscleFiberVelocity);

		void setDofStiffness(const std::vector<double>& dofStiffness)
		{

		}

		void setMuscleName(const std::vector<std::string>& muscleName)
		{

		}

		void setOutputTimeStamp(const double& timeStamp)
		{

		}


		void setMuscleForcePassive(const std::vector<double>& muscleForcePassive)
		{

		}

		void setMuscleForceActive(const std::vector<double>& muscleForceActive)
		{

		}

		void setMuscleTendonStrain(const std::vector<double>& tendonStrain)
		{

		}

		void setTendonStrain (const std::vector<double>& tendonStrain)
		{

		}

		const double& GetAngleTimeStamp();

		const std::vector<std::string>& GetDofName()
		{
			return dofName_;
		}

		const std::map<std::string, double>& GetDataMap();

		const std::map<std::string, double>& GetDataMapTorque();

		void stop();

		void setDirectory(std::string outDirectory, std::string inDirectory = std::string());

		void setVerbose(int verbose)
		{
		}

		void setRecord(bool record)
		{
			record_ = record;
		}

	protected:

		void positionFeed();


		void torqueIDFeed();

		std::thread* feederPositionThread; 
		std::thread* feederTorqueIDThread;

		std::map<std::string, double> dataAngle_; 
		std::vector< double> data_;
		std::vector< double> dataTorqueID_;
		double timeStamp_;
		double timeNow_;
		std::map<std::string, double> dataTorque_;
		OpenSimFileLogger<double>* logger_;
		std::mutex mtxTime_;
		std::mutex mtxData_;
		std::mutex loggerMutex_;
		std::mutex mtxDataTorqueID_;
		std::string outDirectory_;
		bool record_;
		std::vector<std::string> dofName_;
		bool threadStop_;
		bool newData_; 
		XMLInterpreter* _xmlInterpreter;

		std::string ip_;
		int port_;

		SOCKET positionSock_;
		SOCKET torqueIDSock_;
		SOCKET torqueSock_;
		SOCKET fiberLengthSock_;
		SOCKET fiberVelSock_;
		SOCKET muscleForceSock_;
		struct sockaddr_in torqueAddr_;
		struct sockaddr_in fiberVelAddr_;
		struct sockaddr_in fiberLengthAddr_;
		struct sockaddr_in muscleForceAddr_;
		int fromlen_;
};

#endif