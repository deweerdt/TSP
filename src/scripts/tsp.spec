Summary:        TSP : library for a generic Transport Sampling Protocol
Name:           tsp
Version:        0.6.5
Release:        1
Group:          Applications/Communications
URL:            https://savannah.nongnu.org/projects/tsp
License:        LGPL
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root

%define prefix /usr/local
%define bindir $RPM_BUILD_ROOT%{prefix}
%define srcdir /%_topdir/SOURCES


#BuildRequires:  

%description
TSP : library for a generic Transport Sampling Protocol
TSP is both a protocol and a library available in C and Java
that may be used to observe evolutionary data. 
Main applications of TSP are test beds and simulator
realtime or not.

%prep
rm -rf  
tar xvfz %{srcdir}/%{name}-%{version}.tar.gz 

%build
cd %{name}
./configure --prefix=%{bindir} --enable-openGL
source src/scripts/tsp_dev.login.sh
make
  
%install 
cd %{name}
source src/scripts/tsp_dev.login.sh
make install

%clean
rm -rf 

%files
%defattr(-,root,root,0644)
%attr(0755, root, root) %{prefix}/tsp/bin/bb_tools
%attr(0755, root, root) %{prefix}/tsp/bin/bb_tsp_provider
%attr(0755, root, root) %{prefix}/tsp/bin/Visu3D
%attr(0755, root, root) %{prefix}/tsp/bin/bb_simu
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_ascii_writer
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_gdisp
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_gdisp+
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_res_reader
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_res_writer
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_rpc_cleanup
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_stdout_client
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_stdout_client_callback
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_stub_server
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_testgrp_client
%attr(0755, root, root) %{prefix}/tsp/bin/tsp_tutorial_client
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_checkid
%attr(0755, root, root) %{prefix}/tsp/scripts/psinfo.sh
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_destroy
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_dump
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_find
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_findlabel
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_memset
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_read
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_synchro_recv
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_synchro_send
%attr(0755, root, root) %{prefix}/tsp/scripts/bb_write
%attr(0755, root, root) %{prefix}/tsp/scripts/tsp_profile.csh
%attr(0755, root, root) %{prefix}/tsp/scripts/tsp_profile.sh
%attr(0644, root, root) %{prefix}/tsp/include/bb_core.h
%attr(0644, root, root) %{prefix}/tsp/include/bb_sha1.h
%attr(0644, root, root) %{prefix}/tsp/include/bb_simple.h
%attr(0644, root, root) %{prefix}/tsp/include/bb_utils.h
%attr(0644, root, root) %{prefix}/tsp/include/bb_tools.h
%attr(0644, root, root) %{prefix}/tsp/include/glue_sserver.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_abs_types.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_const_def.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_consumer.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_datapool.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_datastruct.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_prjcfg.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_provider.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_provider_init.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_ringbuf.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_rpc.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_simple_trace.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_sys_headers.h
%attr(0644, root, root) %{prefix}/tsp/include/tsp_time.h
%attr(0644, root, root) %{prefix}/tsp/include/calc_func.h
%attr(0644, root, root) %{prefix}/tsp/include/libUTIL.h
%attr(0644, root, root) %{prefix}/tsp/lib/libbb.a
%attr(0644, root, root) %{prefix}/tsp/lib/libtsp_calc.a
%attr(0644, root, root) %{prefix}/tsp/lib/libtsp_consumer.a
%attr(0644, root, root) %{prefix}/tsp/lib/libtsp_histo.a
%attr(0644, root, root) %{prefix}/tsp/lib/libtsp_page_config.a
%attr(0644, root, root) %{prefix}/tsp/lib/libtsp_provider.a
%attr(0644, root, root) %{prefix}/tsp/lib/libtsp_res.a
%attr(0644, root, root) %{prefix}/tsp/lib/libtsp_services.a

%changelog
* Tue Jul 19 2005 Erk
- Update spec file for TSP 0.6.5
* Mon Mar 10 2005 Frederic Blanchet-Momas
- Initial RPM release.

