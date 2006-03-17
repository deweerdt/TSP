# -*- makefile -*-

doxygen_input = tsp
static_pages = footer/footer.html header/header.html

htmldir = $(DEVBASE)/dist/doc/api/html
rtfdir = $(DEVBASE)/dist/doc/api/rtf
pdfdir = $(DEVBASE)/dist/doc/api/pdf

build::
	@echo "------TSP--Documentation Generation BEGIN----------"; \
	mkdir -p $(htmldir); \
	mkdir -p $(rtfdir); \
	mkdir -p $(pdfdir); \
	doxygen tsp; \
	cp tsp.css $(htmldir); \
	cp -r header $(htmldir); \
	cp -r footer $(htmldir); \
	cp -r images $(htmldir); \
	echo "-------TSP--Documentation Generation END------------";

destroy::
	$(RM) -rf $(htmldir)
	$(RM) -rf $(rtfdir)
	$(RM) -rf $(pdfdir)

