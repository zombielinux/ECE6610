#!/bin/bash

list="TcpNewReno TcpHybla TcpHighSpeed TcpHtcp TcpVegas TcpScalable TcpVeno TcpBic TcpYeah TcpIllinois TcpWestwood TcpWestwoodPlus TcpLedbat"

for j in $list
	do
	for i in 1 4 8 16 32
		do
		cd /home/will/NS3/ns-allinone-3.26/ns-3.26/scratch/
		/home/will/NS3/ns-allinone-3.26/ns-3.26/waf --run "p1 --nSpokes=$i --Protocol=$j" > /home/will/Homework/Assignment2/$j$i.txt
		sed -i -e 1,4d /home/will/Homework/Assignment2/$j$i.txt
		totaltx="$(cat /home/will/Homework/Assignment2/$j$i.txt | grep Totals | cut -c 17-23)"
		echo "${j},${i},${totaltx}" >> /home/will/Homework/Assignment2/results.txt
		done
	done
# | grep Throughput | cut -c 17-23
