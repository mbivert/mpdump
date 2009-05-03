#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <libmpd/libmpd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xalloc.h"
#include "bool.h"

#define DEF_HOST "127.0.0.1"
#define DEF_PORT        6600

static bool verb = false;
static char *host = DEF_HOST;
static int port = DEF_PORT;

static void m_connect (MpdObj **);
static void song_dump (const mpd_Song *);
static void status_dump (const mpd_Status *);
static void usage (void);

static void
m_connect (MpdObj **mo) {
   *mo = mpd_new ((char *)host, port, NULL);
   if (mpd_connect (*mo) != MPD_OK) {
      mpd_free (*mo);
      fprintf (stderr, "Error: unable to connect mpd (%s:%d).\n", host, port);
      exit (EXIT_FAILURE);
   }
   else if (verb)
      printf ("Connected to mpd on (%s:%d).\n", host, port);
}

static void
song_dump (const mpd_Song *s) {
   assert (s != NULL);
   /* No need to check, _must_ be non-null */
   printf ("Filename\t%s\n", s->file);
   if (s->artist != NULL)
      printf ("Artist\t\t%s\n", s->artist);
   if (s->title != NULL)
      printf ("Title\t\t%s\n", s->title);
   if (s->album != NULL)
      printf ("Album\t\t%s\n", s->album);
   if (s->track != NULL)
      printf ("Track\t\t%s\n", s->track);
   if (s->name != NULL)
      printf ("Name\t\t%s\n", s->name);
   if (s->date != NULL)
      printf ("Date\t\t%s\n", s->date);
   if (s->genre != NULL)
      printf ("Genre\t\t%s\n", s->genre);
   if (s->time != MPD_SONG_NO_TIME)
      printf ("Length\t\t%d(seconds)\n", s->time);
}

static void
status_dump (const mpd_Status *st) {
   assert (st != NULL);
   printf ("Repeat\t\t%s\n", b2s ((bool)st->repeat));
   printf ("Random\t\t%s\n", b2s ((bool)st->random));
   printf ("Single\t\t%s\n", b2s ((bool)st->single));
   printf ("Consume\t\t%s\n", b2s ((bool)st->consume));
   printf ("Playlist length\t%d\n", st->playlistLength);
}

static void
usage (void) {
   fprintf (stderr, "Retrieve informations about mpd.\n Options:\n");
   fprintf (stderr, "\t-h display this help.\n");
   fprintf (stderr, "\t-w select a host (default:%s)\n", DEF_HOST);
   fprintf (stderr, "\t-p select a port (default:%d)\n", DEF_PORT);
   fprintf (stderr, "\t-v verbose output.\n");
}

int
main (int argc, char **argv) {
   int opt;

   MpdObj     *mo     = NULL;
   mpd_Song   *song   = NULL;
   mpd_Status *status = NULL;

   while ((opt = getopt (argc, argv, "hvw:p:")) != -1) {
      switch (opt) {
         case 'v':
            verb = true;
            break;
         case 'w':
            host = xmalloc (strlen (optarg) + 1);
            strcpy (host, optarg);
            break;
         case 'p':
            port = strtol (optarg, (char **)NULL, 10);
            if ((errno == ERANGE && (port == LONG_MAX || port == LONG_MIN))
                  || (errno != 0 && port == 0)) {
               perror ("strtol");
               exit (EXIT_FAILURE);
            }
            break;
         case 'h':
            usage ();
            exit (EXIT_SUCCESS);
            break;
         default:
            fprintf (stderr, "Error: invalid call\n");
            usage ();
            exit (EXIT_FAILURE);
            break;
      }
   }

   m_connect (&mo);
   song = xmalloc (sizeof (song));
   status = xmalloc (sizeof (status));
   if (mpd_player_get_state (mo) == MPD_PLAYER_PLAY) {
      song = mpd_playlist_get_current_song (mo);
      song_dump (song);
   }
   status_dump (status);

   mpd_free (mo);
   return EXIT_SUCCESS;
}
