#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Topology CSE Networks Simulation");

int main(int argc, char *argv[])
{
  // Logging config
  LogComponentEnable("BulkSendApplication", LOG_LEVEL_INFO);
  LogComponentEnable("PacketSink", LOG_LEVEL_INFO);


  // Create and Connect Nodes
  NodeContainer naNe;
  naNe.Create(2);

  NodeContainer nbNd;
  nbNd.Create(2);

  NodeContainer ncNd;
  ncNd.Create(1);
  ncNd.Add(nbNd.Get(1));

  NodeContainer ndNe;
  ndNe.Add(nbNd.Get(1));
  ndNe.Add(naNe.Get(1));

  // Configure Connections
  PointToPointHelper pointToPointNaNe;
  pointToPointNaNe.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNaNe.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNaNe;
  p2pDevicesNaNe = pointToPointNaNe.Install(naNe);

  PointToPointHelper pointToPointNbNd;
  pointToPointNbNd.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNbNd.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNbNd;
  p2pDevicesNbNd = pointToPointNbNd.Install(nbNd);

  PointToPointHelper pointToPointNcNd;
  pointToPointNcNd.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNcNd.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNcNd;
  p2pDevicesNcNd = pointToPointNcNd.Install(ncNd);

  PointToPointHelper pointToPointNdNe;
  pointToPointNdNe.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNdNe.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNdNe;
  p2pDevicesNdNe = pointToPointNdNe.Install(ndNe);

  // Config source and dest addresses
  InternetStackHelper stack;
  stack.Install(naNe);
  stack.Install(nbNd);
  stack.Install(ncNd.Get(0));

  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNaNe;
  p2pInterfacesNaNe = address.Assign(p2pDevicesNaNe);

  address.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNbNd;
  p2pInterfacesNbNd = address.Assign(p2pDevicesNbNd);

  address.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNcNd;
  p2pInterfacesNcNd = address.Assign(p2pDevicesNcNd);

  address.SetBase("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNdNe;
  p2pInterfacesNdNe = address.Assign(p2pDevicesNdNe);


  // Sink deploy
  uint16_t port = 9;
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory",
      InetSocketAddress (Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = packetSinkHelper.Install(naNe.Get(1));
  sinkApps.Start(Seconds(0.));
  sinkApps.Stop(Seconds(15.));
  
  // Flow setting
  BulkSendHelper sourceA("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNaNe.GetAddress(1), port));
  sourceA.SetAttribute("MaxBytes", UintegerValue(2000));
  ApplicationContainer sourceAppsA = sourceA.Install(naNe.Get(0));
  sourceAppsA.Start(Seconds(0.));
  sourceAppsA.Stop(Seconds(15.));

  BulkSendHelper sourceB("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNaNe.GetAddress(1), port));
  sourceB.SetAttribute("MaxBytes", UintegerValue(3000));
  ApplicationContainer sourceAppsB = sourceB.Install(nbNd.Get(0));
  sourceAppsB.Start(Seconds(0.));
  sourceAppsB.Stop(Seconds(15.));

  // First connection path from node c
  BulkSendHelper sourceC1("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNaNe.GetAddress(1), port));
  sourceC1.SetAttribute("MaxBytes", UintegerValue(3000));
  ApplicationContainer sourceAppsC1 = sourceC1.Install(ncNd.Get(0));
  sourceAppsC1.Start(Seconds(3.));
  sourceAppsC1.Stop(Seconds(15.));

  // Second connection path from node c
  BulkSendHelper sourceC2("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNaNe.GetAddress(1), port));
  sourceC2.SetAttribute("MaxBytes", UintegerValue(3000));
  ApplicationContainer sourceAppsC2 = sourceC2.Install(ncNd.Get(0));
  sourceAppsC2.Start(Seconds(7.));
  sourceAppsC2.Stop(Seconds(12.));

  // Configure routing tables
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Run Simulation
  Simulator::Stop(Seconds(60));
  Simulator::Run();
  Simulator::Destroy();

  return 0;  
}
