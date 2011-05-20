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
#include <string.h>


void probe_usb(void)
{
    struct ftdi_context     *fc;
    struct ftdi_device_list *devlist;
    struct ftdi_device_list *dev;
    int                      count;
    int                      i;
    int                      j;
    char                     manufacturer[1024];
    char                     description[1024];
    char                     serial[1024];
    unsigned char            eeprom[128];
    char                     line[17];
    unsigned int             chipid;
    int                      size;
    struct ftdi_eeprom       eedata;

    fc = ftdi_new();
    count = ftdi_usb_find_all( fc, &devlist, 0x0403, 0x6001 );

    printf( "%d device%s found:\n", count, (count == 1 ? "" : "s") );

    if( count <= 0 ) {
        ftdi_free( fc );
        return;
    }

    for( dev = devlist, i = 0; i < count && dev; i++, dev = dev->next ) {
        ftdi_usb_get_strings(fc, dev->dev, manufacturer, 1024,
                             description, 1024, serial, 1024);
        printf("%d:\tmanufacturer: %s, description: %s, serial: %s\n",
               i, manufacturer, description, serial);

        ftdi_usb_open_dev(fc, dev->dev);
        // Read out FTDIChip-ID of R type chips
        if (fc->type != TYPE_R) {
            printf("\tNot type R, skipping\n");
            continue;
        }

        ftdi_read_chipid(fc, &chipid);
        printf("\tFTDI chipid: %X\n", chipid);

        size = ftdi_read_eeprom_getsize(fc, eeprom, 128);
        printf("\tEEPROM Size %d\n", size);
        memset(line, 0, 17);
        for(j = 0; j < size; j++) {
            int index = j % 16;
            if( index == 0 ) {
                printf( "\t" );
            }
            printf( "%02X ", eeprom[j] );
            if( eeprom[j] >= 0x20 && eeprom[j] <= 0x7E ) {
                line[index] = eeprom[j];
            } else {
                line[index] = '.';
            }

            if( index == 15 ) {
                printf(" |  %s\n", line);
                memset(line, 0, 17);
            }
        }
        if( j % 16 ) {
            int index = j % 16;
            printf("%*s |  %s\n", 3 * (16 - index), " ", line);
        }

        ftdi_eeprom_decode( &eedata, eeprom, size );

        printf("\tVendorID: %04X, ProductID: %04X, self-powered: %d, remote wakeup: %d\n", eedata.vendor_id, eedata.product_id, eedata.self_powered, eedata.remote_wakeup);
        printf("\tChipType: %d, InIso: %d, OutIso: %d, SuspPullDown: %d, UseSerial: %d\n", eedata.BM_type_chip, eedata.in_is_isochronous, eedata.out_is_isochronous, eedata.suspend_pull_downs, eedata.use_serial);
        printf("\tFakeVersion: %d, USBVersion: %d, MaxPwr: %d\n", eedata.change_usb_version, eedata.usb_version, eedata.max_power);
        printf("\tManufacturer (%d): %s\n"
               "\tProduct (%d): %s\n"
               "\tSerial (%d): %s\n"
               "\tSize: %d\n", (int)strlen(eedata.manufacturer), eedata.manufacturer, (int)strlen(eedata.product), eedata.product, (int)strlen(eedata.serial), eedata.serial, eedata.size);

        ftdi_usb_close(fc);

        printf("\n");
    }

    ftdi_list_free(&devlist);
    ftdi_free(fc);
}

int main(int argc, char **argv)
{
    probe_usb();
    exit(0);
}

/*
 * vim:ts=4:sw=4:ai:et:si:sts=4
 */
