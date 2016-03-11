
/* addr. of sound chip, used by OUTCHIP.ASM */
unsigned near genAddr=0x388;	


int BoardInstalled()
	{
	unsigned t1, t2, i;

	SndOutput (4, 0x60);    /* mask T1 & T2 */
	SndOutput (4, 0x80);    /* reset IRQ */
	t1 = inp (genAddr);     /* read status register */
	SndOutput (2, 0xff);    /* set timer-1 latch */
	SndOutput (4, 0x21);    /* unmask & start T1 */

	for (i = 0; i < 200; i++)   /* 100 uSec delay for timer-1 overflow */
		inp (genAddr);

	t2 = inp (genAddr);     /* read status register */
	SndOutput (4, 0x60);
	SndOutput (4, 0x80);

//	return(0);
	return (t1 & 0xE0) == 0 && (t2 & 0xE0) == 0xC0;
	}

