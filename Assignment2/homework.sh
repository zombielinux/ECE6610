#!/bin/bash

list="TcpNewReno TcpHybla TcpHighSpeed TcpHtcp TcpVegas TcpScalable TcpVeno TcpBic TcpYeah TcpIllinois TcpWestwood TcpWestwoodPlus TcpLedbat"

for j in $list
	do
	for i in 1 4 16 32
		do
		cd /home/will/NS3/ns-allinone-3.26/ns-3.26/scratch/
		/home/will/NS3/ns-allinone-3.26/ns-3.26/waf --run "p1 --nSpokes=$i --Protocol=$j" > /home/will/Homework/Assignment2/$j$i.txt
		sed -i -e 1,4d /home/will/Homework/Assignment2/$j$i.txt
		done
	done
