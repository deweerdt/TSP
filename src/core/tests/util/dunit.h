static	char	*dunit_sccsid={"@(#)dunit.h\t1.3\t00/09/29\tMATRA PROJET"};

struct dim angle = { 1, 0, 0, 0, 0, 0};
struct dim temps = { 0, 1, 0, 0, 0, 0};
struct dim leng  = { 0, 0, 1, 0, 0, 0};
struct dim mass  = { 0, 0, 0, 1, 0, 0};
struct dim coul  = { 0, 0, 0, 0, 1, 0};
struct dim amp   = { 0,-1, 0, 0, 1, 0}; /* coul/sec		*/
struct dim volt  = { 0,-2, 2, 1,-1, 0};	/* m2-kg/sec2-coul	*/
struct dim newt  = { 0,-2, 1, 1, 0, 0};	/* m2-kg/sec2-coul	*/
struct dic_unit {
	char	*name;
	double	fact;
	struct	dim	*dim;
	} dic[] = {
	"deg",	1.745329252e-2,	&angle,
	"rad",	1.,		&angle,
	"micrad",1.e-6,		&angle,
	"mrad",1.e-3,		&angle,
	"hr",	3600,		&temps,
	"s",	1.,		&temps,
	"ms",	1.e-3,		&temps,
	"m",	1.,		&leng,
	"km",	1.e3,		&leng,
	"A",	1.,		&amp,
	"mA",	1.e-3,		&amp,
	"V",	1.,		&volt,
	"mV",	1.e-3,		&volt,
	"gr",	1.e-3,		&mass,
	"N",	1.,		&newt,
	"kg",	1.,		&mass
};

#define NUNIT (sizeof(dic)/sizeof(dic[0]))

