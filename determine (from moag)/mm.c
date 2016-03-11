char	badarg[] = "bad arg\n";
main(argc, argv)
	char	**argv;
{
	long	toleavel;
	unsigned int	total, toleave, tokeep;
	int	c;

	extern long	atol();
	extern unsigned int	farsetsize();

	if (argc != 2) {
		write(1, "arg count\n", 10);
		exit(1);
	}
	toleavel = atol(argv[1]);
	c = argv[1][strlen(argv[1]) - 1];
	if (c == 'K')
		toleavel *= 1024;
	else if (c < '0' || c > '9') {
		write(1, badarg, 8);
		exit(1);
	}
	toleave = toleavel/16;
	if (toleave < 4096) {		/* 64K */
		write(1, badarg, 8);
		exit(1);
	}

	total = farsetsize(0xffff);

	tokeep = total - toleave;
	tokeep -= 32;

/*	printf("total = %u  toleave = %u  tokeep = %u\n",total,toleave,tokeep);
	exit(0);
 */
	keep(0, tokeep);
}
