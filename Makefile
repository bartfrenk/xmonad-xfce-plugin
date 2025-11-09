BUILDDIR := build

all: build install

clean:
	rm -rf $(BUILDDIR)

build:
	meson setup --prefix=/usr $(BUILDDIR)

install:
	sudo meson install -C $(BUILDDIR)
