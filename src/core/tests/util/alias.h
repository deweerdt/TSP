#ifndef _ALIAS_H_
#define _ALIAS_H_

#define		ALIAS_FILE	"alias.txt"
#define		ALIAS_MAX_NB	100000

extern int	alias_used;

extern void	init_aliases(int benchmark, int n_variables, char **namev);

#endif /* _ALIAS_H_ */
