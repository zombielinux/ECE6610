#include <iostream>
#include <fstream>

//#ifdef NS3_MPI
#include <mpi.h>
//#endif

#include "p2pCampusHelper.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/flow-monitor-module.h"


int main(int argc, char* argv[]) {

  //std::cout << "Hello" << std::endl; 
  int rc;
  int run = 0;
  int ncampuses = 4;
//  int threads = 1;
  int rank, numtasks;
  double stopTime = 2.00;

//Hub to inner ring settings
  std::string hubInnerBW = "100Mbps";
  std::string hubInnerLat= "5ms";

//Inner Ring to Child settings
  std::string innerChildBW = "100Mbps";
  std::string innerChildLat = "8ms";

//Backbone settings
  std::string hubHubBW = "1Gbps";
  std::string hubHubLat = "5ms";

//Other default values for ???
  std::string scanRate = "5ms";
  std::string scanPattern = "Uniform";
  std::string syncType = "Yawns";


  CommandLine cmd;
//  cmd.AddValue("threads", "Number of threads for simulation", threads);
  cmd.AddValue("ScanRate", "Rate to generate worm traffic (5, 10, 20) ms, default 5", scanRate);
  cmd.AddValue("ScanPattern", "Scanning Pattern (Uniform, Local, Sequential), default Uniform", scanPattern);
  cmd.AddValue("BackboneDelay", "ms delay for backbone links (lookahead), default 5ms", hubHubLat);
  cmd.AddValue("SyncType", "Conservative algorithm (Yawns,Null), default Yawns", syncType);
 // cmd.AddValue("inputArgsTest", "test if input arguments are working", inputArgTest);
  cmd.Parse(argc, argv);
  //std::cout << "Number of ranks: " << threads << std::endl;

  //Make sure strings are properly formatted. 
  if (scanRate.substr(scanRate.length() - 2) != "ms") scanRate+="ms";

  if (hubHubLat.substr(hubHubLat.length() - 2) != "ms") scanRate+="ms";

  //Make sure choices are valid
  if ((scanPattern != "Uniform") && (scanPattern != "Local") && (scanPattern != "Sequential")) 	{
	std::cout << "You've chosen an invalid ScanPattern" << std::endl;
	run++;
	}

  if ((syncType != "Yawns") && (syncType != "Null")) {
	std::cout << "You've chosen an invalid SyncType" << std::endl;
	run++;
	}

  std::cout << "ScanRate     : " << scanRate << std::endl;
  std::cout << "ScanPattern  : " << scanPattern << std::endl;
  std::cout << "BackboneDelay: " << hubHubLat << std::endl;
  std::cout << "SyncType     : " << syncType << std::endl;


  if (run == 0) {
	  rc = MPI_Init(&argc,&argv);
	  if (rc != MPI_SUCCESS) {
		std::cout << "Error starting MPI program. Terminating." << std::endl;
		MPI_Abort(MPI_COMM_WORLD, rc);
	  }
	  if (rc == MPI_SUCCESS) std::cout << "MPI! It VERKS" << std::endl;

	  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	  std::cout << "My rank is : " << rank << " of:" << numtasks-1 << std::endl;


	  for (int i=0; i < ncampuses/numtasks; i++) {
		std::cout << "I am rank " << rank << " and I am responsible for hub" << ncampuses*rank/numtasks+i << std::endl;
		//IT VERKS! WS 11-04-17

	  }
	  //Hub1
	  //Hub2
	  //Hub3
	  //Hub4



	  MPI_Finalize();

	  SeedManager::SetSeed(1);


	  Simulator::Stop(Seconds(stopTime));
	  Simulator::Run();

	  Simulator::Destroy();
	  std::cout << "Destroyed Simulator" << std::endl;
	}
}
