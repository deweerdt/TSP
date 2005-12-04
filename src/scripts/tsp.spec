Summary:        TSP : library for a generic Transport Sampling Protocol
Name:           tsp
Version:        0.7.1cvs
Release:        1
Group:          Applications/Communications
URL:            https://savannah.nongnu.org/projects/tsp
License:        LGPL
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root

%define prefix /opt/tsp
%define rpmprefix $RPM_BUILD_ROOT%{prefix}
%define srcdir /%_topdir/SOURCES


#BuildRequires:  

%description
TSP : library for a generic Transport Sampling Protocol
TSP is both a protocol and a library available in C and Java
that may be used to observe evolutionary data. 
Main applications of TSP are test beds and simulator
be it realtime or not.

%prep
rm -rf  
tar xvfz %{srcdir}/%{name}-%{version}.tar.gz 

%build
cd %{name}
./configure --prefix=%{rpmprefix} --enable-openGL
source src/scripts/tsp_dev.login.sh
make
  
%install 
cd %{name}
source src/scripts/tsp_dev.login.sh
make install

%clean
rm -rf 

%post
if [ -d  /etc/profile.d ]; then
  cd /etc/profile.d
  ln -s /opt/tsp/scripts/tsp_profile.sh
  ln -s /opt/tsp/scripts/tsp_profile.csh
fi


%preun
if [ -d  /etc/profile.d ]; then
  cd /etc/profile.d
  rm tsp_profile.sh
  rm tsp_profile.csh
fi


%files
%defattr(-,root,root,0644)
%attr(0755, root, root) %{prefix}/bin/*
%attr(0755, root, root) %{prefix}/scripts/*
%attr(0644, root, root) %{prefix}/include/*.h
%attr(0644, root, root) %{prefix}/lib/lib*

%changelog
* Fri Dec 1 2005 Erk
  - TSP 0.7.1cvs
    We should change the version as soon as preceding version
    released in order to avoid CVS and real version mismatch
* Fri Nov 1 2005 Erk
  - TSP 0.7.0
* Fri Oct 14 2005 Erk
  - add link to /etc/profile.d/ during post
  - delete those link during preun
* Thu Oct 13 2005 Erk
  - bump to 0.7.0pre1
* Tue Sep 6  2005 Erk
  - use shell wildcard in order to select files as suggested
    by Fred. B.M. in order to avoid to try to install non generated
    binaries on box where some devel libraries were not detected
    by the configure script.
* Sun Aug 28 2005 Erk
  - bb_create and bb_publish
* Sun Aug 14 2005 Erk
  - Added new bb_check_version and rename bb_checkid --> bb_check_id
  - Change install path location prefix from /usr/local/tsp
    to /opt/tsp which is the recommanded path from 
    FHS 2.3 (http://www.pathname.com/fhs/) and
    LSB 3.0 (http://www.linuxbase.org/ or 
             http://lsbbook.gforge.freestandards.org/install-app.html)
* Tue Jul 19 2005 Erk
  - Update spec file for TSP 0.6.5
* Mon Mar 10 2005 Frederic Blanchet-Momas
  - Initial RPM release.

