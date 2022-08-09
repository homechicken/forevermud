# Makefile to fix permissions

.PHONY: all clean doc permissions

all: permissions

clean:
	make -C src clean
	make -C doc clean

permissions:
	make -C conf permissions
	make -C src permissions
	make -C data permissions
	@chmod 644 Makefile copying

