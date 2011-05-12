/* batty.c */

#include <getopt.h>
#include <sndfile.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pitchshift.h"

#define PROG "batty"
#define VERSION "0.01"

static void
die( const char *msg, ... ) {
  va_list ap;
  va_start( ap, msg );
  fprintf( stderr, "Fatal: " );
  vfprintf( stderr, msg, ap );
  fprintf( stderr, "\n" );
  va_end( ap );
  exit( 1 );
}

static double
parse_double( const char *s ) {
  char *endp;
  double v = strtod( s, &endp );
  if ( endp == s || *endp ) {
    die( "Bad number: %s", s );
  }
  return v;
}

static long
parse_long( const char *s ) {
  char *endp;
  long v = strtol( s, &endp, 10 );
  if ( endp == s || *endp ) {
    die( "Bad number: %s", s );
  }
  return v;
}

static void *
alloc( size_t sz ) {
  void *m = malloc( sz );
  if ( !m ) {
    die( "Out of memory" );
  }
  memset( m, 0, sz );
  return m;
}

static void
pitchshift( const char *infile, const char *outfile, double shift,
            long framesize, long osamp ) {
  SNDFILE *in, *out;
  SF_INFO ininfo, outinfo;
  sf_count_t got;
  float *buf;
  long ofs;

  ininfo.format = 0;
  if ( in = sf_open( infile, SFM_READ, &ininfo ), !in ) {
    die( "Failed to open %s", infile );
  }

  buf = alloc( sizeof( float ) * ininfo.channels * ininfo.frames );
  got = sf_readf_float( in, buf, ininfo.frames );
  sf_close( in );

  if ( got != ininfo.frames ) {
    die( "Short read" );
  }

  for ( ofs = 0; ofs < ininfo.channels; ofs++ ) {
    smbPitchShift( shift, ininfo.frames, framesize, osamp,
                   ininfo.samplerate, buf + ofs, buf + ofs,
                   ininfo.channels );
  }

  outinfo = ininfo;
  if ( out = sf_open( outfile, SFM_WRITE, &ininfo ), !out ) {
    die( "Failed to open %s", outfile );
  }

  got = sf_writef_float( out, buf, outinfo.frames );
  sf_close( out );

  if ( got != outinfo.frames ) {
    die( "Short write" );
  }

}

static void
usage( void ) {
  fprintf( stderr, "Usage: " PROG " [options] <in> <out>\n\n"
           "Options:\n"
           "  -S, --shift <n>       Pitch shift (0.5 .. 2.0)\n"
           "  -O, --oversample <n>  Oversample\n"
           "  -F, --framesize <n>   FFT frame size\n"
           "  -V, --version         See version number\n"
           "  -h, --help            See this text\n\n" );
  exit( 1 );
}

int
main( int argc, char **argv ) {
  double shift = 1.0;
  long osamp = 16;
  long framesize = 2048;
  int ch;

  static struct option opts[] = {
    {"help", no_argument, NULL, 'h'},
    {"shift", required_argument, NULL, 'S'},
    {"oversample", required_argument, NULL, 'O'},
    {"framesize", required_argument, NULL, 'F'},
    {"version", no_argument, NULL, 'V'},
    {NULL, 0, NULL, 0}
  };

  static char *optn = "hVS:O:F:";

  while ( ch = getopt_long( argc, argv, optn, opts, NULL ), ch != -1 ) {
    switch ( ch ) {
    case 'V':
      printf( "%s %s\n", PROG, VERSION );
      return 0;
    case 'S':
      shift = parse_double( optarg );
      break;
    case 'O':
      osamp = parse_long( optarg );
      break;
    case 'F':
      framesize = parse_long( optarg );
      break;
    case 'h':
    default:
      usage(  );
    }
  }

  argc -= optind;
  argv += optind;

  if ( argc != 2 ) {
    usage(  );
  }

  pitchshift( argv[0], argv[1], shift, framesize, osamp );

  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */
