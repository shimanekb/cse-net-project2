#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Topology CSE Networks Simulation");

Ptr<FlowMonitor> monitor;

void CalculateThroughput()
{

    std::ofstream throughputFile;
    throughputFile.open("throughput_vs_time.csv", std::ofstream::app);
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    ns3::Time simTime = Simulator::Now();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
        i != stats.end(); ++i)
    {
        double recievedBytes = i->second.rxBytes * 8.0;
        double throughPutMbs = 0;

        if (recievedBytes != 0) {
            double throughPut = (i -> second.rxBytes * 8.0) / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds());
            throughPutMbs = (throughPut / 1000) / 1000; 
        }

        throughputFile << throughPutMbs << "," << simTime.GetSeconds() << ",\"Flow " << i->first <<"\"\n";
    }
    throughputFile.close();
    Simulator::Schedule(MilliSeconds(100), &CalculateThroughput);
}

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
  PointToPointHelper pointToPoint;
  NetDeviceContainer p2pDevicesNaNe;
  p2pDevicesNaNe = pointToPoint.Install(naNe);

  NetDeviceContainer p2pDevicesNbNd;
  p2pDevicesNbNd = pointToPoint.Install(nbNd);

  NetDeviceContainer p2pDevicesNcNd;
  p2pDevicesNcNd = pointToPoint.Install(ncNd);

  NetDeviceContainer p2pDevicesNdNe;
  p2pDevicesNdNe = pointToPoint.Install(ndNe);

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
  BulkSendHelper source("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNaNe.GetAddress(1), port));
  source.SetAttribute("MaxBytes", UintegerValue(200000));
  ApplicationContainer sourceAppsA = source.Install(naNe.Get(0));
  sourceAppsA.Start(Seconds(0.));
  sourceAppsA.Stop(Seconds(15.));

  ApplicationContainer sourceAppsB = source.Install(nbNd.Get(0));
  sourceAppsB.Start(Seconds(0.));
  sourceAppsB.Stop(Seconds(15.));

  // First connection path from node c
  ApplicationContainer sourceAppsC1 = source.Install(ncNd.Get(0));
  sourceAppsC1.Start(Seconds(3.));
  sourceAppsC1.Stop(Seconds(15.));

  // Second connection path from node c
  ApplicationContainer sourceAppsC2 = source.Install(ncNd.Get(0));
  sourceAppsC2.Start(Seconds(7.));
  sourceAppsC2.Stop(Seconds(12.));

  // Configure routing tables
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Schedule monitoring
  FlowMonitorHelper flowHelper;
  monitor = flowHelper.InstallAll();

  // Creating file to hold throughput information
  std::ofstream throughputFile;
  throughputFile.open("throughput_vs_time.csv", std::ofstream::trunc);
  throughputFile << "\"Throughput (Mbps)\",\"Time (s)\",\"Flow\"\n" ;
  throughputFile.close();

  // Animation 
  AnimationInterface anim("test.xml");
  anim.SetConstantPosition(naNe.Get(0), 10.0, 0.0);
  anim.SetConstantPosition(nbNd.Get(0), 10.0, 30.0);
  anim.SetConstantPosition(ncNd.Get(0), 10.0, 60.0);


  anim.SetConstantPosition(naNe.Get(1), 80.0, 30.0);
  anim.SetConstantPosition(ndNe.Get(0), 40.0, 60.0);

  Simulator::Schedule(MilliSeconds(10000), &CalculateThroughput);

  // Run Simulation
  Simulator::Stop(Seconds(60));
  Simulator::Run();
  Simulator::Destroy();

  
  return 0;  
}

