.PHONY: clean format build-debug build-release
.ONESHELL:

BUILDIR:=buildir
GEN:=Ninja

clean:
	@rm -rf $(BUILDIR)

format:
	@bash format.sh source

build-debug:
	@cmake -S. -G$(GEN) -B$(BUILDIR) -DCMAKE_BUILD_TYPE=Debug
	@cmake --build $(BUILDIR)

build-release:
	@cmake -S. -G$(GEN) -B$(BUILDIR) -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(BUILDIR)
