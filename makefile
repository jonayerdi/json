
project = json
output_dir = Debug
executable = $(project).exe
pdb = $(project).pdb
cfiles = json.c
cc = cl
cflags = /W3 /GS /Gd /Zi /Od /Ob0 /MDd /TC /Fo: $(output_dir)/ /Fe: $(output_dir)/$(executable) /Fd: $(output_dir)/$(pdb)

build: $(cfiles) $(output_dir)
  $(cc) $(cflags) $(cfiles) 

remove:
  del /Q $(output_dir)

clean:
  del /Q $(output_dir)\*.obj

$(output_dir):
  mkdir $(output_dir)

.PHONY: build, remove, clean, $(output_dir)
