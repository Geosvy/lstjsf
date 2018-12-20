/*	lstjsf	List some of the contents of an Edgetech jsf file
 *
 *	Thomas O'Brien
 *      US Geologicl Survey
 *      384 Woods Hole Road
 *      Woods Hole, MA 02543
 *
 *	tobrien@usgs.gov
 *	tforideshd@gmail.com
 *
 *      November 11 2005
 *      Rolling in features to identify subbottom channels 
 *      and to unravel to new output file
 *
 *	September 19 2012
 *	Install a couple of hooks to check for and notify user of
 *	record length changes in jsf file
 *
 *	September 7 2017
 *	Add some features from latest jsf file format definitions
 *	Rev. 1.18 
 *
 *	October 22 2018
 *	Tidy things up a bit
 */

#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "jsf2.h"
#include "segy_rev_1.h"

#define  Bit4 0x10

char *progname;

int
main (int argc, char *argv[])
{				/* START MAIN */

  int i;
  int j;
  int c;
  int in_fd;			/* file descriptor */
  int tseq_reel;
  int tseq_line;
  int pingNum;
  int doing_SB;
  int do_list_subbottom;
  int do_list_sidescan;
  int do_rec_count;
  int done_Calloc;
  int DataSize;
  int ProcessedPing;
  int SeismicRecords;
  int SidescanRecords;
  int RecLength;
  int JSFmsgSize;
  int inbytes;
  int SegBytes;
  int header_done;
  int Hi_Port_SS;
  int Hi_Stbd_SS;
  int Low_Port_SS;
  int Low_Stbd_SS;
  int iFirst;
  int Current_RecLength = 0;

  off_t offset;			/* offset for seeking starting record */
  off_t where;

  unsigned char *JSFmsg;
  unsigned char JSFSEGYHead[TRHDLEN];

  doing_SB = 0;
  tseq_reel = 1;
  tseq_line = 1;
  do_rec_count = 0;
  do_list_sidescan = 0;
  do_list_subbottom = 0;
  done_Calloc = 0;
  pingNum = 0;
  ProcessedPing = 0;
  j = 0;
  i = 0;
  iFirst = 0;
  SeismicRecords = 0;
  SidescanRecords = 0;
  DataSize = 0;
  JSFmsgSize = 16;
  SegBytes = 0;
  header_done = 0;
  Hi_Port_SS = 0;
  Hi_Stbd_SS = 0;
  Low_Port_SS = 0;
  Low_Stbd_SS = 0;
  RecLength = 0;

  progname = argv[0];

  if ((argc - optind) < 2)
    usage ();

/*
 *     Process arguments
 */

  while ((c = getopt (argc, argv, "csb")) != -1)
    {
      switch (c)
	{
	case 'c':
	  do_rec_count++;
	  break;
	case 's':
	  do_list_sidescan++;
	  break;
	case 'b':
	  do_list_subbottom++;
	  break;
	case '?':
	  err_exit ();
	  break;
	}
    }

/*          
 *    open the input jsf file
 */

  if ((in_fd = open (argv[optind], O_RDONLY)) == -1)
    {
      fprintf (stderr, "%s: cannot open %s\n", argv[optind], progname);
      perror ("open");
      err_exit ();
    }

/*
 *	Get some memory for JSF header
 */

  JSFmsg =
    (unsigned char *) calloc ((size_t) JSFmsgSize, sizeof (unsigned char));

/* 
 *      Count records ?
 */

  if (do_rec_count)
    {
      while (1)
	{
	  inbytes = read (in_fd, JSFmsg, (size_t) JSFmsgSize);
	  if (inbytes == ZERO)
	    {
	      fprintf (stdout,
		       "End of File reached %d seismic records and %d sidescan records in input file\n",
		       SeismicRecords, SidescanRecords);
	      exit (EXIT_SUCCESS);
	    }
	  if (inbytes != (int) JSFmsgSize)
	    {
	      fprintf (stderr, "%s: error reading JSF message header\n",
		       progname);
	      perror ("read");
	      err_exit ();
	    }

	  if (get_short (JSFmsg, 0) != 0x1601)
	    {
	      fprintf (stdout, "Invalid file format\n");
	      err_exit ();
	    }

	  if (get_short (JSFmsg, 4)
	      == Sonar_Data_Msg && (int) JSFmsg[7] == SubBottom)
	    SeismicRecords++;

	  if (get_short (JSFmsg, 4)
	      == Sonar_Data_Msg && ((int) JSFmsg[7] == Hi_Sidescan ||
				    (int) JSFmsg[7] == Low_Sidescan))
	    SidescanRecords++;

/*
 *  Skip to next JSF Header
 */

	  offset = (off_t) get_int (JSFmsg, 12);
	  where = lseek (in_fd, offset, SEEK_CUR);
	  pingNum++;
	}			// end while()
      exit (EXIT_SUCCESS);	/* Should not get here */
    }				/* End do_rec_count loop */

/*
 *   Check if listing Sidescan records
 */

  if (do_list_sidescan)
    {
      while (1)
	{
	  ProcessedPing = 0;
	  if (!header_done)
	    {
	      fprintf (stdout,
		       "Channel\tPing #\tFormat\t\tX\t\tY\t    Coord\tYear\tHour\tMinute\tSecond\t\tNrecs\n");
	      header_done++;
	    }

	  inbytes = read (in_fd, JSFmsg, (size_t) JSFmsgSize);
	  if (inbytes == ZERO)
	    {
	      fprintf (stdout,
		       "Channel\tPing #\tFormat\t\tX\t\tY\t    Coord\tYear\tHour\tMinute\tSecond\t\tNrecs\n");
	      fprintf (stdout,
		       "%d Port SS High Freq Records  %d Stbd SS High Freq Records\n",
		       Hi_Port_SS, Hi_Stbd_SS);
	      fprintf (stdout,
		       "%d Port SS Low Freq Records  %d Stbd SS Low Freq Records\n",
		       Low_Port_SS, Low_Stbd_SS);
	      exit (EXIT_SUCCESS);
	    }
	  if (inbytes != (int) JSFmsgSize)
	    {
	      fprintf (stderr, "%s: error reading JSF message header\n",
		       progname);
	      perror ("read");
	      err_exit ();
	    }

	  if (get_short (JSFmsg, 0) != 0x1601)
	    {
	      fprintf (stdout, "Invalid file format\n");
	      err_exit ();
	    }

	  if (get_short (JSFmsg, 4) == Sonar_Data_Msg &&
	      ((int) JSFmsg[7] == Hi_Sidescan || JSFmsg[7] == Low_Sidescan))
	    {

/* 
 *  Lets now get the Edgetech Sonar Data Message (Message type 80)"
 */
	      SegBytes = read (in_fd, JSFSEGYHead, (size_t) TRHDLEN);
	      if (SegBytes != TRHDLEN)
		{
		  fprintf (stderr,
			   "%s: error reading JSF trace header and data\n",
			   progname);
		  perror ("read");
		  err_exit ();
		}
	      if ((int) JSFmsg[8] == Port_SS)
		{
		  fprintf (stdout, "Port ");
		  if ((int) JSFmsg[7] == Low_Sidescan)
		    Low_Port_SS++;
		  if ((int) JSFmsg[7] == Hi_Sidescan)
		    Hi_Port_SS++;
		}
	      if ((int) JSFmsg[8] == Stbd_SS)
		{
		  fprintf (stdout, "Stbd ");
		  if ((int) JSFmsg[7] == Low_Sidescan)
		    Low_Stbd_SS++;
		  if ((int) JSFmsg[7] == Hi_Sidescan)
		    Hi_Stbd_SS++;
		}
	      fprintf (stdout, "\t%d", get_int (JSFSEGYHead, 8));
	      if (get_short (JSFSEGYHead, 34) == 0)
		fprintf (stdout, "\tEnvelope");
	      if (get_short (JSFSEGYHead, 34) == 1)
		fprintf (stdout, "\tAnalytic");
	      if (get_short (JSFSEGYHead, 34) == 2)
		fprintf (stdout, "\t1 Short Not M/F");
	      if (get_short (JSFSEGYHead, 34) == 3)
		fprintf (stdout, "\tReal");
	      if (get_short (JSFSEGYHead, 34) == 9)
		fprintf (stdout, "\t2 Shorts R/I Not MF");
	      fprintf (stdout, "\t%d", get_int (JSFSEGYHead, 80));	/* X */
	      fprintf (stdout, "\t%d", get_int (JSFSEGYHead, 84));	/* Y */
	      if (get_short (JSFSEGYHead, 88) == 1)
		fprintf (stdout, "\tXY");
	      if (get_short (JSFSEGYHead, 88) == 2)
		fprintf (stdout, "\tLL");
	      if (get_short (JSFSEGYHead, 88) == 3)
		fprintf (stdout, "\tXY Decm");
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 198));	/* Year */
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 186));	/* Hour */
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 188));	/* Minutes */
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 190));	/* Seconds */
	      fprintf (stdout, "\t\t%d", get_int (JSFmsg, 12));	/* # bytes in ping */
	      fprintf (stdout, "\n");

	      if (!ProcessedPing)
		{
		  offset = (off_t) (get_int (JSFmsg, 12) - SegBytes);
		  where = lseek (in_fd, offset, SEEK_CUR);
		  ProcessedPing++;
		}
	    }

	  if (!ProcessedPing)
	    {
	      offset = (off_t) get_int (JSFmsg, 12);
	      where = lseek (in_fd, offset, SEEK_CUR);
	    }
	}			// end while()

      exit (EXIT_SUCCESS);	/* Should not get here */
    }				// end do_list_ss

  if (do_list_subbottom)
    {
      while (1)
	{
	  ProcessedPing = 0;
	  if (!header_done)
	    {
	      fprintf (stdout,
		       "Ping #\tFormat\t\t   X\t\t   Y\t       Coord    Year   Hour   Minute   Second\tAltitude (mm)\tNbytes\t# Samps\tWeighting\tSampRate\n");
	      header_done++;
	    }

	  inbytes = read (in_fd, JSFmsg, (size_t) JSFmsgSize);
	  if (inbytes == ZERO)
	    {
	      fprintf (stdout,
		       "Ping #\tFormat\t\t   X\t\t   Y\t       Coord    Year   Hour   Minute   Second\tAltitude (mm)\tNbytes\t# Samps\tWeighting\tSampRate\n");
	      fprintf (stdout, "%d Subbottom Records\n", SeismicRecords);
	      exit (EXIT_SUCCESS);
	    }
	  if (inbytes != (int) JSFmsgSize)
	    {
	      fprintf (stderr, "%s: error reading JSF message header\n",
		       progname);
	      perror ("read");
	      err_exit ();
	    }

	  if (get_short (JSFmsg, 0) != 0x1601)
	    {
	      fprintf (stdout, "Invalid file format\n");
	      err_exit ();
	    }

	  if (get_short (JSFmsg, 4) == Sonar_Data_Msg &&
	      ((int) JSFmsg[7] == SubBottom))
	    {

	      if (!iFirst)
		{
		  RecLength = get_int (JSFmsg, 12);
		  iFirst++;
		}
	      Current_RecLength = get_int (JSFmsg, 12);

/*
 *	Check if the record length has changed
 */

	      if (Current_RecLength != RecLength)
		{
		  fprintf (stdout,
			   "Record length change from %d Bytes to %d Bytes at Ping # %d\n",
			   RecLength, get_int (JSFmsg, 12), ++pingNum);
		  RecLength = Current_RecLength;
		}
/*
 *  Lets now get the Edgetech Sonar Data Message (Message type 80)"
 */
	      SegBytes = read (in_fd, JSFSEGYHead, (size_t) TRHDLEN);
	      if (SegBytes != TRHDLEN)
		{
		  fprintf (stderr,
			   "%s: error reading JSF trace header and data\n",
			   progname);
		  perror ("read");
		  err_exit ();
		}
	      SeismicRecords++;

//            fprintf (stdout, "%d", get_int (JSFSEGYHead, 8));

	      fprintf (stdout, "%d", ++pingNum);
	      if (get_short (JSFSEGYHead, 34) == 0)
		fprintf (stdout, "\tEnvelope");
	      if (get_short (JSFSEGYHead, 34) == 1)
		fprintf (stdout, "\tAnalytic");
	      if (get_short (JSFSEGYHead, 34) == 2)
		fprintf (stdout, "\tRaw");
	      if (get_short (JSFSEGYHead, 34) == 3)
		fprintf (stdout, "\tReal");
	      if (get_short (JSFSEGYHead, 34) == 4)
		fprintf (stdout, "\tPixel");
	      fprintf (stdout, "\t%d", get_int (JSFSEGYHead, 80));
	      fprintf (stdout, "\t%d", get_int (JSFSEGYHead, 84));
	      if (get_short (JSFSEGYHead, 88) == 1)
		fprintf (stdout, "\tXY");
	      if (get_short (JSFSEGYHead, 88) == 2)
		fprintf (stdout, "\tLL");
	      if (get_short (JSFSEGYHead, 88) == 3)
		fprintf (stdout, "\tXY Decm");
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 198));
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 186));
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 188));
	      fprintf (stdout, "\t%d", get_short (JSFSEGYHead, 190));
              fprintf (stdout, "\t%d", get_int (JSFSEGYHead, 144));
	      fprintf (stdout, "\t\t%d", get_int (JSFmsg, 12));
              fprintf (stdout, "\t  %d", get_short (JSFSEGYHead, 114));
	      fprintf (stdout, "\t   %d", get_short (JSFSEGYHead, 168));
	      fprintf (stdout, "\t\t%d", get_int (JSFSEGYHead, 116));
	      fprintf (stdout, "\n");

	      if (!ProcessedPing)
		{
		  offset = (off_t) (get_int (JSFmsg, 12) - SegBytes);
		  where = lseek (in_fd, offset, SEEK_CUR);
		  ProcessedPing++;
		}
	    }
	  if (!ProcessedPing)
	    {
	      offset = (off_t) get_int (JSFmsg, 12);
	      where = lseek (in_fd, offset, SEEK_CUR);
	    }
	}			// End while() 
    }				// End do_list_subbottom

  exit (EXIT_SUCCESS);
}				/* END MAIN */

void
err_exit (void)
{
  fprintf (stdout, "Error: Segmentation Error: Core Dump\n");
  exit (EXIT_FAILURE);
}

void
usage (void)
{

  fprintf (stdout, "lstjsf ... Lists Edgetech JSF formatted files\n");
  fprintf (stdout,
	   "Usage:	lstjsf - options full path to input file name\n");
  fprintf (stdout,
	   "Options \t-c Get count of Subbottom and Sidescan records\n");
  fprintf (stdout, "\t\t-s List Sidescan Sonar message header\n");
  fprintf (stdout,
	   "\t\t-b List Subbottom message header, checks for record length change\n");
  exit (EXIT_FAILURE);
}
