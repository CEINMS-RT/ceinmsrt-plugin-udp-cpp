#include <Torque_Position_UDP_Simulink.h>
#include <cstdlib>
#include <csignal>
#include <windows.h>
#include <xercesc/util/PlatformUtils.hpp>
#include <GetTime.h>
#include <iomanip> 

	bool endMain;

	void SigintHandler(int sig)
	{
		endMain = true;;
	}

	int main(int argc, char *argv[])
	{
		endMain = false;
		signal(SIGINT, SigintHandler);

		xercesc::XMLPlatformUtils::Initialize();

		TorqueAndPositionUDPSimulink plugin;
		std::vector<std::string> dofName;
		dofName.push_back("r_ankle");

		std::cout << "start: " << argv[1] << std::endl;

		std::vector<double> torque, fiberLength, fiberVel, muscleForce;
		torque.push_back(2);
		fiberLength.push_back(3);
		fiberVel.push_back(4);
		muscleForce.push_back(5);

		plugin.setDofName(dofName);
		plugin.setDirectory("Test");
		plugin.setRecord(true);
		std::cout << "A" << std::endl;
		std::string filename = argv[1];
		plugin.init(filename);
		std::cout << "B" << std::endl;

		while (!endMain)
		{
			double timeInint = rtb::getTime();
			double time = plugin.GetAngleTimeStamp();
			std::map<std::string, double> temp = plugin.GetDataMap();
			std::cout << "Angle data: " << temp["r_ankle"] <<  std::endl;
			//timeInint = timeEMG::getTime();
			//std::map<std::string, double> tempT = plugin.GetDataMapTorque();
			//std::cout << "Time to get Data torque: " << std::setprecision(16) << timeEMG::getTime() - timeInint << " Torque data: " << tempT["ankle_angle_r"] << " : " << tempT["ankle_angle_l"] << std::endl;
			plugin.setDofTorque(torque);
			plugin.setMuscleFiberLength(fiberLength);
			plugin.setMuscleFiberVelocity(fiberVel);
			plugin.setMuscleForce(muscleForce);
			Sleep(100);
		}
		plugin.stop();

		return 0;
	}

