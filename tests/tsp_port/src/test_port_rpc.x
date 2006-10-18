enum TSP_port_enum {
  TSP_port_e0,
  TSP_port_e1,
  TSP_port_e2,
  TSP_port_e3,
  TSP_port_e4
};

struct TSP_port_struct {
   int    i;
   double a;
   string status_str<>;
};

program TSP_PORT {
	version TSP_PORT_VERS {
		TSP_port_enum   get_next(TSP_port_enum) = 1;
		TSP_port_struct get(void)               = 2;
		int             set(TSP_port_struct)    = 3;
	} = 1;
} = 77;

