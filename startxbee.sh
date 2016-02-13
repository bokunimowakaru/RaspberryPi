#!/bin/bash
cd ~
git clone -b raspi https://github.com/bokunimowakaru/xbeeCoord.git
cd xbeeCoord/tools
make
~/xbeeCoord/cqpub_pi/practice10.sh
