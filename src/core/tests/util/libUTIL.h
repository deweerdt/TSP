#ifndef _RES_INTERFACE_H
#define _RES_INTERFACE_H

extern int _use_dbl;

#define RES_COM_NUM	20
#define RES_COM_LEN	256

#define RES_VAR_NUM	3000
#define RES_NAME_LEN	100
#define RES_DESC_LEN	256

typedef void* d_rhandle;
typedef void* d_whandle;



#if defined(_LIBUTIL_REENTRANT) || defined(_LIBUTIL_COMPIL)

d_rhandle d_ropen_r(char *name, int* use_dbl);
int	d_rval_r(d_rhandle h, int c);
void	d_rnam_r(d_rhandle h,char *nam, char *desc, int i);
void	d_rcom_r(d_rhandle h,char *com, int i);
int	d_read_r(d_rhandle h,void *buf);
int	d_dread_r(d_rhandle h,void *buf, int i);
void	d_rclos_r(d_rhandle h);


d_whandle	d_wopen_r(char *name, int use_dbl);
void	d_wcom_r(d_whandle h,char *com);
void	d_wnam_r(d_whandle h,char *nam, char *des);
void	d_writ_r(d_whandle h,void *buf);
void	d_wclos_r(d_whandle h);

#endif

#if !defined(_LIBUTIL_REENTRANT) || defined(_LIBUTIL_COMPIL)

void     d_ropen(char *name);
int	d_rval(int c);
void	d_rnam(char *nam, char *desc, int i);
void	d_rcom(char *com, int i);
int	d_read(void *buf);
int	d_dread(void *buf, int i);

void	d_wopen (char *name);
void	d_wcom(char *com);
void	d_wnam(char *nam, char *des);
void	d_writ(void *buf);

void	d_clos(void);

#endif


#endif /* _RES_INTERFACE_H */
