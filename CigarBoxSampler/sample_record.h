/******************************************************************************/
/*  Flash Sampler Tutorial                                                    */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari, Dearraindrop               */
/******************************************************************************/
/*                                                                            */
/*  :  Sampler for the Critter Board with on-board flash or flash cards       */
/*                                                                            */
/******************************************************************************/


int sample_record_uart_init(void);

int sample_record_uart(void);

int sample_record_line_init(void);
int sample_record_line(void);


// check to stop recording
inline int get_rec_status();

// check rec enable
inline int get_rec_enable();

