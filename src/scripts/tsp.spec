Summary:        TSP : library for a generic Transport Sampling Protocol
Name:           tsp
Version:        0.6.4
Release:        1
Group:          Applications/Communications
URL:            http://savannah.nongnu.org/projects/tsp
License:        LGPL
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root

%define bindir $RPM_BUILD_ROOT/opt/%{name}-%{version}
%define srcdir /%_topdir/SOURCES


#BuildRequires:  

%description
TSP : library for a generic Transport Sampling Protocol


%prep
rm -rf  
tar xvfz %{srcdir}/%{name}-%{version}.tar.gz 

%build
cd %{name}
./configure --disable-jtsp  --prefix=%{bindir}
source src/scripts/tsp_dev.login.sh
make

  
%install 
cd %{name}
mkdir -p /%{bindir}
cp -r exec/current/include %{bindir}
cp -r exec/current/linux/debug/* %{bindir}


%clean
rm -rf 



%files
%defattr(-,root,root,0644)
%attr(0755, root, root) /opt/%{name}-%{version}/bin/bb_destroy
%attr(0755, root, root) /opt/%{name}-%{version}/bin/bb_tsp_provider
%attr(0755, root, root) /opt/%{name}-%{version}/bin/tsp_stdout_client
%attr(0755, root, root) /opt/%{name}-%{version}/bin/tsp_tutorial_client
%attr(0755, root, root) /opt/%{name}-%{version}/bin/bb_dump
%attr(0755, root, root) /opt/%{name}-%{version}/bin/bb_write
%attr(0755, root, root) /opt/%{name}-%{version}/bin/tsp_stdout_client_callback
%attr(0755, root, root) /opt/%{name}-%{version}/bin/bb_read
%attr(0755, root, root) /opt/%{name}-%{version}/bin/tsp_ascii_writer
%attr(0755, root, root) /opt/%{name}-%{version}/bin/tsp_stub_server
%attr(0755, root, root) /opt/%{name}-%{version}/bin/bb_test
%attr(0755, root, root) /opt/%{name}-%{version}/bin/tsp_res_writer
%attr(0755, root, root) /opt/%{name}-%{version}/bin/tsp_testgrp_client
%attr(0644, root, root) /opt/%{name}-%{version}/include/bb_core.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/bb_sha1.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/bb_simple.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/bb_utils.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/glue_sserver.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_abs_types.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_const_def.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_consumer.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_datapool.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_datastruct.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_prjcfg.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_provider.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_provider_init.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_ringbuf.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_rpc.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_simple_trace.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_sys_headers.h
%attr(0644, root, root) /opt/%{name}-%{version}/include/tsp_time.h
%attr(0644, root, root) /opt/%{name}-%{version}/lib/libbb.a
%attr(0644, root, root) /opt/%{name}-%{version}/lib/libtsp_calc.a
%attr(0644, root, root) /opt/%{name}-%{version}/lib/libtsp_consumer.a
%attr(0644, root, root) /opt/%{name}-%{version}/lib/libtsp_page_config.a
%attr(0644, root, root) /opt/%{name}-%{version}/lib/libtsp_provider.a
%attr(0644, root, root) /opt/%{name}-%{version}/lib/libtsp_res.a
%attr(0644, root, root) /opt/%{name}-%{version}/lib/libtsp_services.a
%attr(0644, root, root) /opt/%{name}-%{version}/obj/bb_tsp_provider.o
%attr(0644, root, root) /opt/%{name}-%{version}/obj/bb_tsp_provider_main.o
%attr(0644, root, root) /opt/%{name}-%{version}/obj/glue_stub.o
%attr(0644, root, root) /opt/%{name}-%{version}/obj/server_main.o
%attr(0644, root, root) /opt/%{name}-%{version}/obj/tsp_ascii_writer.o
%attr(0644, root, root) /opt/%{name}-%{version}/obj/tsp_ascii_writer_config.tab.o
%attr(0644, root, root) /opt/%{name}-%{version}/obj/tsp_ascii_writer_config_yy.o
%attr(0644, root, root) /opt/%{name}-%{version}/obj/tsp_ascii_writer_main.o



%changelog
* Mon Mar 10 2005 Frederic Blanchet-Momas
- Initial RPM release.
