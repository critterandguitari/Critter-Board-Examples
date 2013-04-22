/******************************************************************************/
/*  audio.h                                                       */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  audio.h  8 bit FM synth voices for Critte Board                        */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

void synth_stop(void);
void synth_init(void);

void fm_voice(int voice, int freq, int index, int harmonicity);




