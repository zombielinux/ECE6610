#include <iostream>
#include <fstream>

#ifdef NS3_MPI
#include <mpi.h>
#endif

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

	std::cout << "Hello" << std::endl; 
  int rc;
  int threads = 1;

  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    std::cout << "Error starting MPI program. Terminating." << std::endl;
 		MPI_Abort(MPI_COMM_WORLD, rc);
  }
  if (rc == MPI_SUCCESS) {
    std::cout << "MPI! It VERKS" << std::endl;

  }
	CommandLine cmd;
  cmd.AddValue("threads", "Number of threads for simulation", threads);
  cmd.Parse(argc, argv);



}
