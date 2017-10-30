#include <iostream>

#ifdef NS3_MPI
#include <mpi.h>
#endif

#include "p2pCampusHelper.h"


int main(int argc, char* argv[]) {

	std::cout << "Hello" << std::endl; 
  int rc;

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
