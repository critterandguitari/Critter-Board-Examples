/******************************************************************************/
/*  Flash Sampler Tutorial                                                    */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari, Dearraindrop               */
/******************************************************************************/
/*                                                                            */
/*  :  Sampler for the Critter Board with on-board flash or flash cards       */
/*                                                                            */
/******************************************************************************/


void sample_play_init(void);
void sample_play_stop(void);
void sample_play_start(void);
void sampler_change(unsigned int beg, unsigned int end, unsigned int speed, unsigned int dir);
