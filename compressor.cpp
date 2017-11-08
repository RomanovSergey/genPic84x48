
#include <iostream>
#include <cstdint>

using namespace std;

#define CMD_ZER   0x40
#define CMD_ONE   0x80
#define CMD_FOL   0xc0

#define MAX_ICNT  504

/*
 * Simple compress binary image (84x48 format)
 * Only 0x00 and 0xFF duplicate will reduce
 * Params:
 *   ipic - pointer to input array of image
 *   opic - pointer ot output array of image
 *   owrited - how much byte writed to opic
 *   OMAX - size of output array of image
 * Return:
 *   0 - good
 */
int compressImg84x48( const uint8_t *ipic, uint8_t *opic, int *owrited, int OMAX )
{
    int      iind = 0; // input index array
    int      oind = 0; // output index array
    int      cnt;      // intermediate helper counter

    if ( (ipic == NULL) || (opic == NULL) ) { cout << "Error: ipic or opic NULL" << endl; return -1; }
    if ( owrited == NULL ) { cout << "Error: where is 'owrited'? a?" << endl; return -1; }
    *owrited = 0;

    while ( (iind < MAX_ICNT) && (oind < OMAX) ) {
        if ( ipic[iind] == 0 ) { // reduce 0x00, max 64 bytes
            cnt = 0;
            while ( cnt < 64 ) {
                iind++;
                if ( iind >= MAX_ICNT ) { break; }
                if ( ipic[iind] == 0 ) { cnt++; } else {  break; }
            }
            opic[oind] = CMD_ZER + cnt;
            oind++;
            if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
        } else if ( ipic[iind] == 0xFF ) { // reduce 0xFF, max 64 bytes
            cnt = 0;
            while ( cnt < 64 ) {
                iind++;
                if ( iind >= MAX_ICNT ) { break; }
                if ( ipic[iind] == 0xFF ) { cnt++; } else {  break; }
            }
            opic[oind] = CMD_ONE + cnt;
            oind++;
            if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
        } else { // follows other bytes - not reduces
            cnt = 0;
            opic[oind + cnt + 1] = ipic[iind];
            while ( cnt < 64 ) {
                iind++;
                if ( iind >= MAX_ICNT ) { break; }
                if ( (ipic[iind] == 0x00) || (ipic[iind] == 0xFF) ) {
                    break;
                } else {
                    cnt++;
                    opic[oind + cnt + 1] = ipic[iind];
                }
            }
            opic[oind] = CMD_FOL + cnt;
            oind += (cnt + 1);
            if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
        }
    }
     *owrited = oind;
    return 0;
}

