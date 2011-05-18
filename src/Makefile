#
# Copyright 2011 Gavin Hurlbut
#
# This file is part of HDPVR-killer-utils.
# 
# HDPVR-killer-utils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# HDPVR-killer-utils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with HDPVR-killer-utils.  If not, see <http://www.gnu.org/licenses/>.
# 

INSTALL = /usr/bin/install
LDFLAGS = -lftdi
CFLAGS  = -O3 -g -Wall -Werror
SOURCES = ftdi-probe.c hdpvr-power.c
OBJS    = ${SOURCES:%.c=%.o}
BINS	= ${SOURCES:%.c=%}

all:	${BINS}

%:	%.o
%.o:	%.c

clean:
	${RM} ${BINS} ${OBJS}

install:	all hdpvr.chipids
	${INSTALL} -m 755 ${BINS} /usr/local/bin
	${INSTALL} -m 644 hdpvr.chipids /etc

