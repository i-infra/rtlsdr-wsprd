/*
 This file is part of program wsprd, a detector/demodulator/decoder
 for the Weak Signal Propagation Reporter (WSPR) mode.

 File name: wsprd.c

 Copyright 2001-2015, Joe Taylor, K1JT

 Much of the present code is based on work by Steven Franke, K9AN,
 which in turn was based on earlier work by K1JT.

 Copyright 2014-2015, Steven Franke, K9AN

 Minor modifications

 Copyright 2016, Guenael Jouchet, VA2GKA

 License: GNU GPL v3

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define HASHTAB_SIZE       32768
#define HASHTAB_ENTRY_LEN  13
#define LOCTAB_ENTRY_LEN   5
#define FFT_SIZE           512
#define MAX_CANDIDATES     200
#define MAX_UNIQUES        100

/* DC spike rejection: skip candidates within +/- DC_REJECT_BW Hz of DC
   whose SNR exceeds the next-best candidate by DC_REJECT_MARGIN dB.
   RTL-SDR devices commonly leak the LO into the digitised stream,
   producing a strong false peak at exactly 0 Hz offset. */
#define DC_REJECT_BW       2.0f    /* Hz half-width around DC */
#define DC_REJECT_MARGIN   15.0f   /* dB above the next candidate */

/* Maximum consecutive Fano timeouts per candidate before abandoning the
   jigger loop early.  A real signal occasionally times out on one offset
   but succeeds on the next; a DC spike or spurious peak times out on
   every attempt.  3 is a good balance: it still gives a real marginal
   signal a few chances while cutting the worst-case ~43-iteration burn. */
#define MAX_JIGGER_TIMEOUTS 3

/* Option & config of decoder (Shared with the wsprd code) */
struct decoder_options {
    int  freq;          // Dial frequency
    char rcall[13];     // Callsign of the RX station
    char rloc[7];       // Locator of the RX station
    int  quickmode;     // Decoder option & tweak
    int  usehashtable;  //  ''
    int  npasses;       //  ''
    int  subtraction;   //  ''
};

struct cand {
    float  freq;
    float  snr;
    int    shift;
    float  drift;
    float  sync;
};

struct decoder_results {
    double freq;
    float  sync;
    float  snr;
    float  dt;
    float  drift;
    int    jitter;
    char   message[23];
    char   call[13];
    char   loc[7];
    char   pwr[3];
    int    cycles;
};

void sync_and_demodulate(float *id,
                         float *qd,
                         long  np,
                         unsigned char *symbols,
                         float *freq,
                         int   ifmin,
                         int   ifmax,
                         float fstep,
                         int   *shift,
                         int   lagmin,
                         int   lagmax,
                         int   lagstep,
                         float *drift,
                         int   symfac,
                         float *sync,
                         int   mode);
void subtract_signal(float *id,
                     float *qd,
                     long  np,
                     float f0,
                     int   shift,
                     float drift,
                     const unsigned char *channel_symbols);
void subtract_signal2(float *id,
                      float *qd,
                      long np,
                      float f0,
                      int shift,
                      float drift,
                      const unsigned char *channel_symbols);
int wspr_decode(float  *idat, 
                float  *qdat, 
                int    samples,
                struct decoder_options options, 
                struct decoder_results *decodes,
                int    *n_results);

