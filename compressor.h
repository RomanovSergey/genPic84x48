#ifndef COMPRESSOR_H
#define COMPRESSOR_H

int compressImg84x48( const uint8_t *ipic, uint8_t *opic, int *owrited, const int OMAX );
int decompressImv84x48( const uint8_t *ipic, uint8_t *opic, int iMAX );

#endif // COMPRESSOR_H
