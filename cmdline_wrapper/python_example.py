#!/usr/bin/python

import subprocess
import time

while 1:
    p = subprocess.check_output(["usbtenkiget","-i","0"])
    value = float(p)
    print value * 2
    time.sleep(3)
