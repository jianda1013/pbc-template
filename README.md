# pbc docker template

Make a template for pbc library for docker
[pbc library](https://crypto.stanford.edu/pbc/manual/)

## volume
mount `./src` in container's `/usr/src`

## alias

- **debug** : compile main.c with `-DDEBUG` mode
- **build** : compile main.c
- **run** : run a.out with input `a.param` as stdin

## makefile command

- `make` : `start` + `exec`
- `start` : build the docker container running background
- `clean` : shutdown the container and remove
- `exec` : run `bash` in container
- `re` : `clean` + `start`
