# INESI
Library and application for getting information about `.nes` files.

## Usage

```sh
inesi rom.nes
```

## Setup
1. Launching new `bash` session having all required env. variables:
    ```sh
    ./configure
    ```
    _`chmod` might be necessary._

2. Building the binaries:
    ```sh
    make
    ```
    * `inesi` is dynamically linked _(won't run if `libinesi.so` isn't searchable via `LD_LIBRARY_PATH`)_.
    * `inesi_static` is self-contained.

3. Instalation:
    ```sh
    sudo -E make install
    ```
    _`-E` is required to pass the environment variables to the makefile._

    _Alternatively setting `PREFIX` should be enough._

## TODOs:
- [ ] Write tests.