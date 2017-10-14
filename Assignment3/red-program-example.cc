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
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);
//WS Works	
	
//	std::cout <<"eq,"<< Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;

	enqueue.open("/home/will/Homework/Assignment3/p2c-enqueue.csv", std::ios::app);
	enqueue <<Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;
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

	dequeue.open("/home/will/Homework/Assignment3/p2c-dequeue.csv", std::ios::app);
	dequeue <<Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;
	dequeue.close();


	//TODO:  Need to figure out how to print the time this packet left and which flow it belongs to.  Hint below in app setup.
	//REMEMBER:  The sequence number is actually in bytes not packets so to make it graph nice you'll need to manipulate to get 
	//           nice consequtive sequence numbers to graph

}

void DroppedAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);


//	std::cout << "dr," <<Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;

	dropped.open("/home/will/Homework/Assignment3/p2c-dropped.csv", std::ios::app);
	dropped <<Simulator::Now().GetSeconds() <<","<<tcp.GetDestinationPort()-4999<<","<<tcp.GetSequenceNumber() << std::endl;
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

	enqueue.open("/home/will/Homework/Assignment3/p2c-enqueue.csv");
	enqueue << "Time,Source,Sequence\n";
	enqueue.close();

	dequeue.open("/home/will/Homework/Assignment3/p2c-dequeue.csv");
	dequeue << "Time,Source,Sequence\n";
	dequeue.close();

	dropped.open("/home/will/Homework/Assignment3/p2c-dropped.csv");
	dropped << "Time,Source,Sequence\n";
	dropped.close();

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

	std::string leafLinkBW = "10Mbps";
	std::string leafLinkDelay = "1ms";

	std::string bottleneckLinkBW = "1Mbps";
	std::string bottleneckLinkDelay = "50ms";

	std::string animationFile = "demo.xml";


	double stopTime = 10.0;

	double minTh = 50;
	double maxTh = 80;


	CommandLine cmd;
	cmd.AddValue("runNumber", "run number for random variable generation", runNumber);
	cmd.AddValue("numberOfLeaves", "number of leaves on each side of bottleneck", numberOfLeaves);
	cmd.AddValue("animationFile", "File name for animation output", animationFile);
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

	PointToPointDumbbellHelper d (	numberOfLeaves, pointToPointLeaf,	//specify how many (computer) nodes I want on the left.  specify the link from the computers to the left blue circle
					numberOfLeaves, pointToPointLeaf,	//specify how many (computer) nodes I want on the right.  specify the link from the computers to the right blue circle
					bottleneckLink);				//specify the link between the blue circles
	

	//this is for NetAnim
	d.BoundingBox(1, 1, 100, 100);

	InternetStackHelper stack;

	//Do the ones on the left side than right
	for(u32 i = 0; i < d.LeftCount(); ++i) {
		stack.Install(d.GetLeft(i));
	}
	for(u32 i = 0; i < d.RightCount(); ++i) {
		stack.Install(d.GetRight(i));
	}

	//setup red queue on bottleneck
	stack.Install(d.GetLeft());	//GetLeft() and GetRight() return one node.  It's the node on that side of the bottleneck
	stack.Install(d.GetRight());	
	TrafficControlHelper tchBottleneck;
	tchBottleneck.SetRootQueueDisc("ns3::RedQueueDisc");
 	//this `install` below returns a QueueDiscContainer.  Since I'm only installing one queue, I'm just going to grab the first one
	Ptr<QueueDisc> redQueue = (tchBottleneck.Install(d.GetLeft()->GetDevice(0))).Get(0);
	tchBottleneck.Install(d.GetRight()->GetDevice(0));
	
	//NOTE:  If I don't do the above with the RedQueueDisc, I'll end up with default queues on the bottleneck link.

	//setup traces
	redQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&EnqueueAtRed));
	redQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&DequeueAtRed));
	redQueue->TraceConnectWithoutContext("Drop", MakeCallback(&DroppedAtRed));

	//ASSIGN IP
	d.AssignIpv4Addresses (	Ipv4AddressHelper (	"10.1.1.0", "255.255.255.0"),
							Ipv4AddressHelper ( "10.2.1.0", "255.255.255.0"),
							Ipv4AddressHelper ( "10.3.1.0", "255.255.255.0"));

	//APPLICATION

	//Configure Sources
	ApplicationContainer sources;

	Address sourceLocalAddress(InetSocketAddress (Ipv4Address::GetAny(), port));
	

	//Install Sources
	OnOffHelper sourceHelper("ns3::TcpSocketFactory", Address());
	sourceHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	sourceHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	sourceHelper.SetAttribute("DataRate", DataRateValue(DataRate(leafLinkBW)));
	sourceHelper.SetAttribute("PacketSize", UintegerValue(packetSize));		

	//NOTE:  How can you make it so you can determine which flow of traffic a packet belongs to by only 
	//       examinining the TCP header which is what is available when the traces fire?  It's something that
	//		 you set when you setup your applications

	//

	for(u32 i = 0; i < d.LeftCount(); ++i) {
		//NOTE:  here I'm going to create one source on each leaf node and configure it to send to the corresponding leaf on the other side.
		//       pretend I did this... I actually did do it but I removed it to not give everything away
		AddressValue remoteAddress (InetSocketAddress (d.GetRightIpv4Address(i), port+i));
		sourceHelper.SetAttribute("Remote", remoteAddress);
		sources = sourceHelper.Install(d.GetLeft(i));
		sources.Add(sourceHelper.Install(d.GetLeft(i)));
		sources.Start(Seconds (1.0));
		sources.Stop(Seconds (stopTime));
	}


	//Configure Sinks

	//NOTE:  Keep in mind I could install multiple sinks on a single node.  
	//HINT:  That will probably be useful for the first experiment if you solve the problem the way I did

	ApplicationContainer sinks;


//Added WS

	//Install Sinks
	for(u32 i = 0; i < d.RightCount(); ++i) {
		//NOTE:  Here I'm going to create one sink on each leaf node on the right hand side
		Address sinkLocalAddress(InetSocketAddress (Ipv4Address::GetAny (), port+i));
		PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
		sinks = sinkHelper.Install(d.GetRight(i));
	}

	sources.Start(Seconds(1));
	sinks.Start(Seconds(0));

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	std::cout << "'Ere we go" << std::endl;

	AnimationInterface* anim = nullptr;
	FlowMonitorHelper flowMonHelper;
	Ptr<FlowMonitor> flowmon;

	if(createVis) {
		anim = new AnimationInterface(animationFile);
		anim->EnablePacketMetadata();
		anim->EnableIpv4L3ProtocolCounters(Seconds(0), Seconds(stopTime));
	}

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

	for(u32 i = 0; i < sinks.GetN(); ++i) {
		Ptr<Application> app = sinks.Get(i);
		Ptr<PacketSink> pktSink = DynamicCast<PacketSink>(app);
		u32 recieved = pktSink->GetTotalRx();
		std::cout << "\tSink\t" << i << "\tBytes\t" << recieved << std::endl;
		totalBytes += recieved;
	}

	std::cout << std::endl << "\tTotal\t\tBytes\t" << totalBytes << std::endl;

	std::cout << "Done" << std::endl;

	if(anim)
		delete anim;

	Simulator::Destroy();
	if (inputArgTest == 1) {
		std::cout << "Input Args are working" << std::endl;
	}
}


