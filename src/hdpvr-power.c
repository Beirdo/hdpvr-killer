/*
 * Copyright 2011 Gavin Hurlbut
 *
 * This file is part of HDPVR-killer-utils.
 * 
 * HDPVR-killer-utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * HDPVR-killer-utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with HDPVR-killer-utils.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <ftdi.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int find_chipid(unsigned int chipid)
{
    FILE *fp;
    char line[80];
    unsigned int id;
    char *string;

    fp = fopen( "/etc/hdpvr.chipids", "r" );
    if( !fp ) {
        printf( "Couldn't open /etc/hdpvr.chipids: %s", strerror(errno) );
        return( 0 );
    }

    while( !feof(fp) ) {
        string = fgets(line, 80, fp);
        id = strtoul(line, NULL, 16);
        if( id == chipid ) {
            fclose( fp );
            return( 1 );
        }
    }

    fclose( fp );
    return( 0 );
}

struct ftdi_context *open_ftdi_usb(char *serial)
{
    struct ftdi_context    *fc;
    char                    description[1024];
    int                     retval;
    unsigned int            chipid;

    snprintf(description, 1024, "s:0x0403:0x6001:%s", serial);

    fc = ftdi_new();
    retval = ftdi_usb_open_string( fc, description );
    if( retval ) {
        printf( "Couldn't open %s\n", description );
        ftdi_free( fc );
        return( NULL );
    }

    printf( "Found FTDI device serial number %s\n", serial );

    // Read out FTDIChip-ID of R type chips (which is all we use!)
    if (fc->type != TYPE_R) {
        printf( "FTDI device is not type R, incorrect device!\n" );
        ftdi_free( fc );
        return( NULL );
    }

    ftdi_read_chipid(fc, &chipid);
    printf("\tFTDI chipid: %08X\n", chipid);

    if( !find_chipid(chipid) ) {
        printf( "FTDI device with chipid %08X is not in accepted device "
                "list!\n", chipid );
        ftdi_free( fc );
        return( NULL );
    }

    return( fc );
}

void hdpvr_power(struct ftdi_context *ftdi, int on)
{
    printf("Turning HDPVR %s\n", (on ? "on" : "off"));
    ftdi_set_bitmode(ftdi, (on ? 0x10 : 0x11), BITMODE_CBUS);
}

int main(int argc, char **argv)
{
    struct ftdi_context *ftdi;

    if( argc < 2 ) {
        fprintf(stderr, "Usage: %s serial on|off|cycle\n\n", argv[0]);
        exit(1);
    }

    ftdi = open_ftdi_usb(argv[1]);
    if( !ftdi ) {
        exit(1);
    }

    if( !strcasecmp(argv[2], "on") ) {
        hdpvr_power(ftdi, 1);
    } else if( !strcasecmp(argv[2], "off") ) {
        hdpvr_power(ftdi, 0);
    } else if( !strcasecmp(argv[2], "cycle") ) {
        hdpvr_power(ftdi, 0);
        printf("Delaying 15s\n");
        sleep(15);
        hdpvr_power(ftdi, 1 );
    }

    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    exit( 0 );
}

/*
 * vim:ts=4:sw=4:ai:et:si:sts=4
 */
