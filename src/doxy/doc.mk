#
# Generated by the configure script for:
# @PACKAGE_NAME@ version @PACKAGE_VERSION@
DEST=$(DEVBASE)/doc/html 

all::
		echo "------TSP--Documentation Generation BEGIN----------\n"; \
		doxygen tsp; \
		cp doxygen.css $(DEST); \
		cp header/*.* $(DEST); \
		cp footer/*.* $(DEST); \
		echo "-------TSP--Documentation Generation END------------\n";


clean:
