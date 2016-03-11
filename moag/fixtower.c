#include	<stdio.h>
#include	<io.h>
#include	<fcntl.h>
#include	<sys/stat.h>

#define	MYBUFSIZE	10240
unsigned char	buf[MYBUFSIZE];
main() {
	unsigned char	*p;
	int	fd, i, n;
	long	sum;
	struct stat	sb;
	extern void	exit();
	extern int	_fmode;

	_fmode = O_BINARY;
	fd = open("moag.exe", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "fixtower:  Cannot open tower!\n");
		exit(1);
	}
	sum = 0;
	while ((n = read(fd, (void *)buf, MYBUFSIZE)) > 0) {
		for (i = 0, p = buf; i < n; i++)
			sum += *p++;
	}
	close(fd);
	fprintf(stderr, "fixtower:  sum=%ld\n", sum);

	if (stat("moag.exe", &sb) < 0) {
		fprintf(stderr, "fixtower:  Cannot stat moag.exe!\n");
		exit(1);
	}

	if ((fd = open("tower.exe", O_RDWR)) < 0) {
		fprintf(stderr, "fixtower:  Cannot open tower.exe!\n");
		exit(1);
	}
	lseek(fd, 0L, SEEK_END);
	if (write(fd, (void *)&sb.st_size, sizeof(long)) != sizeof(long) ||
	    write(fd, (void *)&sum, sizeof(long)) != sizeof(long)) {
		fprintf(stderr, "fixtower:  write error on tower.exe\n");
		exit(1);
	}
	exit(0);
}
