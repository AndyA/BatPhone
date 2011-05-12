/* pitchshift.h */

#ifndef __PITCHSHIFT_H
#define __PITCHSHIFT_H

void smbPitchShift( float pitchShift, long numSampsToProcess,
                    long fftFrameSize, long osamp, float sampleRate,
                    float *indata, float *outdata, int stride );
void smbFft( float *fftBuffer, long fftFrameSize, long sign );
double smbAtan2( double x, double y );

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */
