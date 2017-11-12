
#include <iostream>
#include <cstdint>

using namespace std;

#define CMD_ZER   0x40
#define CMD_ONE   0x80
#define CMD_FOL   0xc0

#define PICSIZE  504

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
 *  <0 - error
 */
int compressImg84x48( const uint8_t *ipic, uint8_t *opic, int *owrited, const int OMAX )
{
    int      iind = 0; // input index array
    int      oind = 0; // output index array
    uint8_t  rb;       // readed byte from ipic
    uint8_t* pcmd;     // pointer to current command in opic array

    if ( (ipic == NULL) || (opic == NULL) ) { cout << "Error: ipic or opic NULL" << endl; return -1; }
    if ( owrited == NULL ) { cout << "Error: where is 'owrited'? a?" << endl; return -1; }
    *owrited = 0;

    pcmd = opic;
    rb = ipic[iind];
    if ( rb == 0x00 ) {
        *pcmd = CMD_ZER;
    } else if ( rb == 0xFF ) {
        *pcmd = CMD_ONE;
    } else {
        *pcmd = CMD_FOL;
        oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
        opic[oind] = rb;
    }
    iind++;

    do {
        rb = ipic[iind];
        if ( rb == 0 ) { // reduce 0x00, max 64 bytes
            if ( (*pcmd & 0xC0) == CMD_ZER ) {
                if ( (*pcmd & 0x3F) < 0x3F ) {
                    (*pcmd)++;
                } else {
                    oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                    pcmd = &opic[oind];
                    *pcmd = CMD_ZER; // new cmd, because an old is overflow
                }
            } else { // prev cmd not zer
                if ( (*pcmd & 0xC0) == CMD_FOL ) { // if prev cmd follow
                    if ( ipic[iind+1] != 0 ) { // and if next byte is not zero
                        if ( (*pcmd & 0x3F) < 0x3F ) {
                            (*pcmd)++;
                            oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                            opic[oind] = rb;
                        } else {
                            oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                            pcmd = &opic[oind];
                            *pcmd = CMD_FOL; // new cmd, because an old is overflow
                            oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                            opic[oind] = rb;
                        }
                        iind++;
                        continue;
                    }
                }
                oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                pcmd = &opic[oind];
                *pcmd = CMD_ZER;
            }
        } else if ( rb == 0xFF ) { // reduce 0xFF, max 64 bytes
            if ( (*pcmd & 0xC0) == CMD_ONE ) {
                if ( (*pcmd & 0x3F) < 0x3F ) {
                    (*pcmd)++;
                } else {
                    oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                    pcmd = &opic[oind];
                    *pcmd = CMD_ONE; // new cmd, because an old is overflow
                }
            } else { // prev cmd not one
                if ( (*pcmd & 0xC0) == CMD_FOL ) { // if prev cmd follow
                    if ( ipic[iind+1] != 0xFF ) { // and if next byte is not one
                        if ( (*pcmd & 0x3F) < 0x3F ) {
                            (*pcmd)++;
                            oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                            opic[oind] = rb;
                        } else {
                            oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                            pcmd = &opic[oind];
                            *pcmd = CMD_FOL; // new cmd, because an old is overflow
                            oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                            opic[oind] = rb;
                        }
                        iind++;
                        continue;
                    }
                }
                oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                pcmd = &opic[oind];
                *pcmd = CMD_ONE;
            }
        } else { // follows other bytes - not reduces
            if ( (*pcmd & 0xC0) == CMD_FOL ) {
                if ( (*pcmd & 0x3F) < 0x3F ) {
                    (*pcmd)++;
                    oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                    opic[oind] = rb;
                } else {
                    oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                    pcmd = &opic[oind];
                    *pcmd = CMD_FOL; // new cmd, because an old is overflow
                    oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                    opic[oind] = rb;
                }
            } else {
                oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                pcmd = &opic[oind];
                *pcmd = CMD_FOL; // new cmd, because an old is overflow
                oind++; if ( oind >= OMAX ) { cout << "Error: opic not enough" << endl; return -2; }
                opic[oind] = rb;
            }
        }
        iind++;
    } while ( iind < PICSIZE );
    *owrited = ++oind;
    return 0;
}

/*
 * Decompress compressed image 84x48
 * Params:
 *   ipic - pointer to compressed input array of image
 *   opic - pointer ot output array of decompressed image
 *   OMAX - size of input array of image
 * Return:
 *   0 - good
 *  <0 - error
 */
int decompressImv84x48( const uint8_t *ipic, uint8_t *opic, int iMAX )
{
    int      iind = 0; // input index array
    int      oind = 0; // output index array
    int      cnt;      // intermediate helper counter

    if ( (ipic == NULL) || (opic == NULL) ) { cout << "Error: ipic or opic NULL" << endl; return -1; }

    while ( (iind < iMAX) && (oind < PICSIZE) ) {
        if ( (ipic[iind] & 0xC0) == CMD_ZER ) { // reduce 0x00, max 64 bytes
            cnt = ( ipic[iind] & 0x3F ) + 1;
            for ( int n = 0; n < cnt; n++ ) {
                if ( oind >= PICSIZE ) { cout << "Error: oind overflow (in CMD_ZER)" << endl; return -2; }
                opic[oind++] = 0x00;
            }
            iind++;
        } else if ( (ipic[iind] & 0xC0) == CMD_ONE ) { // reduce 0xFF, max 64 bytes
            cnt = ( ipic[iind] & 0x3F ) + 1;
            for ( int n = 0; n < cnt; n++ ) {
                if ( oind >= PICSIZE ) { cout << "Error: oind overflow (in CMD_ONE)" << endl; return -2; }
                opic[oind++] = 0xFF;
            }
            iind++;
        } else if ( (ipic[iind] & 0xC0) == CMD_FOL ) { // follows other bytes - not reduces
            cnt = ( ipic[iind] & 0x3F ) + 1;
            if ( (iind + cnt + 1) > iMAX ) { cout << "Error: try to get more then iMAX" << endl; return -3; }
            for ( int n = 0; n < cnt; n++ ) {
                if ( oind >= PICSIZE ) { cout << "Error: oind overflow (in CMD_FOL)" << endl; return -2; }
                opic[oind++] = ipic[iind + n + 1];
            }
            iind += (cnt + 1);
        } else { // error: no cmd found
            cout << "Error: where is input cmd? iind = " << iind << endl;
            return -4;
        }
    }

    if ( iind == iMAX ) {
        cout << "Super good! : iind == iMAX" << endl;
    } else {
        cout << "Xmm: iind != iMAX : iind=" << iind << " iMAX=" << iMAX << endl;
    }

    return 0;
}
