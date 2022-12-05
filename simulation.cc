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
#include "ns3/ipv4-flow-classifier.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Topology CSE Networks Simulation");

FlowMonitorHelper flowHelper;
Ptr<FlowMonitor> monitor;

/**
 * Calculates average throughput after the simulation, and displays for each flow the 
 * throughput and its associated stats.
*/
void averageThroughput()
{
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier());
    float totalThroughPut = 0;
    int numberOfFlows = 0;
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
        i != stats.end(); ++i)
    {
        double recievedBytes = i->second.rxBytes * 8.0;
        double throughPutMbs = 0;
        ++numberOfFlows;

        /**
         * Incase there was no throughput at a given moment avoid divide by zero. This should not happen for calculations
         * at the end of a simulation.
        */
        if (recievedBytes != 0) {
            double throughPut = (i -> second.rxBytes * 8.0) / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds());
            throughPutMbs = (throughPut / 1000) / 1000; 
        }

        // Get flow source and dest addresses
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << "->" 
            << t.destinationAddress << ")\n";

        // Display packet send and receive stats
        std::cout << " Tx Packets: " << i->second.txPackets << "\n";
        std::cout << " Tx Bytes: " << i->second.txBytes << "\n";
        std::cout << " TxOffered: " << i->second.txBytes * 8.0 / 
            (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds()) / 1000 / 1000 << " Mbps\n";
        std::cout << " Rx Packets: " << i->second.rxPackets << "\n";
        std::cout << " Rx Bytes: " << i->second.rxBytes << "\n";

        // Display throughput
        std::cout << " Throughput: " << throughPutMbs << " Mbps\n";
        totalThroughPut = totalThroughPut + throughPutMbs;
    }

    float averageThroughPut = totalThroughPut / numberOfFlows;
    std::cout << "Average Throughput: " << averageThroughPut << " Mbps\n";
}

/**
 * Runs periodically during the simulation, calculating current througput for each flow,
 * and records the measurements int a csv file.
*/
void CalculateThroughput()
{
    std::ofstream throughputFile;
    throughputFile.open("throughput_vs_time.csv", std::ofstream::app);
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    // Get current simulation time, to know what throughput was at a given moment.
    ns3::Time simTime = Simulator::Now();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
        i != stats.end(); ++i)
    {
        double recievedBytes = i->second.rxBytes * 8.0;
        double throughPutMbs = 0;

        /**
         * Incase there was no throughput at a given moment avoid divide by zero. This could happen if calculation is done before any 
         * flow activity has happened.
        */

        if (recievedBytes != 0) {
            double throughPut = (i -> second.rxBytes * 8.0) / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds());
            throughPutMbs = (throughPut / 1000) / 1000; 
        }

        // Record throughPutMbs, time in simulation in seconds, and flow number.
        throughputFile << throughPutMbs << "," << simTime.GetSeconds() << ",\"Flow " << i->first <<"\"\n";
    }
    throughputFile.close();

    // Reschedule again to take another measurement.
    Simulator::Schedule(MilliSeconds(100), &CalculateThroughput);
}

int main(int argc, char *argv[])
{
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

  // Configure pont to point Connections
  PointToPointHelper pointToPoint;

  // Sense channel and device attributes are all the same for each path,
  // I am reusing the same point to point helper
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("0.5ms"));
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

  // Assigning addresses
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


  // Sink deploy, Server at destination host.
  uint16_t port = 9;
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory",
      InetSocketAddress (Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = packetSinkHelper.Install(naNe.Get(1));
  sinkApps.Start(Seconds(0.));
  sinkApps.Stop(Seconds(15.));
  
  // Flow setting, Client at source hosts. Sending 500,000 bytes at each
  // Client. Unlimited overwhelms my home computer.
  BulkSendHelper source("ns3::TcpSocketFactory", 
      InetSocketAddress(p2pInterfacesNaNe.GetAddress(1), port));
  source.SetAttribute("MaxBytes", UintegerValue(500000));
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
  monitor = flowHelper.InstallAll();

  // Creating file to hold throughput information, placing title row at top
  // Truncates if file already exists for fresh data.
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

  // Schedule first calculate throughput measuring. Will
  // recursively take measurements during the simulation.
  Simulator::Schedule(MilliSeconds(100), &CalculateThroughput);

  // Run Simulation
  Simulator::Stop(Seconds(60));
  Simulator::Run();
  Simulator::Destroy();
  
  // Record average throughput stats
  averageThroughput();

  // fin
  return 0;  
}

