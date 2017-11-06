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
  int inner = 8;
  int outer = 2;

//  int threads = 1;
  int rank, numtasks;
  double stopTime = 2.00;

//Hub to inner ring settings
  std::string hubInnerBW = "100Mbps";
  std::string hubInnerLat= "5ms";

//Inner Ring to Child settings
  std::string innerOuterBW = "100Mbps";
  std::string innerOuterLat = "8ms";

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
//DBG	  if (rc == MPI_SUCCESS) std::cout << "MPI! It VERKS" << std::endl;

	  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

//DBG	  std::cout << "My rank is : " << rank << " of:" << numtasks-1 << std::endl;


//Set up random number generator
  RngSeedManager::SetSeed ((rank+1)*41);
  Ptr<UniformRandomVariable> randint = CreateObject<UniformRandomVariable> ();
  int x =0;
  for (int j=0; j <16; j++){
 	 x =0;
 	 for (int i=0; i <7; i++)  x+= randint->GetInteger(); 
//DBG	 if (x <=4)  std::cout << "Rank: " << rank << " Randint "<< j << ": "<< x << std::endl;
//DBG	 else std::cout << "Node " << j << " doesn't get an address" << std::endl;
	}
//Create node containers
	  NodeContainer hubs;
	  hubs.Create(ncampuses);

	  NodeContainer hub0inners;
	  hub0inners.Create(inner);

	  NodeContainer hub1inners;
	  hub1inners.Create(inner);

	  NodeContainer hub2inners;
	  hub2inners.Create(inner);

	  NodeContainer hub3inners;
	  hub3inners.Create(inner);

	  NodeContainer hub0outers;
	  hub0outers.Create(inner*outer);

	  NodeContainer hub1outers;
	  hub1outers.Create(inner*outer);

	  NodeContainer hub2outers;
	  hub2outers.Create(inner*outer);

	  NodeContainer hub3outers;
	  hub3outers.Create(inner*outer);

   	  if (rank == 0){
		//Handle the hub to hub connections first
		  std::cout << "Rank " << rank << " handling hub0tohub1, hub0tohub2, hub0tohub3" << std::endl;
	  	  PointToPointHelper hub0tohub1;
		  hub0tohub1.SetDeviceAttribute("DataRate", StringValue(hubHubBW));
		  hub0tohub1.SetChannelAttribute("Delay", StringValue(hubHubLat));
		  NetDeviceContainer hub0tohub1container = hub0tohub1.Install(hubs.Get(0),hubs.Get(1));

		  PointToPointHelper hub0tohub2;
		  hub0tohub2.SetDeviceAttribute("DataRate", StringValue(hubHubBW));
		  hub0tohub2.SetChannelAttribute("Delay", StringValue(hubHubLat));
		  NetDeviceContainer hub0tohub2container = hub0tohub2.Install(hubs.Get(0),hubs.Get(2));

		  PointToPointHelper hub0tohub3;
		  hub0tohub3.SetDeviceAttribute("DataRate", StringValue(hubHubBW));
		  hub0tohub3.SetChannelAttribute("Delay", StringValue(hubHubLat));
		  NetDeviceContainer hub0tohub3container = hub0tohub3.Install(hubs.Get(0),hubs.Get(3));

		//Do the inner nodes on hub 0 exist
		  int hub0innersExist [8] = {};
		  for (int j = 0; j <8; j++){
			x=0;
			for (int i=0; i <7; i++) x+= randint->GetInteger();
			hub0innersExist [j]=x;
			}
		  for (int j =0; j<8;j++) hub0innersExist[j] = hub0innersExist[j];

		//Do the outer nodes on hub 0 exist
		  int hub0outersExist[16] = {};
		  for (int j =0; j<16; j++){
			x=0;
			for (int i=0; i<6; i++) x+= randint->GetInteger();
			hub0outersExist[j]=x;
		  }
		//Inner Ring of Hub 0
		//Inner ring 0
		  if (hub0innersExist[0] <=4) {
		  	PointToPointHelper hub0toinner0;
		  	hub0toinner0.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
		  	hub0toinner0.SetChannelAttribute("Delay", StringValue(hubInnerLat));
		  	NetDeviceContainer hub0toinner0container = hub0toinner0.Install(hubs.Get(0), hub0inners.Get(0));

			//Outers of Inner0
			  if (hub0outersExist[0] < 4){
				  PointToPointHelper hub0inner0toouter0;
				  hub0inner0toouter0.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner0toouter0.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner0toouter0container = hub0inner0toouter0.Install(hub0inners.Get(0), hub0outers.Get(0));
			  }
			  if (hub0outersExist[1] < 4){
				  PointToPointHelper hub0inner0toouter1;
				  hub0inner0toouter1.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner0toouter1.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner0toouter1container = hub0inner0toouter1.Install(hub0inners.Get(0), hub0outers.Get(1));
			  }
		  }
		//Inner ring 1
		  if (hub0innersExist[1] <=4) {
			  PointToPointHelper hub0toinner1;
			  hub0toinner1.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub0toinner1.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub0toinner1container = hub0toinner1.Install(hubs.Get(0), hub0inners.Get(1));
		//Outers of Inner1
			  if (hub0outersExist[2] < 4){
				  PointToPointHelper hub0inner1toouter2;
				  hub0inner1toouter2.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner1toouter2.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner1toouter2container = hub0inner1toouter2.Install(hub0inners.Get(1), hub0outers.Get(2));
			  }
			  if (hub0outersExist[3] < 4){
				  PointToPointHelper hub0inner1toouter3;
				  hub0inner1toouter3.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner1toouter3.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner1toouter3container = hub0inner1toouter3.Install(hub0inners.Get(1), hub0outers.Get(3));
			  }
		  }
		//Inner ring 2
		  if (hub0innersExist[2] <=4) {
			  PointToPointHelper hub0toinner2;
			  hub0toinner2.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub0toinner2.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub0toinner2container = hub0toinner2.Install(hubs.Get(0), hub0inners.Get(2));
		//Outers of Inner2
			  if (hub0outersExist[4] < 4){
				  PointToPointHelper hub0inner2toouter4;
				  hub0inner2toouter4.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner2toouter4.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner2toouter4container = hub0inner2toouter4.Install(hub0inners.Get(2), hub0outers.Get(4));
			  }
			  if (hub0outersExist[5] < 4){
				  PointToPointHelper hub0inner2toouter5;
				  hub0inner2toouter5.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner2toouter5.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner2toouter5container = hub0inner2toouter5.Install(hub0inners.Get(2), hub0outers.Get(5));
			  }
		  }
		//Inner ring 3
  		  if (hub0innersExist[3] <=4) {
			  PointToPointHelper hub0toinner3;
			  hub0toinner3.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub0toinner3.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub0toinner3container = hub0toinner3.Install(hubs.Get(0), hub0inners.Get(3));
		//Outers of Inner3
			  if (hub0outersExist[6] < 4){
				  PointToPointHelper hub0inner3toouter6;
				  hub0inner3toouter6.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner3toouter6.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner3toouter6container = hub0inner3toouter6.Install(hub0inners.Get(3), hub0outers.Get(6));
			  }
			  if (hub0outersExist[7] < 4){
				  PointToPointHelper hub0inner3toouter7;
				  hub0inner3toouter7.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner3toouter7.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner3toouter7container = hub0inner3toouter7.Install(hub0inners.Get(3), hub0outers.Get(7));
			  }
		  }
		//Inner ring 4
		  if (hub0innersExist[4] <=4) {
			  PointToPointHelper hub0toinner4;
			  hub0toinner4.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub0toinner4.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub0toinner4container = hub0toinner4.Install(hubs.Get(0), hub0inners.Get(4));
		//Outers of Inner4
			  if (hub0outersExist[8] < 4){
				  PointToPointHelper hub0inner4toouter8;
				  hub0inner4toouter8.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner4toouter8.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner4toouter8container = hub0inner4toouter8.Install(hub0inners.Get(4), hub0outers.Get(8));
			  }
			  if (hub0outersExist[9] < 4){
				  PointToPointHelper hub0inner4toouter9;
				  hub0inner4toouter9.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner4toouter9.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner4toouter9container = hub0inner4toouter9.Install(hub0inners.Get(4), hub0outers.Get(9));
			  }
		  }
		//Inner ring 5
		  if (hub0innersExist[5] <=4) {
			  PointToPointHelper hub0toinner5;
			  hub0toinner5.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub0toinner5.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub0toinner5container = hub0toinner5.Install(hubs.Get(0), hub0inners.Get(5));
		//Outers of Inner5
			  if (hub0outersExist[10] < 4){
				  PointToPointHelper hub0inner5toouter10;
				  hub0inner5toouter10.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner5toouter10.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner5toouter10container = hub0inner5toouter10.Install(hub0inners.Get(5), hub0outers.Get(10));
			  }
			  if (hub0outersExist[11] < 4){
				  PointToPointHelper hub0inner5toouter11;
				  hub0inner5toouter11.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner5toouter11.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner5toouter11container = hub0inner5toouter11.Install(hub0inners.Get(5), hub0outers.Get(11));
			  } 
		  }
		//Inner ring 6
		  if (hub0innersExist[6] <=4) {
			  PointToPointHelper hub0toinner6;
			  hub0toinner6.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub0toinner6.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub0toinner6container = hub0toinner6.Install(hubs.Get(0), hub0inners.Get(6));
		//Outers of Inner6
			  if (hub0outersExist[12] < 4){
				  PointToPointHelper hub0inner6toouter12;
				  hub0inner6toouter12.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner6toouter12.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner6toouter12container = hub0inner6toouter12.Install(hub0inners.Get(6), hub0outers.Get(12));
			  }
			  if (hub0outersExist[13] < 4){
				  PointToPointHelper hub0inner6toouter13;
				  hub0inner6toouter13.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner6toouter13.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner6toouter13container = hub0inner6toouter13.Install(hub0inners.Get(6), hub0outers.Get(13));
			  }
		  }
		//Inner ring 7
		  if (hub0innersExist[7] <=4) {
			  PointToPointHelper hub0toinner7;
			  hub0toinner7.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub0toinner7.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub0toinner7container = hub0toinner7.Install(hubs.Get(0), hub0inners.Get(7));
		//Outers of Inner7
			  if (hub0outersExist[14] < 4){
				  PointToPointHelper hub0inner7toouter14;
				  hub0inner7toouter14.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner7toouter14.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner7toouter14container = hub0inner7toouter14.Install(hub0inners.Get(7), hub0outers.Get(14));
			  }
			  if (hub0outersExist[15] < 4){
				  PointToPointHelper hub0inner7toouter15;
				  hub0inner7toouter15.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub0inner7toouter15.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub0inner7toouter15container = hub0inner7toouter15.Install(hub0inners.Get(7), hub0outers.Get(15));
			  }
		  }

		//Outer Ring of hub0

		}

	  if (((numtasks == 1) && (rank == 0)) || ((numtasks == 2) && (rank == 0)) || ((numtasks == 4) && (rank == 1))){
	 	 std::cout << "Rank " << rank << " handling hub1tohub2, hub1tohub3" << std::endl;
		 PointToPointHelper hub1tohub2;
		  hub1tohub2.SetDeviceAttribute("DataRate", StringValue(hubHubBW));
		  hub1tohub2.SetChannelAttribute("Delay", StringValue(hubHubLat));
		  NetDeviceContainer hub1tohub2container = hub1tohub2.Install(hubs.Get(1),hubs.Get(2));

		  PointToPointHelper hub1tohub3;
		  hub1tohub3.SetDeviceAttribute("DataRate", StringValue(hubHubBW));
		  hub1tohub3.SetChannelAttribute("Delay", StringValue(hubHubLat));
		  NetDeviceContainer hub1tohub3container = hub1tohub3.Install(hubs.Get(1),hubs.Get(3));

		//Do the inner nodes on hub 1 exist
  		  int hub1innersExist [8] = {};
  		  for (int j = 0; j <8; j++){
			x=0;
			for (int i=0; i <7; i++) x+= randint->GetInteger();
			hub1innersExist [j]=x;
			}
		  for (int j =0; j<8;j++) hub1innersExist[j] = hub1innersExist[j];

		//Do the outer nodes on hub 1 exist
		  int hub1outersExist[16] = {};
		  for (int j =0; j<16; j++){
			x=0;
			for (int i=0; i<6; i++) x+= randint->GetInteger();
			hub1outersExist[j]=x;
		  }

		//Inner ring of hub1
 		//Inner Ring of Hub 0
		//Inner ring 0
		  if (hub1innersExist[0] <=4) {
		  	PointToPointHelper hub1toinner0;
		  	hub1toinner0.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
		  	hub1toinner0.SetChannelAttribute("Delay", StringValue(hubInnerLat));
		  	NetDeviceContainer hub1toinner0container = hub1toinner0.Install(hubs.Get(0), hub1inners.Get(0));

			//Outers of Inner0
			  if (hub1outersExist[0] < 4){
				  PointToPointHelper hub1inner0toouter0;
				  hub1inner0toouter0.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner0toouter0.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner0toouter0container = hub1inner0toouter0.Install(hub1inners.Get(0), hub1outers.Get(0));
			  }
			  if (hub1outersExist[1] < 4){
				  PointToPointHelper hub1inner0toouter1;
				  hub1inner0toouter1.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner0toouter1.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner0toouter1container = hub1inner0toouter1.Install(hub1inners.Get(0), hub1outers.Get(1));
			  }
		  }
		//Inner ring 1
		  if (hub1innersExist[1] <=4) {
			  PointToPointHelper hub1toinner1;
			  hub1toinner1.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub1toinner1.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub1toinner1container = hub1toinner1.Install(hubs.Get(0), hub1inners.Get(1));
		//Outers of Inner1
			  if (hub1outersExist[2] < 4){
				  PointToPointHelper hub1inner1toouter2;
				  hub1inner1toouter2.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner1toouter2.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner1toouter2container = hub1inner1toouter2.Install(hub1inners.Get(1), hub1outers.Get(2));
			  }
			  if (hub1outersExist[3] < 4){
				  PointToPointHelper hub1inner1toouter3;
				  hub1inner1toouter3.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner1toouter3.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner1toouter3container = hub1inner1toouter3.Install(hub1inners.Get(1), hub1outers.Get(3));
			  }
		  }
		//Inner ring 2
		  if (hub1innersExist[2] <=4) {
			  PointToPointHelper hub1toinner2;
			  hub1toinner2.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub1toinner2.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub1toinner2container = hub1toinner2.Install(hubs.Get(0), hub1inners.Get(2));
		//Outers of Inner2
			  if (hub1outersExist[4] < 4){
				  PointToPointHelper hub1inner2toouter4;
				  hub1inner2toouter4.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner2toouter4.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner2toouter4container = hub1inner2toouter4.Install(hub1inners.Get(2), hub1outers.Get(4));
			  }
			  if (hub1outersExist[5] < 4){
				  PointToPointHelper hub1inner2toouter5;
				  hub1inner2toouter5.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner2toouter5.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner2toouter5container = hub1inner2toouter5.Install(hub1inners.Get(2), hub1outers.Get(5));
			  }
		  }
		//Inner ring 3
  		  if (hub1innersExist[3] <=4) {
			  PointToPointHelper hub1toinner3;
			  hub1toinner3.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub1toinner3.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub1toinner3container = hub1toinner3.Install(hubs.Get(0), hub1inners.Get(3));
		//Outers of Inner3
			  if (hub1outersExist[6] < 4){
				  PointToPointHelper hub1inner3toouter6;
				  hub1inner3toouter6.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner3toouter6.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner3toouter6container = hub1inner3toouter6.Install(hub1inners.Get(3), hub1outers.Get(6));
			  }
			  if (hub1outersExist[7] < 4){
				  PointToPointHelper hub1inner3toouter7;
				  hub1inner3toouter7.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner3toouter7.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner3toouter7container = hub1inner3toouter7.Install(hub1inners.Get(3), hub1outers.Get(7));
			  }
		  }
		//Inner ring 4
		  if (hub1innersExist[4] <=4) {
			  PointToPointHelper hub1toinner4;
			  hub1toinner4.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub1toinner4.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub1toinner4container = hub1toinner4.Install(hubs.Get(0), hub1inners.Get(4));
		//Outers of Inner4
			  if (hub1outersExist[8] < 4){
				  PointToPointHelper hub1inner4toouter8;
				  hub1inner4toouter8.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner4toouter8.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner4toouter8container = hub1inner4toouter8.Install(hub1inners.Get(4), hub1outers.Get(8));
			  }
			  if (hub1outersExist[9] < 4){
				  PointToPointHelper hub1inner4toouter9;
				  hub1inner4toouter9.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner4toouter9.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner4toouter9container = hub1inner4toouter9.Install(hub1inners.Get(4), hub1outers.Get(9));
			  }
		  }
		//Inner ring 5
		  if (hub1innersExist[5] <=4) {
			  PointToPointHelper hub1toinner5;
			  hub1toinner5.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub1toinner5.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub1toinner5container = hub1toinner5.Install(hubs.Get(0), hub1inners.Get(5));
		//Outers of Inner5
			  if (hub1outersExist[10] < 4){
				  PointToPointHelper hub1inner5toouter10;
				  hub1inner5toouter10.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner5toouter10.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner5toouter10container = hub1inner5toouter10.Install(hub1inners.Get(5), hub1outers.Get(10));
			  }
			  if (hub1outersExist[11] < 4){
				  PointToPointHelper hub1inner5toouter11;
				  hub1inner5toouter11.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner5toouter11.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner5toouter11container = hub1inner5toouter11.Install(hub1inners.Get(5), hub1outers.Get(11));
			  } 
		  }
		//Inner ring 6
		  if (hub1innersExist[6] <=4) {
			  PointToPointHelper hub1toinner6;
			  hub1toinner6.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub1toinner6.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub1toinner6container = hub1toinner6.Install(hubs.Get(0), hub1inners.Get(6));
		//Outers of Inner6
			  if (hub1outersExist[12] < 4){
				  PointToPointHelper hub1inner6toouter12;
				  hub1inner6toouter12.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner6toouter12.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner6toouter12container = hub1inner6toouter12.Install(hub1inners.Get(6), hub1outers.Get(12));
			  }
			  if (hub1outersExist[13] < 4){
				  PointToPointHelper hub1inner6toouter13;
				  hub1inner6toouter13.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner6toouter13.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner6toouter13container = hub1inner6toouter13.Install(hub1inners.Get(6), hub1outers.Get(13));
			  }
		  }
		//Inner ring 7
		  if (hub1innersExist[7] <=4) {
			  PointToPointHelper hub1toinner7;
			  hub1toinner7.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub1toinner7.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub1toinner7container = hub1toinner7.Install(hubs.Get(0), hub1inners.Get(7));
		//Outers of Inner7
			  if (hub1outersExist[14] < 4){
				  PointToPointHelper hub1inner7toouter14;
				  hub1inner7toouter14.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner7toouter14.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner7toouter14container = hub1inner7toouter14.Install(hub1inners.Get(7), hub1outers.Get(14));
			  }
			  if (hub1outersExist[15] < 4){
				  PointToPointHelper hub1inner7toouter15;
				  hub1inner7toouter15.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub1inner7toouter15.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub1inner7toouter15container = hub1inner7toouter15.Install(hub1inners.Get(7), hub1outers.Get(15));
			  }
		  }




		}
	  if (((numtasks == 1) && (rank == 0)) || ((numtasks == 2) && (rank == 1)) || ((numtasks == 4) && (rank == 2))){
		  std::cout << "Rank " << rank << " handling hub2tohub3" << std::endl;
		  PointToPointHelper hub2tohub3;
		  hub2tohub3.SetDeviceAttribute("DataRate", StringValue(hubHubBW));
	  	  hub2tohub3.SetChannelAttribute("Delay", StringValue(hubHubLat));
		  NetDeviceContainer hub2tohub3container = hub2tohub3.Install(hubs.Get(2),hubs.Get(3));

		//Do the inner nodes on hub 2 exist
		  int hub2innersExist [8] = {};
		  for (int j = 0; j <8; j++){
			x=0;
			for (int i=0; i <7; i++) x+= randint->GetInteger();
			hub2innersExist [j]=x;
			}
		  for (int j =0; j<8;j++) hub2innersExist[j] = hub2innersExist[j];

		//Do the outer nodes on hub 2 exist
		  int hub2outersExist[16] = {};
		  for (int j =0; j<16; j++){
			x=0;
			for (int i=0; i<6; i++) x+= randint->GetInteger();
			hub2outersExist[j]=x;
		  }


		//Inner ring of hub 2
				//Inner Ring of Hub 0
		//Inner ring 0
		  if (hub2innersExist[0] <=4) {
		  	PointToPointHelper hub2toinner0;
		  	hub2toinner0.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
		  	hub2toinner0.SetChannelAttribute("Delay", StringValue(hubInnerLat));
		  	NetDeviceContainer hub2toinner0container = hub2toinner0.Install(hubs.Get(0), hub2inners.Get(0));

			//Outers of Inner0
			  if (hub2outersExist[0] < 4){
				  PointToPointHelper hub2inner0toouter0;
				  hub2inner0toouter0.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner0toouter0.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner0toouter0container = hub2inner0toouter0.Install(hub2inners.Get(0), hub2outers.Get(0));
			  }
			  if (hub2outersExist[1] < 4){
				  PointToPointHelper hub2inner0toouter1;
				  hub2inner0toouter1.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner0toouter1.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner0toouter1container = hub2inner0toouter1.Install(hub2inners.Get(0), hub2outers.Get(1));
			  }
		  }
		//Inner ring 1
		  if (hub2innersExist[1] <=4) {
			  PointToPointHelper hub2toinner1;
			  hub2toinner1.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub2toinner1.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub2toinner1container = hub2toinner1.Install(hubs.Get(0), hub2inners.Get(1));
		//Outers of Inner1
			  if (hub2outersExist[2] < 4){
				  PointToPointHelper hub2inner1toouter2;
				  hub2inner1toouter2.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner1toouter2.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner1toouter2container = hub2inner1toouter2.Install(hub2inners.Get(1), hub2outers.Get(2));
			  }
			  if (hub2outersExist[3] < 4){
				  PointToPointHelper hub2inner1toouter3;
				  hub2inner1toouter3.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner1toouter3.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner1toouter3container = hub2inner1toouter3.Install(hub2inners.Get(1), hub2outers.Get(3));
			  }
		  }
		//Inner ring 2
		  if (hub2innersExist[2] <=4) {
			  PointToPointHelper hub2toinner2;
			  hub2toinner2.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub2toinner2.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub2toinner2container = hub2toinner2.Install(hubs.Get(0), hub2inners.Get(2));
		//Outers of Inner2
			  if (hub2outersExist[4] < 4){
				  PointToPointHelper hub2inner2toouter4;
				  hub2inner2toouter4.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner2toouter4.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner2toouter4container = hub2inner2toouter4.Install(hub2inners.Get(2), hub2outers.Get(4));
			  }
			  if (hub2outersExist[5] < 4){
				  PointToPointHelper hub2inner2toouter5;
				  hub2inner2toouter5.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner2toouter5.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner2toouter5container = hub2inner2toouter5.Install(hub2inners.Get(2), hub2outers.Get(5));
			  }
		  }
		//Inner ring 3
  		  if (hub2innersExist[3] <=4) {
			  PointToPointHelper hub2toinner3;
			  hub2toinner3.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub2toinner3.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub2toinner3container = hub2toinner3.Install(hubs.Get(0), hub2inners.Get(3));
		//Outers of Inner3
			  if (hub2outersExist[6] < 4){
				  PointToPointHelper hub2inner3toouter6;
				  hub2inner3toouter6.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner3toouter6.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner3toouter6container = hub2inner3toouter6.Install(hub2inners.Get(3), hub2outers.Get(6));
			  }
			  if (hub2outersExist[7] < 4){
				  PointToPointHelper hub2inner3toouter7;
				  hub2inner3toouter7.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner3toouter7.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner3toouter7container = hub2inner3toouter7.Install(hub2inners.Get(3), hub2outers.Get(7));
			  }
		  }
		//Inner ring 4
		  if (hub2innersExist[4] <=4) {
			  PointToPointHelper hub2toinner4;
			  hub2toinner4.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub2toinner4.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub2toinner4container = hub2toinner4.Install(hubs.Get(0), hub2inners.Get(4));
		//Outers of Inner4
			  if (hub2outersExist[8] < 4){
				  PointToPointHelper hub2inner4toouter8;
				  hub2inner4toouter8.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner4toouter8.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner4toouter8container = hub2inner4toouter8.Install(hub2inners.Get(4), hub2outers.Get(8));
			  }
			  if (hub2outersExist[9] < 4){
				  PointToPointHelper hub2inner4toouter9;
				  hub2inner4toouter9.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner4toouter9.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner4toouter9container = hub2inner4toouter9.Install(hub2inners.Get(4), hub2outers.Get(9));
			  }
		  }
		//Inner ring 5
		  if (hub2innersExist[5] <=4) {
			  PointToPointHelper hub2toinner5;
			  hub2toinner5.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub2toinner5.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub2toinner5container = hub2toinner5.Install(hubs.Get(0), hub2inners.Get(5));
		//Outers of Inner5
			  if (hub2outersExist[10] < 4){
				  PointToPointHelper hub2inner5toouter10;
				  hub2inner5toouter10.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner5toouter10.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner5toouter10container = hub2inner5toouter10.Install(hub2inners.Get(5), hub2outers.Get(10));
			  }
			  if (hub2outersExist[11] < 4){
				  PointToPointHelper hub2inner5toouter11;
				  hub2inner5toouter11.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner5toouter11.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner5toouter11container = hub2inner5toouter11.Install(hub2inners.Get(5), hub2outers.Get(11));
			  } 
		  }
		//Inner ring 6
		  if (hub2innersExist[6] <=4) {
			  PointToPointHelper hub2toinner6;
			  hub2toinner6.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub2toinner6.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub2toinner6container = hub2toinner6.Install(hubs.Get(0), hub2inners.Get(6));
		//Outers of Inner6
			  if (hub2outersExist[12] < 4){
				  PointToPointHelper hub2inner6toouter12;
				  hub2inner6toouter12.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner6toouter12.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner6toouter12container = hub2inner6toouter12.Install(hub2inners.Get(6), hub2outers.Get(12));
			  }
			  if (hub2outersExist[13] < 4){
				  PointToPointHelper hub2inner6toouter13;
				  hub2inner6toouter13.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner6toouter13.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner6toouter13container = hub2inner6toouter13.Install(hub2inners.Get(6), hub2outers.Get(13));
			  }
		  }
		//Inner ring 7
		  if (hub2innersExist[7] <=4) {
			  PointToPointHelper hub2toinner7;
			  hub2toinner7.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub2toinner7.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub2toinner7container = hub2toinner7.Install(hubs.Get(0), hub2inners.Get(7));
		//Outers of Inner7
			  if (hub2outersExist[14] < 4){
				  PointToPointHelper hub2inner7toouter14;
				  hub2inner7toouter14.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner7toouter14.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner7toouter14container = hub2inner7toouter14.Install(hub2inners.Get(7), hub2outers.Get(14));
			  }
			  if (hub2outersExist[15] < 4){
				  PointToPointHelper hub2inner7toouter15;
				  hub2inner7toouter15.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub2inner7toouter15.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub2inner7toouter15container = hub2inner7toouter15.Install(hub2inners.Get(7), hub2outers.Get(15));
			  }
		  }


  	  if (((numtasks == 1) && (rank == 0)) || ((numtasks == 2) && (rank == 1)) || ((numtasks == 4) && (rank == 3))){
		//Inner ring of hub3

		//Do the inner nodes on hub 3 exist
		  int hub3innersExist [8] = {};
		  for (int j = 0; j <8; j++){
			x=0;
			for (int i=0; i <7; i++) x+= randint->GetInteger();
			hub3innersExist [j]=x;
			}
		  for (int j =0; j<8;j++) hub3innersExist[j] = hub3innersExist[j];

		//Do the outer nodes on hub 3 exist
		  int hub3outersExist[16] = {};
		  for (int j =0; j<16; j++){
			x=0;
			for (int i=0; i<6; i++) x+= randint->GetInteger();
			hub3outersExist[j]=x;
		  }


		//Inner ring 0
		  if (hub3innersExist[0] <=4) {
		  	PointToPointHelper hub3toinner0;
		  	hub3toinner0.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
		  	hub3toinner0.SetChannelAttribute("Delay", StringValue(hubInnerLat));
		  	NetDeviceContainer hub3toinner0container = hub3toinner0.Install(hubs.Get(0), hub3inners.Get(0));

			//Outers of Inner0
			  if (hub3outersExist[0] < 4){
				  PointToPointHelper hub3inner0toouter0;
				  hub3inner0toouter0.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner0toouter0.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner0toouter0container = hub3inner0toouter0.Install(hub3inners.Get(0), hub3outers.Get(0));
			  }
			  if (hub3outersExist[1] < 4){
				  PointToPointHelper hub3inner0toouter1;
				  hub3inner0toouter1.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner0toouter1.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner0toouter1container = hub3inner0toouter1.Install(hub3inners.Get(0), hub3outers.Get(1));
			  }
		  }
		//Inner ring 1
		  if (hub3innersExist[1] <=4) {
			  PointToPointHelper hub3toinner1;
			  hub3toinner1.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub3toinner1.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub3toinner1container = hub3toinner1.Install(hubs.Get(0), hub3inners.Get(1));
		//Outers of Inner1
			  if (hub3outersExist[2] < 4){
				  PointToPointHelper hub3inner1toouter2;
				  hub3inner1toouter2.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner1toouter2.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner1toouter2container = hub3inner1toouter2.Install(hub3inners.Get(1), hub3outers.Get(2));
			  }
			  if (hub3outersExist[3] < 4){
				  PointToPointHelper hub3inner1toouter3;
				  hub3inner1toouter3.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner1toouter3.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner1toouter3container = hub3inner1toouter3.Install(hub3inners.Get(1), hub3outers.Get(3));
			  }
		  }
		//Inner ring 2
		  if (hub3innersExist[2] <=4) {
			  PointToPointHelper hub3toinner2;
			  hub3toinner2.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub3toinner2.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub3toinner2container = hub3toinner2.Install(hubs.Get(0), hub3inners.Get(2));
		//Outers of Inner2
			  if (hub3outersExist[4] < 4){
				  PointToPointHelper hub3inner2toouter4;
				  hub3inner2toouter4.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner2toouter4.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner2toouter4container = hub3inner2toouter4.Install(hub3inners.Get(2), hub3outers.Get(4));
			  }
			  if (hub3outersExist[5] < 4){
				  PointToPointHelper hub3inner2toouter5;
				  hub3inner2toouter5.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner2toouter5.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner2toouter5container = hub3inner2toouter5.Install(hub3inners.Get(2), hub3outers.Get(5));
			  }
		  }
		//Inner ring 3
  		  if (hub3innersExist[3] <=4) {
			  PointToPointHelper hub3toinner3;
			  hub3toinner3.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub3toinner3.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub3toinner3container = hub3toinner3.Install(hubs.Get(0), hub3inners.Get(3));
		//Outers of Inner3
			  if (hub3outersExist[6] < 4){
				  PointToPointHelper hub3inner3toouter6;
				  hub3inner3toouter6.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner3toouter6.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner3toouter6container = hub3inner3toouter6.Install(hub3inners.Get(3), hub3outers.Get(6));
			  }
			  if (hub3outersExist[7] < 4){
				  PointToPointHelper hub3inner3toouter7;
				  hub3inner3toouter7.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner3toouter7.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner3toouter7container = hub3inner3toouter7.Install(hub3inners.Get(3), hub3outers.Get(7));
			  }
		  }
		//Inner ring 4
		  if (hub3innersExist[4] <=4) {
			  PointToPointHelper hub3toinner4;
			  hub3toinner4.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub3toinner4.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub3toinner4container = hub3toinner4.Install(hubs.Get(0), hub3inners.Get(4));
		//Outers of Inner4
			  if (hub3outersExist[8] < 4){
				  PointToPointHelper hub3inner4toouter8;
				  hub3inner4toouter8.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner4toouter8.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner4toouter8container = hub3inner4toouter8.Install(hub3inners.Get(4), hub3outers.Get(8));
			  }
			  if (hub3outersExist[9] < 4){
				  PointToPointHelper hub3inner4toouter9;
				  hub3inner4toouter9.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner4toouter9.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner4toouter9container = hub3inner4toouter9.Install(hub3inners.Get(4), hub3outers.Get(9));
			  }
		  }
		//Inner ring 5
		  if (hub3innersExist[5] <=4) {
			  PointToPointHelper hub3toinner5;
			  hub3toinner5.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub3toinner5.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub3toinner5container = hub3toinner5.Install(hubs.Get(0), hub3inners.Get(5));
		//Outers of Inner5
			  if (hub3outersExist[10] < 4){
				  PointToPointHelper hub3inner5toouter10;
				  hub3inner5toouter10.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner5toouter10.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner5toouter10container = hub3inner5toouter10.Install(hub3inners.Get(5), hub3outers.Get(10));
			  }
			  if (hub3outersExist[11] < 4){
				  PointToPointHelper hub3inner5toouter11;
				  hub3inner5toouter11.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner5toouter11.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner5toouter11container = hub3inner5toouter11.Install(hub3inners.Get(5), hub3outers.Get(11));
			  } 
		  }
		//Inner ring 6
		  if (hub3innersExist[6] <=4) {
			  PointToPointHelper hub3toinner6;
			  hub3toinner6.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub3toinner6.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub3toinner6container = hub3toinner6.Install(hubs.Get(0), hub3inners.Get(6));
		//Outers of Inner6
			  if (hub3outersExist[12] < 4){
				  PointToPointHelper hub3inner6toouter12;
				  hub3inner6toouter12.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner6toouter12.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner6toouter12container = hub3inner6toouter12.Install(hub3inners.Get(6), hub3outers.Get(12));
			  }
			  if (hub3outersExist[13] < 4){
				  PointToPointHelper hub3inner6toouter13;
				  hub3inner6toouter13.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner6toouter13.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner6toouter13container = hub3inner6toouter13.Install(hub3inners.Get(6), hub3outers.Get(13));
			  }
		  }
		//Inner ring 7
		  if (hub3innersExist[7] <=4) {
			  PointToPointHelper hub3toinner7;
			  hub3toinner7.SetDeviceAttribute("DataRate", StringValue(hubInnerBW));
			  hub3toinner7.SetChannelAttribute("Delay", StringValue(hubInnerLat));
			  NetDeviceContainer hub3toinner7container = hub3toinner7.Install(hubs.Get(0), hub3inners.Get(7));
		//Outers of Inner7
			  if (hub3outersExist[14] < 4){
				  PointToPointHelper hub3inner7toouter14;
				  hub3inner7toouter14.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner7toouter14.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner7toouter14container = hub3inner7toouter14.Install(hub3inners.Get(7), hub3outers.Get(14));
			  }
			  if (hub3outersExist[15] < 4){
				  PointToPointHelper hub3inner7toouter15;
				  hub3inner7toouter15.SetDeviceAttribute("DataRate", StringValue(innerOuterBW));
				  hub3inner7toouter15.SetChannelAttribute("Delay", StringValue(innerOuterLat));
				  NetDeviceContainer hub3inner7toouter15container = hub3inner7toouter15.Install(hub3inners.Get(7), hub3outers.Get(15));
			  }
		  }
		}



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
