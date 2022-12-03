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

  // Nodes
  NodeContainer na;
  na.Create(1);
  NodeContainer nb;
  nb.Create(1);
  NodeContainer nc;
  nc.Create(1);
  NodeContainer nd;
  nd.Create(1);
  NodeContainer ne;
  ne.Create(1);

  // Connect Nodes
  NodeContainer naNe;
  naNe.Add(na.Get(0));
  naNe.Add(ne.Get(0));
  NodeContainer nbNd;
  nbNd.Add(nb.Get(0));
  nbNd.Add(nd.Get(0));
  NodeContainer ncNd;
  ncNd.Add(nc.Get(0));
  ncNd.Add(nd.Get(0));
  NodeContainer ndNe;
  ndNe.Add(nd.Get(0));
  ndNe.Add(ne.Get(0));

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

  Ipv4AddressHelper addressNaNe;
  addressNaNe.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNaNe;
  p2pInterfacesNaNe = addressNaNe.Assign(p2pDevicesNaNe);

  Ipv4AddressHelper addressNbNd;
  addressNbNd.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNbNd;
  p2pInterfacesNbNd = addressNbNd.Assign(p2pDevicesNbNd);

  Ipv4AddressHelper addressNcNd;
  addressNcNd.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNcNd;
  p2pInterfacesNcNd = addressNcNd.Assign(p2pDevicesNcNd);

  Ipv4AddressHelper addressNdNe;
  addressNdNe.SetBase("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfacesNdNe;
  p2pInterfacesNdNe = addressNdNe.Assign(p2pDevicesNdNe);


  // Sink deploy
  uint16_t sinkPort = 8080;
  Address sinkAddress(InetSocketAddress(p2pInterfacesNdNe.GetAddress(1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory",
      InetSocketAddress (Ipv4Address::GetAny(), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install(ndNe.Get(1));
  sinkApps.Start(Seconds(0.));
  sinkApps.Stop(Seconds(15.));
  
  // Flow setting
  uint16_t port = 9;
  BulkSendHelper sourceA("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNaNe.GetAddress(0), port));
  sourceA.SetAttribute("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceAppsA = sourceA.Install(naNe.Get(0));
  sourceAppsA.Start(Seconds(0.));
  sourceAppsA.Stop(Seconds(15.));

  BulkSendHelper sourceB("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNbNd.GetAddress(0), port));
  sourceB.SetAttribute("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceAppsB = sourceB.Install(nbNd.Get(0));
  sourceAppsB.Start(Seconds(0.));
  sourceAppsB.Stop(Seconds(15.));

  // First connection path from node c
  BulkSendHelper sourceC1("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNcNd.GetAddress(0), port));
  sourceC1.SetAttribute("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceAppsC1 = sourceC1.Install(ncNd.Get(0));
  sourceAppsC1.Start(Seconds(3.));
  sourceAppsC1.Stop(Seconds(15.));

  // Second connection path from node c
  uint16_t port2 = 10;
  BulkSendHelper sourceC2("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNcNd.GetAddress(0), port2));
  sourceC2.SetAttribute("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceAppsC2 = sourceC2.Install(ncNd.Get(0));
  sourceAppsC2.Start(Seconds(7.));
  sourceAppsC2.Stop(Seconds(12.));

  // Configure routing tables
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Tracer
  pointToPointNaNe.EnablePcapAll("second");
  pointToPointNbNd.EnablePcapAll("second");
  pointToPointNcNd.EnablePcapAll("second");
  pointToPointNdNe.EnablePcapAll("second");

  // Run Simulation
  Simulator::Stop(Seconds(60));
  Simulator::Run();
  Simulator::Destroy();
  std::cout << "Hello WorldDDDD";
  return 0;  
}
