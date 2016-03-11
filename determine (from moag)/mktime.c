#include	<stdio.h>
#include	<dos.h>

main(argc, argv)
	char	**argv;
{
	struct date	std;
	struct time	stt;
	long	then;
	extern char	*ctime();
	extern long	dostounix();

	if (argc < 4) {
		fprintf(stderr, "Usage:  %s year mon day [hour [min [sec]]]\n",
								argv[0]);
		fprintf(stderr, "  eg: Jan 1, 1991: is 1991 1 1\n");
		exit(1);
	}

	std.da_year = atoi(argv[1]);
	std.da_mon = atoi(argv[2]);
	std.da_day = atoi(argv[3]);

	if (argc >= 5)
		stt.ti_hour = atoi(argv[4]);
	else
		stt.ti_hour = 0;

	if (argc >= 6)
		stt.ti_min = atoi(argv[5]);
	else
		stt.ti_min = 0;

	if (argc >= 7)
		stt.ti_sec = atoi(argv[6]);
	else
		stt.ti_sec = 0;

	stt.ti_hund = 0;

	then = dostounix(&std, &stt);
	printf("%ld = %s", then, ctime(&then));
}
