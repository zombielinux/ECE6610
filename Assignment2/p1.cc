/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

int 
main (int argc, char *argv[])
{
  SeedManager::SetSeed(1);

  uint32_t nSpokes = 8;
  double endTime = 60.0;
  std::string protocol = "TcpHybla";

  //Process the command line

  CommandLine cmd;
  cmd.AddValue("nSpokes", "Number of spokes to place in each star", nSpokes);
  cmd.AddValue("Protocol", "TCP Protocol to use: TcpNewReno, TcpHybla, TcpHighSpeed", protocol);
  cmd.Parse(argc, argv);

  if(protocol == "TcpNewReno") {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpNewReno::GetTypeId()));
  }
  else if(protocol == "TcpHybla") {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpHybla::GetTypeId()));
  }
  else if(protocol == "TcpHighSpeed") {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpHighSpeed::GetTypeId()));
  }

  std::cout << protocol << std::endl;
  std::cout << nSpokes << std::endl;


  //Use the given p2p attributes for each star
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));
  
  //Use the pointToPoint helper to create point to point stars
  PointToPointStarHelper star(nSpokes, pointToPoint);

  InternetStackHelper internet;
  star.InstallStack(internet);

  Ipv4AddressHelper address("172.16.1.0", "255.255.255.0");

  star.AssignIpv4Addresses(address);

  Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), 5000));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  ApplicationContainer sinkApps;
  for(uint32_t i = 0; i < nSpokes; ++i) {
  	sinkApps.Add(sinkHelper.Install(star.GetSpokeNode(i)));
  }

  sinkApps.Start(Seconds(1.0));

  BulkSendHelper bulkSender ("ns3::TcpSocketFactory", Address());
  bulkSender.SetAttribute("SendSize", UintegerValue(1024));

  ApplicationContainer sourceApps;
  for(uint32_t i = 0; i < nSpokes; ++i) 
  {
    uint32_t remoteNode = (i + 1) % nSpokes;
  	AddressValue remoteAddress(InetSocketAddress(star.GetSpokeIpv4Address(remoteNode), 5000));
  	bulkSender.SetAttribute("Remote", remoteAddress);
  	sourceApps.Add(bulkSender.Install(star.GetSpokeNode(i)));
  }

  sourceApps.Start(Seconds(2.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  Simulator::Stop(Seconds(60.0));

  Simulator::Run ();

  uint64_t totalRx = 0;

  for(uint32_t i = 0; i < nSpokes; ++i) {

  	Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(i));
  	uint32_t bytesReceived = sink->GetTotalRx();
  	totalRx += bytesReceived;
  	std::cout << "Sink " << i << "\tTotalRx: " << bytesReceived * 1e-6 * 8 << "Mb";
  	std::cout << "\tThroughput: " << (bytesReceived * 1e-6 * 8) / endTime << "Mbps" << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Totals\tTotalRx: " << totalRx * 1e-6 * 8 << "Mb";
  std::cout << "\tGoodput: " << (totalRx * 1e-6 * 8) / endTime << "Mbps" << std::endl;

  Simulator::Destroy ();
}
