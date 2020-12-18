
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>


/* Local Includes. */

#include "FilePOSOutput.h"

#include "nvutility.h"

#include "version.h"



void usage ()
{
  fprintf (stderr, "\nUsage: dump_pos [-n RECORD_NUMBER | -t] POS_OR_SBET_FILE_NAME\n");
  fprintf (stderr, "Dumps the pos or SBET file to stderr.\n\n");
  fprintf (stderr, "\tWhere -n dumps only the requested record number and\n");
  fprintf (stderr, "\t-t dumps only the start and end times.\n\n");
  fflush (stderr);
}


/********************************************************************
 *
 * Module Name : main.c
 *
 * Author/Date : Jan C. Depner
 *
 * Description : Dumps the sbet file listed in the .kin file or the .pos
                 file if there is no .kin file.
 *
 ********************************************************************/

int32_t main (int32_t argc, char **argv)
{
  char               pos_file[512];
  FILE               *pfp = NULL;
  POS_OUTPUT_T       pos;
  char               c;
  extern char        *optarg;
  extern int         optind;
  int32_t            year, month, mday, jday, hour, minute, recnum = -1;
  float              second;
  int64_t            start_timestamp, end_timestamp;
  uint8_t            time_only = NVFalse;


  printf ("\n\n%s\n\n", VERSION);
  fflush (stdout);

  int32_t option_index = 0;
  while (NVTrue) 
    {
      static struct option long_options[] = {{0, no_argument, 0, 0}};

      c = (char) getopt_long (argc, argv, "tn:", long_options, &option_index);
      if (c == -1) break;

      switch (c) 
        {
        case 0:

          switch (option_index)
            {
            case 0:
              break;
            }
          break;

        case 'n':
          sscanf (optarg, "%d", &recnum);
          break;

        case 't':
          time_only = NVTrue;
          break;

        default:
          usage ();
          exit (-1);
          break;
        }
    }


  if (optind >= argc)
    {
      usage ();
      exit (-1);
    }

  strcpy (pos_file, argv[optind]);

  if ((pfp = open_pos_file (pos_file)) == NULL)
    {
      perror (pos_file);
      exit (-1);
    }


  start_timestamp = pos_get_start_timestamp ();
  end_timestamp = pos_get_end_timestamp ();

  charts_cvtime (start_timestamp, &year, &jday, &hour, &minute, &second);
  year += 1900;
  jday2mday (year, jday, &month, &mday);
  fprintf (stderr, "\n\nStart time = %4d/%02d/%02d (%03d) %02d:%02d:%09.6f\n", year, month + 1, 
           mday, jday, hour, minute, second);

  charts_cvtime (end_timestamp, &year, &jday, &hour, &minute, &second);
  year += 1900;
  jday2mday (year, jday, &month, &mday);
  fprintf (stderr, "End time   = %4d/%02d/%02d (%03d) %02d:%02d:%09.6f\n\n\n", year, month + 1, 
           mday, jday, hour, minute, second);
  fflush (stderr);

  if (!time_only)
    {
      if (recnum != -1)
        {
          pos_read_record_num (pfp, &pos, recnum);
          pos_dump_record (pos);
        }
      else
        {
          while (!pos_read_record (pfp, &pos))
            {
              pos_dump_record (pos);
            }
        }
    }


  fclose (pfp);


  return (0);
}
