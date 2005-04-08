char *strdup (const char *s1);
unsigned long long strtoull(const char *str, char **endptr, int base);
long long strtoll(const char *str, char **endptr, int base);
int vsnprintf(char *s, size_t n, const char *format,  va_list ap);
int snprintf(char *s,  size_t  n,  const  char  *format,  /*args*/ ...);
