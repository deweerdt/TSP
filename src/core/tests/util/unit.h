
static	char	*unit_h_sccsid={"@(#)unit.h\t1.3\t00/09/29\tMATRA PROJET"};

struct dim {
	int	t[6];
	};

struct sunit {
	struct dim d;
	double conv;
	} ;

struct sunit	si_unit (char *unit);
int		add_dim (struct dim *a, struct dim *b, int ordr);
int		cmp_unit (struct dim *a, struct dim *b);

