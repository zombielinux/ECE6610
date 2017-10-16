#include <iostream>
#include <fstream>
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

using namespace ns3;

// i hate typing _t so I'm making shortcuts
using i32 = int32_t;
using u32 = uint32_t;
//if you're not familiar with the above lines, it's like an oldschool typedef

//i can now do things like this:
//	i32 myInt = 5 

//instead of this: 
//	int32_t myInt = 5

// yes, i'm that lazy

std::stringstream filePlotQueue;
std::stringstream filePlotQueueAvg;

u32 checkTimes;
double avgQueueSize;
u32 port = 5000;


u32 stream1 = 0;
u32 stream2 = 0;
u32 stream3 = 0;
u32 stream4 = 0;

constexpr u32 packetSize = 1000 - 42;

std::ofstream enqueue;
std::ofstream dequeue;
std::ofstream dropped;
std::ofstream queuesize;

//REMEMBER:  This is not an exact replica of what you need to create.  It just shows something relatively similar.
//NOTE: What it is I do in this example may not be the only way to achieve what you're trying to do for this assignment

//Get information from TCP header
//Need Sqeuenc number
//Need time (one for arrival, one for departure)
//Enable packet metadata. 
//Easier to look at tcp header documentation. 	



void EnqueueAtRed(Ptr<const QueueItem> item) {

	TcpHeader tcp;
//	Ipv4Header tmp;
//	Ipv4Address src;
	Ptr<Packet> pkt = item->GetPacket();
//	pkt->RemoveHeader(tmp);
	pkt->PeekHeader(tcp);
	

//WS Works	

//	std::cout <<"eq,"<< Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;
//	std::cout << pkt << std::endl;
//	pkt -> print(std::cout);
//	std::cout << tmp.GetDestination() << " <-  " << tmp.GetSource() << std::endl;
// 	std::cout << (i32)tcp.GetLength() << std::endl;
	enqueue.open("/home/will/Homework/Assignment3/p2c.csv", std::ios::app);
	enqueue <<Simulator::Now().GetSeconds()-1 << ",";

	switch (tcp.GetSourcePort()-5000) {
		case 0 : enqueue << stream1++%90 << ",,,,,,," <<std::endl;
			break;
		case 1 : enqueue << "," << ((stream2++)%90)+100 << ",,,,,,"<<std::endl;
			break;
		case 2 : enqueue << ",," <<((stream3++)%90)+200 << ",,,,,"<<std::endl;
			break;
		case 3 : enqueue << ",,," << ((stream4++)%90)+300 << ",,,,"<<std::endl;
			break;
	}

//	<<","<<(tcp.GetSourcePort()-5000)*100<< std::endl;
//	std::cout <<Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()<<","<<tcp.GetSequenceNumber()<<","<<tcp.GetSourcePort()<<","<<(u32)tcp.GetLength() << std::endl;
//	std::cout <<Simulator::Now().GetSeconds() <<","<<tcp.GetSourcePort()<<","<<tcp.GetDestinationPort()<< std::endl;
	enqueue.close();

	//TODO:  Need to figure out how to print the time this packet arrived and which flow it belongs to.  Hint below in app setup.
	//REMEMBER:  The sequence number is actually in bytes not packets so to make it graph nice you'll need to manipulate to get 
	//           nice consequtive sequence numbers to graph

}

void DequeueAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);

//	std::cout << "dq," << Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;

//	dequeue.open("/home/will/Homework/Assignment3/p2c-dequeue-test.csv", std::ios::app);
//	dequeue <<Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;
//	dequeue.close();


	//TODO:  Need to figure out how to print the time this packet left and which flow it belongs to.  Hint below in app setup.
	//REMEMBER:  The sequence number is actually in bytes not packets so to make it graph nice you'll need to manipulate to get 
	//           nice consequtive sequence numbers to graph

}

void DroppedAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);


//	std::cout << "dr," <<Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;

	dropped.open("/home/will/Homework/Assignment3/p2c.csv", std::ios::app);
//	dropped <<Simulator::Now().GetSeconds() <<","<<tcp.GetSequenceNumber() << std::endl;
	dropped <<Simulator::Now().GetSeconds()-1 << ",";

	switch (tcp.GetSourcePort()-5000) {
		case 0 : dropped << ",,,,"<<stream1++%90 << ",,," <<std::endl;
			break;
		case 1 : dropped << ",,,,," << ((stream2++)%90)+100 << ",,"<<std::endl;
			break;
		case 2 : dropped << ",,,,,," <<((stream3++)%90)+200 << ","<<std::endl;
			break;
		case 3 : dropped << ",,,,,,," << ((stream4++)%90)+300 << ""<<std::endl;
			break;
	}


	dropped.close();


	//TODO:  Need to figure out how to print the time this packet was dropped and which flow it belongs to.  Hint below in app setup.
	//REMEMBER:  The sequence number is actually in bytes not packets so to make it graph nice you'll need to manipulate to get 
	//           nice consequtive sequence numbers to graph
}


//This code is fine for printing average and actual queue size
void CheckQueueSize(Ptr<QueueDisc> queue) {
	u32 qsize = StaticCast<RedQueueDisc>(queue)->GetQueueSize();
	avgQueueSize += qsize;
	checkTimes++;
	

	queuesize.open("/home/will/Homework/Assignment3/p2c-queuesize.csv", std::ios::app);
//	std::cout <<Simulator::Now().GetSeconds()<< "," << qsize << ","<< avgQueueSize << std::endl;
	queuesize <<Simulator::Now().GetSeconds()<< "," << qsize << ","<< avgQueueSize << std::endl;
	queuesize.close();

	Simulator::Schedule(Seconds(0.01), &CheckQueueSize, queue);
	std::ofstream fPlotQueue(filePlotQueue.str().c_str(), std::ios::out | std::ios::app);
	fPlotQueue << Simulator::Now().GetSeconds() << " " << qsize << std::endl;
	fPlotQueue.close();

	std::ofstream fPlotQueueAvg(filePlotQueueAvg.str().c_str(), std::ios::out | std::ios::app);
	fPlotQueueAvg << Simulator::Now().GetSeconds() << " " << avgQueueSize / checkTimes << std::endl;
	fPlotQueueAvg.close();
}


int main(int argc, char* argv[]) {

	ns3::PacketMetadata::Enable();

	enqueue.open("/home/will/Homework/Assignment3/p2c.csv");
	enqueue << "Time,Source,Sequence\n";
	enqueue.close();
/*
	dequeue.open("/home/will/Homework/Assignment3/p2c-dequeue-test.csv");
	dequeue << "Time,Source,Sequence\n";
	dequeue.close();

	dropped.open("/home/will/Homework/Assignment3/p2c-dropped-test.csv");
	dropped << "Time,Source,Sequence\n";
	dropped.close();
*/
	queuesize.open("/home/will/Homework/Assignment3/p2c-queuesize.csv");
	queuesize << "Time,CurrentSize,AverageSize\n";
	queuesize.close();



	std::string pathOut = ".";
	bool writeForPlot = true;
	bool createVis = false;
	bool useFlowMon = true;

	//allow these to be varied via command line
	u32 runNumber = 0;
	u32 numberOfLeaves = 10;
	u32 maxPackets = 100;
	u32 inputArgTest= 0;

	std::string leafLinkBW = "100Mbps";
	std::string leafLinkDelay = "3ms";

	std::string bottleneckLinkBW = "45Mbps";
	std::string bottleneckLinkDelay = "2ms";

//	std::string animationFile = "demo.xml";


	double stopTime = 2.00;

	double minTh = 50;
	double maxTh = 80;


	CommandLine cmd;
	cmd.AddValue("runNumber", "run number for random variable generation", runNumber);
	cmd.AddValue("numberOfLeaves", "number of leaves on each side of bottleneck", numberOfLeaves);
//	cmd.AddValue("animationFile", "File name for animation output", animationFile);
	cmd.AddValue("createVis", "<0/1> to create animation output", createVis);
	cmd.AddValue("writeForPlot", "<0/1> to write results for queue plot", writeForPlot);
	cmd.AddValue("useFlowMon", "<0/1> to use the flowmonitor", useFlowMon);
	cmd.AddValue("maxPackets", "Max packets allowed in the device queue", maxPackets);
	//added WS 10-9
	cmd.AddValue("inputArgsTest", "test if input arguments are working", inputArgTest);

	cmd.Parse(argc, argv);

	Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
	Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(packetSize));
	Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(1));

	//RED setup
	Config::SetDefault("ns3::RedQueueDisc::Mode", StringValue("QUEUE_MODE_PACKETS"));
	Config::SetDefault("ns3::RedQueueDisc::MeanPktSize", UintegerValue(packetSize));
  	Config::SetDefault ("ns3::RedQueueDisc::MinTh", DoubleValue (minTh));
  	Config::SetDefault ("ns3::RedQueueDisc::MaxTh", DoubleValue (maxTh));
  	Config::SetDefault ("ns3::RedQueueDisc::LinkBandwidth", StringValue (bottleneckLinkBW));
  	Config::SetDefault ("ns3::RedQueueDisc::LinkDelay", StringValue (bottleneckLinkDelay));
  	Config::SetDefault ("ns3::RedQueueDisc::QueueLimit", UintegerValue (maxPackets));
	Config::SetDefault ("ns3::RedQueueDisc::MeanPktSize", UintegerValue (packetSize));

	SeedManager::SetSeed(1);
	SeedManager::SetRun(runNumber);


	PointToPointHelper bottleneckLink;
	bottleneckLink.SetDeviceAttribute ("DataRate", StringValue(bottleneckLinkBW));
	bottleneckLink.SetChannelAttribute ("Delay", StringValue(bottleneckLinkDelay));

	PointToPointHelper pointToPointLeaf;
	pointToPointLeaf.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	pointToPointLeaf.SetChannelAttribute ("Delay", StringValue(leafLinkDelay));

	//here's a pic of the topology we're creating
	//https://ctieware.eng.monash.edu.au/twiki/bin/view/Simulation/DumbbellTopologyIPv4

	//The (Computers) nodes on the far left and far right are the leaves.  The two routers (blue circles) are also nodes in ns-3 and the link 
	//between the routers is the bottleneck.

//	PointToPointDumbbellHelper d (	numberOfLeaves, pointToPointLeaf,	//specify how many (computer) nodes I want on the left.  specify the link from the computers to the left blue circle
//					numberOfLeaves, pointToPointLeaf,	//specify how many (computer) nodes I want on the right.  specify the link from the computers to the right blue circle
//					bottleneckLink);				//specify the link between the blue circles

	NodeContainer nodes;
	nodes.Create(10);

	//Create the 1->A link
	PointToPointHelper onetoA;
	onetoA.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	onetoA.SetChannelAttribute ("Delay", StringValue(".5ms"));
	NetDeviceContainer onetoAContainer = onetoA.Install(nodes.Get(0),nodes.Get(4));

	//Create the 2->A link
	PointToPointHelper twotoA;
	twotoA.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	twotoA.SetChannelAttribute ("Delay", StringValue("1ms"));
	NetDeviceContainer twotoAContainer = twotoA.Install(nodes.Get(1),nodes.Get(4));

	//Create the 3->A link
	PointToPointHelper threetoA;
	threetoA.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	threetoA.SetChannelAttribute ("Delay", StringValue("2ms"));
	NetDeviceContainer threetoAContainer = threetoA.Install(nodes.Get(2),nodes.Get(4));

	//Create the 4->A link
	PointToPointHelper fourtoA;
	fourtoA.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	fourtoA.SetChannelAttribute ("Delay", StringValue("5ms"));
	NetDeviceContainer fourtoAContainer = fourtoA.Install(nodes.Get(3),nodes.Get(4));

	//Create the A->B Bottlneck
	PointToPointHelper AtoB;
	AtoB.SetDeviceAttribute ("DataRate", StringValue(bottleneckLinkBW));
	AtoB.SetChannelAttribute ("Delay", StringValue(bottleneckLinkDelay));
	NetDeviceContainer AtoBContainer = AtoB.Install(nodes.Get(4),nodes.Get(5));

	//Create the B->5 link
	PointToPointHelper BtoFive;
	BtoFive.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	BtoFive.SetChannelAttribute ("Delay", StringValue(".5ms"));
	NetDeviceContainer BtoFiveContainer = BtoFive.Install(nodes.Get(5),nodes.Get(6));

	//Create the B->6 link
	PointToPointHelper BtoSix;
	BtoSix.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	BtoSix.SetChannelAttribute ("Delay", StringValue("1ms"));
	NetDeviceContainer BtoSixContainer = BtoSix.Install(nodes.Get(5),nodes.Get(7));

	//Create the B->7 link
	PointToPointHelper BtoSeven;
	BtoSeven.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	BtoSeven.SetChannelAttribute ("Delay", StringValue("3ms"));
	NetDeviceContainer BtoSevenContainer = BtoSeven.Install(nodes.Get(5),nodes.Get(8));

	//Create the B->8 link
	PointToPointHelper BtoEight;
	BtoEight.SetDeviceAttribute ("DataRate", StringValue(leafLinkBW));
	BtoEight.SetChannelAttribute ("Delay", StringValue("5ms"));
	NetDeviceContainer BtoEightContainer = BtoEight.Install(nodes.Get(5),nodes.Get(9));



//	PointToPointStarHelper left ( numberOfLeaves , pointToPointLeaf);


//	PointToPointStarHelper right ( numberOfLeaves , pointToPointLeaf);


	//this is for NetAnim
//	d.BoundingBox(1, 1, 100, 100);

	InternetStackHelper stack;

	//Do the ones on the left side than right
	for(u32 i=0; i < nodes.GetN(); i++) {
		stack.Install(nodes.Get(i));
	}

//	for(u32 i = 0; i < d.LeftCount(); ++i) {
//		stack.Install(d.GetLeft(i));
//	}
//	for(u32 i = 0; i < d.RightCount(); ++i) {
//		stack.Install(d.GetRight(i));
//	}

	//setup red queue on bottleneck
	//This just installs on the two middle nodes of the dumbell. 
//	stack.Install(d.GetLeft());	//GetLeft() and GetRight() return one node.  It's the node on that side of the bottleneck
//	stack.Install(d.GetRight());	

	TrafficControlHelper tchBottleneck;
	tchBottleneck.SetRootQueueDisc("ns3::RedQueueDisc");
	tchBottleneck.Install(nodes.Get(4)->GetDevice(0));

 	//this `install` below returns a QueueDiscContainer.  Since I'm only installing one queue, 
//	I'm just going to grab the first one
//	Ptr<QueueDisc> redQueue = (tchBottleneck.Install(d.GetLeft()->GetDevice(0))).Get(0);
//	tchBottleneck.Install(d.GetRight()->GetDevice(0));

	//Install red queue on node B
//	Ptr<QueueDisc> redQueue = (tchBottleneck.Install(nodes.Get(4)->GetDevice(0))).Get(0);
//	tchBottleneck.Install(nodes.Get(5)->GetDevice(0));


	Ptr<QueueDisc> redQueue = (tchBottleneck.Install(AtoBContainer.Get(1))).Get(0);

	//NOTE:  If I don't do the above with the RedQueueDisc, I'll end up with default queues on the bottleneck link.

	//setup traces
	redQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&EnqueueAtRed));
	redQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&DequeueAtRed));
	redQueue->TraceConnectWithoutContext("Drop", MakeCallback(&DroppedAtRed));

	//ASSIGN IP
//	d.AssignIpv4Addresses (	Ipv4AddressHelper (	"10.1.1.0", "255.255.255.0"),
//							Ipv4AddressHelper ( "10.2.1.0", "255.255.255.0"),
//							Ipv4AddressHelper ( "10.3.1.0", "255.255.255.0"));

//Assign IPs to nodes
	Ipv4AddressHelper ipv4nodes;
	ipv4nodes.SetBase ("10.10.1.0", "255.255.255.0");
	Ipv4InterfaceContainer onetoAip = ipv4nodes.Assign(onetoAContainer);

	ipv4nodes.SetBase ("10.10.2.0", "255.255.255.0");
	Ipv4InterfaceContainer twotoAip = ipv4nodes.Assign(twotoAContainer);

	ipv4nodes.SetBase ("10.10.3.0", "255.255.255.0");
	Ipv4InterfaceContainer threetoAip = ipv4nodes.Assign(threetoAContainer);

	ipv4nodes.SetBase ("10.10.4.0", "255.255.255.0");
	Ipv4InterfaceContainer fourtoAip = ipv4nodes.Assign(fourtoAContainer);

	ipv4nodes.SetBase ("10.10.5.0", "255.255.255.0");
	Ipv4InterfaceContainer AtoBip = ipv4nodes.Assign(AtoBContainer);

	ipv4nodes.SetBase ("10.10.6.0", "255.255.255.0");
	Ipv4InterfaceContainer BtoFiveip = ipv4nodes.Assign(BtoFiveContainer);

//	ipv4nodes.NewNetwork();
//	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	ipv4nodes.SetBase ("10.10.7.0", "255.255.255.0");
	Ipv4InterfaceContainer BtoSixip = ipv4nodes.Assign(BtoSixContainer);

	ipv4nodes.SetBase ("10.10.8.0", "255.255.255.0");
	Ipv4InterfaceContainer BtoSevenip = ipv4nodes.Assign(BtoSevenContainer);

	ipv4nodes.SetBase ("10.10.9.0", "255.255.255.0");
	Ipv4InterfaceContainer BtoEightip = ipv4nodes.Assign(BtoEightContainer);


	
	//APPLICATION

	//Configure Sources
//	ApplicationContainer sources;
//
//	Address sourceLocalAddress(InetSocketAddress (Ipv4Address::GetAny(), port));


	//Install Sources

	//NOTE:  How can you make it so you can determine which flow of traffic a packet belongs to by only 
	//       examinining the TCP header which is what is available when the traces fire?  It's something that
	//		 you set when you setup your applications
/*	OnOffHelper sourceHelper("ns3::TcpSocketFactory", Address());
	sourceHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	sourceHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	sourceHelper.SetAttribute("DataRate", DataRateValue(DataRate(leafLinkBW)));
	sourceHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
*/
	//

//	AddressValue remoteAddress (InetSocketAddres//	std::cout <<BtoEightip.GetIpv4Address(0) <<"   " <<BtoEightip.GetIpv4Address(1) << std::endl;
	//1 to 5 source creation

	ApplicationContainer sourceOnetoA;
//	OnOffHelper sourceHelperOnetoA("ns3::TcpSocketFactory", InetSocketAddress (onetoAip.GetAddress(0), 4000));
	OnOffHelper sourceHelperOnetoA("ns3::TcpSocketFactory", Address());
	sourceHelperOnetoA.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	sourceHelperOnetoA.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	sourceHelperOnetoA.SetAttribute("DataRate", DataRateValue(DataRate(leafLinkBW)));
	sourceHelperOnetoA.SetAttribute("PacketSize", UintegerValue(packetSize));
	AddressValue remoteAddressOnetoA (InetSocketAddress (BtoFiveip.GetAddress(1), port));
	sourceHelperOnetoA.SetAttribute("Remote", remoteAddressOnetoA);
	sourceOnetoA = sourceHelperOnetoA.Install(nodes.Get(0));
	sourceOnetoA.Stop(Seconds (stopTime));

	//1 to 5 source creation
	ApplicationContainer sourceTwotoSix;
	OnOffHelper sourceHelperTwotoSix("ns3::TcpSocketFactory", InetSocketAddress (onetoAip.GetAddress(0), 10000));
	sourceHelperTwotoSix.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	sourceHelperTwotoSix.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	sourceHelperTwotoSix.SetAttribute("DataRate", DataRateValue(DataRate(leafLinkBW)));
	sourceHelperTwotoSix.SetAttribute("PacketSize", UintegerValue(packetSize+100000));

//	AddressValue remoteAddressTwotoSix (InetSocketAddress (BtoFiveip.GetAddress(1), port));
	AddressValue remoteAddressTwotoSix (InetSocketAddress (BtoSixip.GetAddress(1), port));
	sourceHelperTwotoSix.SetAttribute("Remote", remoteAddressTwotoSix);
	sourceTwotoSix = sourceHelperTwotoSix.Install(nodes.Get(1));
	sourceTwotoSix.Start(Seconds (1.0));
	sourceTwotoSix.Stop(Seconds (stopTime));

	//1 to 5 source creation
	ApplicationContainer sourceThreetoSeven;
	OnOffHelper sourceHelperThreetoSeven("ns3::TcpSocketFactory", InetSocketAddress (onetoAip.GetAddress(0), 10000));
	sourceHelperThreetoSeven.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	sourceHelperThreetoSeven.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	sourceHelperThreetoSeven.SetAttribute("DataRate", DataRateValue(DataRate(leafLinkBW)));
	sourceHelperThreetoSeven.SetAttribute("PacketSize", UintegerValue(packetSize));

//	AddressValue remoteAddressThreetoSeven (InetSocketAddress (BtoFiveip.GetAddress(1), port));
	AddressValue remoteAddressThreetoSeven (InetSocketAddress (BtoSevenip.GetAddress(1), port+1));
	sourceHelperThreetoSeven.SetAttribute("Remote", remoteAddressThreetoSeven);
	sourceThreetoSeven = sourceHelperThreetoSeven.Install(nodes.Get(2));
	sourceThreetoSeven.Start(Seconds (1.0));
	sourceThreetoSeven.Stop(Seconds (stopTime));

	//1 to 5 source creation
	ApplicationContainer sourceFourtoEight;
	OnOffHelper sourceHelperFourtoEight("ns3::TcpSocketFactory", InetSocketAddress (onetoAip.GetAddress(0), 10000));
	sourceHelperFourtoEight.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	sourceHelperFourtoEight.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	sourceHelperFourtoEight.SetAttribute("DataRate", DataRateValue(DataRate(leafLinkBW)));
	sourceHelperFourtoEight.SetAttribute("PacketSize", UintegerValue(packetSize));
//	AddressValue remoteAddressFourtoEight (InetSocketAddress (BtoFiveip.GetAddress(1), port));
	AddressValue remoteAddressFourtoEight (InetSocketAddress (BtoEightip.GetAddress(1), port+3));
	sourceHelperFourtoEight.SetAttribute("Remote", remoteAddressFourtoEight);
	sourceFourtoEight = sourceHelperFourtoEight.Install(nodes.Get(3));
	sourceFourtoEight.Start(Seconds (1.0));
	sourceFourtoEight.Stop(Seconds (stopTime));


/*
	std::cout << onetoAip.GetAddress(0)<< "  " << onetoAip.Get(0).first << std::endl;
	std::cout << twotoAip.GetAddress(0)<< "  " << twotoAip.Get(0).first << std::endl;
	std::cout << threetoAip.GetAddress(0)<< "  " << threetoAip.Get(0).first << std::endl;
	std::cout << fourtoAip.GetAddress(0)<< "  " << fourtoAip.Get(0).first << std::endl;

	std::cout << AtoBip.GetAddress(1) << "  " << AtoBip.Get(0).first << std::endl;
	std::cout << AtoBip.GetAddress(0) << "  " << AtoBip.Get(1).first << std::endl;

	std::cout << BtoFiveip.GetAddress(1)<< "  " << BtoFiveip.Get(1).first << std::endl;
	std::cout << BtoSixip.GetAddress(1)<< "  " << BtoSixip.Get(1).first << std::endl;
	std::cout << BtoSevenip.GetAddress(1)<< "  " << BtoSevenip.Get(1).first << std::endl;
	std::cout << BtoEightip.GetAddress(1)<< "  " << BtoEightip.Get(1).first << std::endl;
*/
/*
	for(u32 i = 0; i < 4; ++i) {
		//NOTE:  here I'm going to create one source on each leaf node and configure it to send to the corresponding leaf on the other side.
		//       pretend I did this... I actually did do it but I removed it to not give everything away
		AddressValue remoteAddress (InetSocketAddress (d.GetRightIpv4Address(i), port+i));
		sourceHelper.SetAttribute("Remote", remoteAddress);
		sources = sourceHelper.Install(d.GetLeft(i));
		sources.Add(sourceHelper.Install(d.GetLeft(i)));
		sources.Start(Seconds (1.0));
		sources.Stop(Seconds (stopTime));
	}
*/

	//Configure Sinks

	//NOTE:  Keep in mind I could install multiple sinks on a single node.  
	//HINT:  That will probably be useful for the first experiment if you solve the problem the way I did

//	ApplicationContainer sinks;

	ApplicationContainer sinkFive;
	Address sinkLocalAddressFive(InetSocketAddress (BtoFiveip.GetAddress(1), port));
	PacketSinkHelper sinkHelperFive ("ns3::TcpSocketFactory", sinkLocalAddressFive);
	sinkFive = sinkHelperFive.Install(nodes.Get(6));

	ApplicationContainer sinkFiveTwo;
	Address sinkLocalAddressFiveTwo(InetSocketAddress (BtoSixip.GetAddress(1), port+1));
	PacketSinkHelper sinkHelperFiveTwo ("ns3::TcpSocketFactory", sinkLocalAddressFive);
	sinkFiveTwo = sinkHelperFiveTwo.Install(nodes.Get(7));

	ApplicationContainer sinkFiveThree;
	Address sinkLocalAddressFiveThree(InetSocketAddress (BtoSevenip.GetAddress(1), port+2));
	PacketSinkHelper sinkHelperFiveThree ("ns3::TcpSocketFactory", sinkLocalAddressFiveThree);
	sinkFiveThree = sinkHelperFiveThree.Install(nodes.Get(8));

	ApplicationContainer sinkFiveFour;
	Address sinkLocalAddressFiveFour(InetSocketAddress (BtoEightip.GetAddress(1), port+3));
	PacketSinkHelper sinkHelperFiveFour ("ns3::TcpSocketFactory", sinkLocalAddressFiveFour);
	sinkFiveFour = sinkHelperFiveFour.Install(nodes.Get(9));

/*	ApplicationContainer sinkFive;
	Address sinkLocalAddressFive(InetSocketAddress (BtoFiveip.GetAddress(1), port));
	PacketSinkHelper sinkHelperFive ("ns3::TcpSocketFactory", sinkLocalAddressFive);
	sinkFive = sinkHelperFive.Install(nodes.Get(6));
	sinkFive.Start(Seconds(0));

	ApplicationContainer sinkSix;
	Address sinkLocalAddressSix(InetSocketAddress (BtoSixip.GetAddress(1), port+1));
	PacketSinkHelper sinkHelperSix ("ns3::TcpSocketFactory", sinkLocalAddressSix);
	sinkSix.Start(Seconds(0));

	ApplicationContainer sinkSeven;
	Address sinkLocalAddressSeven(InetSocketAddress (BtoSevenip.GetAddress(1), port+2));
	PacketSinkHelper sinkHelperSeven ("ns3::TcpSocketFactory", sinkLocalAddressSeven);
	sinkSeven.Start(Seconds(0));

	ApplicationContainer sinkEight;
	Address sinkLocalAddressEight(InetSocketAddress (BtoEightip.GetAddress(1), port+3));
	PacketSinkHelper sinkHelperEight ("ns3::TcpSocketFactory", sinkLocalAddressEight);
	sinkEight.Start(Seconds(0));
*/
//Added WS

	//Install Sinks
/*	for(u32 i = 0; i < d.RightCount(); ++i) {
		//NOTE:  Here I'm going to create one sink on each leaf node on the right hand side
		Address sinkLocalAddress(InetSocketAddress (Ipv4Address::GetAny (), port+i));
		PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
		sinks = sinkHelper.Install(d.GetRight(i));
	}
*/
//	sources.Start(Seconds(1));
//	sinks.Start(Seconds(0));

	sourceOnetoA.Start(Seconds (1.0));
	sourceTwotoSix.Start(Seconds (1.0));
	sourceThreetoSeven.Start(Seconds (1.0));
	sourceFourtoEight.Start(Seconds (1.0));
	sinkFive.Start(Seconds(0));
	sinkFiveTwo.Start(Seconds(0));
	sinkFiveThree.Start(Seconds(0));
	sinkFiveFour.Start(Seconds(0));


	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	std::cout << "'Ere we go" << std::endl;

	//AnimationInterface* anim = nullptr;
	FlowMonitorHelper flowMonHelper;
	Ptr<FlowMonitor> flowmon;

/*	if(createVis) {
		anim = new AnimationInterface(animationFile);
		anim->EnablePacketMetadata();
		anim->EnableIpv4L3ProtocolCounters(Seconds(0), Seconds(stopTime));
	}
*/
	if(writeForPlot) {
		filePlotQueue << pathOut << "/" << "redQueue.plot";
		filePlotQueueAvg << pathOut << "/" << "redQueueAvg.plot";

		remove(filePlotQueue.str().c_str());
		remove(filePlotQueueAvg.str().c_str());
		Simulator::ScheduleNow(&CheckQueueSize, redQueue);
	}

	if(useFlowMon) {
		flowmon = flowMonHelper.InstallAll();
	}

	Simulator::Stop(Seconds(stopTime));
	Simulator::Run();

	u32 totalBytes = 0;

	if(useFlowMon) {
		std::stringstream flowOut;
		flowOut << pathOut << "/" << "red.flowmon";
		remove(flowOut.str().c_str());
		flowmon->SerializeToXmlFile(flowOut.str().c_str(), true, true);
	}

//	std::cout << sinkFive.GetN() << std::endl;
//	std::cout << sourceThreetoSeven.GetN() << std::endl;
	for(u32 i = 0; i < sinkFive.GetN(); ++i) {
		Ptr<Application> app = sinkFive.Get(i);
		Ptr<PacketSink> pktSink = DynamicCast<PacketSink>(app);
		u32 recieved = pktSink->GetTotalRx();
		std::cout << "\tSink\t" << i << "\tBytes\t" << recieved << std::endl;
		totalBytes += recieved;
	}

	std::cout << std::endl << "\tTotal\t\tBytes\t" << totalBytes << std::endl;

	std::cout << "Done" << std::endl;

//	if(anim)
//		delete anim;

	Simulator::Destroy();
	if (inputArgTest == 1) {
		std::cout << "Input Args are working" << std::endl;
	}
}


