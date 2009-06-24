#!/usr/bin/env python
##
## @file ft_memorypressure.py
## This file is part of ContextKit.
##
## Copyright (C) 2008, 2009 Nokia. All rights reserved.
##
## Contact: Marius Vollmer <marius.vollmer@nokia.com>
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public License
## version 2.1 as published by the Free Software Foundation.
##
## This library is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
## 02110-1301 USA
##

import os
import sys
from subprocess import Popen, PIPE

fMemPagesFree = open('/proc/sys/vm/lowmem_free_pages', 'r')
memFree = 4 * int(fMemPagesFree.read())
fLowMem = open('/proc/sys/vm/lowmem_notify_low_pages','r')
lowMark = 4 * int(fLowMem.read())
fHighMem = open('/proc/sys/vm/lowmem_notify_high_pages', 'r')
highMark = 4 * int(fHighMem.read())

alloc = (memFree - lowMark) >> 10
memload = Popen(["./loadmem",str(alloc+1)], stdin=PIPE, stderr=PIPE)
raw_input('Press Enter...\n')
os.kill(memload.pid, 9)

alloc = (memFree - highMark) >> 10
memload = Popen(["./loadmem",str(alloc+1)], stdin=PIPE, stderr=PIPE)
raw_input('Press Enter...\n')
os.kill(memload.pid, 9)

alloc = (memFree - lowMark) >> 10
memload = Popen(["./loadmem",str(alloc+1)], stdin=PIPE, stderr=PIPE)
raw_input('Press Enter...\n')
os.kill(memload.pid, 9)

memload = Popen(["./loadmem",str(20)], stdin=PIPE, stderr=PIPE)
raw_input('Press Enter...\n')
os.kill(memload.pid, 9)

fMemPagesFree.close()
fHighMem.close()
fLowMem.close()
